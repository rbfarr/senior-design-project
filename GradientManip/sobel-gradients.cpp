#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

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
  Mat prev_ang_mat = ang_mat;
  Mat prev_mag_mat = mag_mat;
  Scalar ang_mean;
  Scalar mag_mean;
  Scalar prev_ang_mean;
  Scalar prev_mag_mean;
  Scalar mag_diff;
  Scalar ang_diff;

  Mat gradient, gradX, gradY;
  Mat prev_gradient, prev_gradX, prev_gradY;
  Mat diff_grad;
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
    
    prev_ang_mat = ang_mat;
    prev_mag_mat = mag_mat;

    prev_ang_mean = ang_mean;
    prev_mag_mean = mag_mean;

    prev_gradX = gradX;
    prev_gradY = gradY;
    prev_gradient = gradient;

    // Sobel operators - X & Y
    Sobel(frame_bw, gradX, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
    Sobel(frame_bw, gradY, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);

    convertScaleAbs(gradX, abs_gradX);
    convertScaleAbs(gradY, abs_gradY);

    addWeighted(abs_gradX, 0.5, abs_gradY, 0.5, 0, gradient);

    // DIFF
    //absdiff(prev_gradient, gradient, diff_grad);
    //diff_grad = gradient - prev_gradient;

    // PHASE
    phase(gradX, gradY, ang_mat, true);
    ang_mean = mean(ang_mat);
    // MAGNITUDE
    magnitude(gradX, gradY, mag_mat);
    mag_mean = mean(mag_mat);

    // PHASE and MAG DIFF
    ang_diff = prev_ang_mean - ang_mean; 
    mag_diff = prev_mag_mean - mag_mean; 

    cout << "Gradient Direction: " << ang_diff[0] << "\t" 
         << "Gradient Magnitude: " << mag_diff[0] << "\r";
    cout << flush;

    Point center = Point(frame.cols/2, frame.rows/2);
    Point dest_point = Point(cos(ang_mean[0])*mag_mean[0], sin(ang_mean[0])*mag_mean[0]);
    //Point dest_point = Point(cos(ang_diff[0])*mag_diff[0], sin(ang_diff[0])*mag_diff[0]);
    line(frame, center, ((50*dest_point)+center), Scalar(0, 255, 255));

    // display the current frame
    imshow("frame", frame);

    if(waitKey(30) >= 0) break;
  }
  return 0;
}
