#include "vidstab.h"

using namespace cv;
using namespace std;

int main() {
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
  int num_divs = 1;

  // [][][0] = ang
  // [][][1] = mag
  Scalar means[num_divs][num_divs][2];
  Scalar prev_means[num_divs][num_divs][2];
  Scalar avg_means[num_divs][num_divs][2];
  Scalar prev_avg_means[num_divs][num_divs][2];

  Mat gradient, gradX, gradY;
  Mat prev_gradX, prev_gradY;
  Mat diff_gradX, diff_gradY;
  Mat abs_gradX, abs_gradY;

  while(1) {
    Mat frame, frame_bw;

    // read the capture into a frame
    bool rSuccess = capture.read(frame);
    if(!rSuccess) {
      cout << "Cannot grab frame from capture." << endl;
      break;
    }

    int scale = 1;
    int delta = 0;
    int ddepth = CV_32F;

    // cvt to grayscale
    cvtColor(frame, frame_bw, CV_RGB2GRAY);
    GaussianBlur(frame_bw, frame_bw, Size(25, 25), 0, 0);
    
    // divide frame into cropped portions
    int hdiv = frame.rows/num_divs;
    int wdiv = frame.cols/num_divs;

    Mat cropped[num_divs][num_divs];
    Rect crop;

    double alpha = .03;

    for(int h = 0; h < num_divs; h++) {
      for(int w = 0; w < num_divs; w++) {
        // ang, mag
        prev_means[h][w][0] = means[h][w][0];
        prev_means[h][w][1] = means[h][w][1];
        prev_avg_means[h][w][0] = avg_means[h][w][0];
        prev_avg_means[h][w][1] = avg_means[h][w][1];

        //abs_gradX.copyTo(prev_gradX);
        //abs_gradY.copyTo(prev_gradY);
        gradX.copyTo(prev_gradX);
        gradY.copyTo(prev_gradY);

        avg_means[h][w][0] = (alpha * means[h][w][0])+(1 - alpha) * avg_means[h][w][0];
        avg_means[h][w][1] = (alpha * means[h][w][1])+(1 - alpha) * avg_means[h][w][1];

        crop = Rect(w*wdiv, h*hdiv, wdiv, hdiv);
        cropped[h][w] = Mat(frame_bw, crop);

        // Sobel operators - X & Y
        Sobel(cropped[h][w], gradX, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
        Sobel(cropped[h][w], gradY, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);

        //convertScaleAbs(gradX, gradX);
        //convertScaleAbs(gradY, gradY);

        addWeighted(abs_gradX, 0.5, abs_gradY, 0.5, 0, gradient);

        // PHASE
        //phase(gradX, gradY, ang_mat, true);
        //means[h][w][0] = mean(ang_mat);

        // MAGNITUDE
        //magnitude(gradX, gradY, mag_mat);
        //means[h][w][1] = mean(mag_mat);

        // PHASE and MAG DIFF
        Mat _ang_diff, _mag_diff = Mat::zeros(gradX.rows, gradX.cols, CV_8U);
        Scalar ang_diff, mag_diff = Scalar(0);
        //ang_diff = prev_means[h][w][0] - means[h][w][0]; 
        //mag_diff = prev_means[h][w][1] - means[h][w][1]; 
        //ang_diff = prev_avg_means[h][w][0] - avg_means[h][w][0]; 
        //mag_diff = prev_avg_means[h][w][1] - avg_means[h][w][1]; 
        if(!prev_gradX.empty() && !prev_gradY.empty()) {
          absdiff(prev_gradX, gradX, diff_gradX);
          absdiff(prev_gradY, gradY, diff_gradY);

          // TODO: need manual calculation?
          //diff_gradX.convertTo(diff_gradX, CV_32F);
          //diff_gradY.convertTo(diff_gradY, CV_32F);
          //cartToPolar(diff_gradX, diff_gradY, _mag_diff, _ang_diff, true);
          
          for(int i; i < diff_gradX.rows; i++) {
            for(int j; j < diff_gradX.cols; j++) {
              double dir_r = atan2( diff_gradY.at<uchar>(j, i), diff_gradX.at<uchar>(j, i) );
              double dir_d = dir_r / PI * 180;
              //cout << "\r" << dir_d;
              //cout << flush;
              //_ang_diff.at<uchar>(i, j) = dir_d;
              //double magnitude = sqrt( pix_x[i] + pix_y[i]^2 );
            }
          }
          
          //ang_diff = mean(_ang_diff);
          //mag_diff = mean(_mag_diff);
          imshow("DX", diff_gradX);
        }

        //cout << "Gradient Direction: " << ang_diff[0] << "\t" 
        //     << "Gradient Magnitude: " << mag_diff[0] << "\r";
        //cout << flush;

        Point center = Point(cropped[h][w].cols/2, cropped[h][w].rows/2);
        //Point dest_point = Point(cos(means[h][w][0][0])*means[h][w][1][0], sin(means[h][w][0][0])*means[h][w][1][0]);
        //Point dest_point = Point(cos(avg_means[h][w][0][0])*avg_means[h][w][1][0], sin(avg_means[h][w][0][0])*avg_means[h][w][1][0]);
        Point dest_point = Point(cos(ang_diff[0])*mag_diff[0], sin(ang_diff[0])*mag_diff[0]);
        line(cropped[h][w], center, ((1*dest_point)+center), Scalar(0, 255, 255));

        cropped[h][w].copyTo( frame_bw(Rect(w*wdiv, h*hdiv, wdiv, hdiv) ));
      }
    }

    // display the current frame
    imshow("frame", frame_bw);

    if(waitKey(30) >= 0) break;
  }
  return 0;
}
