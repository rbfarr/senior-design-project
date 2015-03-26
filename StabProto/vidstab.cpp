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

  Mat frame;
  Mat prev_frame;
  bool tSuccess = capture.read(frame);
  if(!tSuccess) {
    cout << "Cannot grab test frame from capture." << endl;
    exit(0);
  }

  Mat prev_gradient = frame.clone();
  cvtColor(prev_gradient, prev_gradient, CV_RGB2GRAY);
  prev_gradient.convertTo(prev_gradient, CV_32FC1);

  for(;;) {
    Mat comp_frame;

    //prev_frame = frame.clone();

    // read the capture into a frame
    bool rSuccess = capture.read(frame);
    if(!rSuccess) {
      cout << "Cannot grab frame from capture." << endl;
      break;
    }
    
    comp_frame = estimateMotion(&frame, &prev_gradient);

    // display the current frame
    imshow("frame", frame);
    imshow("comp_frame", comp_frame);

    if(waitKey(30) >= 0) break;
  }
  return 0;
}
