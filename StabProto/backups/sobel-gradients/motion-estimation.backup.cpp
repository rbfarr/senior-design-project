#include "vidstab.h"

using namespace cv;
using namespace std;

int main(int argc, char* argv[]) {
  // opens video capture - in this case, 0 represents default webcam
  VideoCapture capture(0);

  if(!capture.isOpened()) {
    cout << "Cannot open default camera. Exiting..." << endl;
    exit(0);
  }

  Mat testframe;
  bool tSuccess = capture.read(testframe);
  if(!tSuccess) {
    cout << "Cannot grab test frame from capture." << endl;
    exit(0);
  }

  Mat ang_mat = Mat::zeros(testframe.rows, testframe.cols, CV_32F);
  Mat mag_mat = Mat::zeros(testframe.rows, testframe.cols, CV_32F);

  // num_divs of frame for LMVs
  int num_divs = 16;

  // [][][0] = ang
  // [][][1] = mag
  Scalar means[num_divs][num_divs][2];
  Scalar prev_means[num_divs][num_divs][2];
  Scalar avg_means[num_divs][num_divs][2];
  Scalar prev_avg_means[num_divs][num_divs][2];

  Mat gradX, gradY;

  for(;;) {
    Mat frame, frame_bw;

    // read the capture into a frame
    bool rSuccess = capture.read(frame);
    if(!rSuccess) {
      cout << "Cannot grab frame from capture." << endl;
      break;
    }

    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;

    // cvt to grayscale
    cvtColor(frame, frame_bw, CV_RGB2GRAY);
    GaussianBlur(frame_bw, frame_bw, Size(25, 25), 0, 0);
    
    // divide frame into cropped portions
    int hdiv = frame.rows/num_divs;
    int wdiv = frame.cols/num_divs;

    Mat cropped[num_divs][num_divs];
    Rect crop;

    for(int h = 0; h < num_divs; h++) {
      for(int w = 0; w < num_divs; w++) {
        // ang, mag
        prev_means[h][w][0] = means[h][w][0];
        prev_means[h][w][1] = means[h][w][1];
        prev_avg_means[h][w][0] = avg_means[h][w][0];
        prev_avg_means[h][w][1] = avg_means[h][w][1];

        // recalculate running average
        avg_means[h][w][0] = (ALPHA * means[h][w][0])+(1 - ALPHA) * avg_means[h][w][0];
        avg_means[h][w][1] = (ALPHA * means[h][w][1])+(1 - ALPHA) * avg_means[h][w][1];

        // crop frame
        crop = Rect(w*wdiv, h*hdiv, wdiv, hdiv);
        cropped[h][w] = Mat(frame_bw, crop);

        // Sobel operators - X & Y
        Sobel(cropped[h][w], gradX, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
        Sobel(cropped[h][w], gradY, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);

        // THRESHOLD NOISE
        convertScaleAbs(gradX, gradX);
        convertScaleAbs(gradY, gradY);
        threshold(gradX, gradX, THRESH_NOISE, 255, THRESH_TOZERO);
        threshold(gradY, gradY, THRESH_NOISE, 255, THRESH_TOZERO);
        gradX.convertTo(gradX, CV_32F);
        gradY.convertTo(gradY, CV_32F);

        // PHASE
        phase(gradX, gradY, ang_mat, true);
        means[h][w][0] = mean(ang_mat);

        // MAGNITUDE
        magnitude(gradX, gradY, mag_mat);
        means[h][w][1] = mean(mag_mat);

        // PHASE and MAG DIFF
        Scalar ang_diff, mag_diff = Scalar(0);
        ang_diff = prev_avg_means[h][w][0] - avg_means[h][w][0]; 
        mag_diff = prev_avg_means[h][w][1] - means[h][w][1]; 
        
        // RESULTS`
        Point center = Point(cropped[h][w].cols/2, cropped[h][w].rows/2);
        //Point dest_point = Point(cos(means[h][w][0][0])*means[h][w][1][0], sin(means[h][w][0][0])*means[h][w][1][0]);
        //Point dest_point = Point(cos(avg_means[h][w][0][0])*avg_means[h][w][1][0], sin(avg_means[h][w][0][0])*avg_means[h][w][1][0]);
        Point dest_point = Point(cos(ang_diff[0])*mag_diff[0], sin(ang_diff[0])*mag_diff[0]);
        line(cropped[h][w], center, ((1*dest_point)+center), Scalar(0, 255, 255));

        // reassemble frame 
        cropped[h][w].copyTo( frame_bw(Rect(w*wdiv, h*hdiv, wdiv, hdiv) ));
      }
    }

    // display the current frame
    imshow("frame", frame_bw);

    if(waitKey(30) >= 0) break;
  }
  return 0;
}
