#include "vidstab.h"

using namespace cv;
using namespace std;

extern Point2d phase_diff;

int main(int argc, char* argv[]) {
  // opens video capture - in this case, 0 represents default webcam
  VideoCapture capture(0);

  if(!capture.isOpened()) {
    cout << "Cannot open default camera. Exiting..." << endl;
    exit(0);
  }

  // Control Variables
  int return_rate = 1;
  int stabilize = 1;
  int display_grad = 0;

  Mat frame;
  Mat prev_frame;
  bool tSuccess = capture.read(frame);
  if(!tSuccess) {
    cout << "Cannot grab test frame from capture." << endl;
    exit(0);
  }

  // Create a named window
  namedWindow("Stabilized Frame");
  namedWindow("Unstabilized Frame");

  // Create a return rate trackbar
  if(!createTrackbar("Return Rate", "Stabilized Frame", &return_rate, 100)){
    cout << "Cannot create trackbar." << endl;
    exit(0);
  }

  // Create buttons to control stabilization
  if(!createTrackbar("Stabilize", "Stabilized Frame", &stabilize, 1)){
    cout << "Cannot create stabilize button." << endl;
    exit(0);
  }

  // Create trackbar to display gradients
  if(!createTrackbar("Display Gradients", "Stabilized Frame", &display_grad, 1)){
    cout << "Cannot create disp_grad button." << endl;
    exit(0);
  }

  Mat prev_gradient = frame.clone();
  Mat unstab = frame.clone();
  cvtColor(prev_gradient, prev_gradient, CV_RGB2GRAY);
  prev_gradient.convertTo(prev_gradient, CV_32FC1);

  for(;;) {

    prev_frame = frame.clone();
    unstab = frame.clone();

    // read the capture into a frame
    bool rSuccess = capture.read(frame);
    if(!rSuccess) {
      cout << "Cannot grab frame from capture." << endl;
      break;
    }
   
    frame = estimateMotion(&frame, &unstab, &prev_gradient, return_rate, stabilize, display_grad);
   
    // display the current frame
    imshow("Stabilized Frame", frame);
    imshow("Unstabilized Frame", unstab);

    if(waitKey(30) >= 0) break;
  }
  cout << endl;
  return 0;
}
