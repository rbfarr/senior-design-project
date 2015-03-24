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

  Mat ang_mat = Mat::zeros(prev_frame.rows, prev_frame.cols, CV_32F);
  Mat mag_mat = Mat::zeros(prev_frame.rows, prev_frame.cols, CV_32F);

  // [][][0] = ang
  // [][][1] = mag
  Scalar means[NUM_DIVS][NUM_DIVS][2];
  Scalar prev_means[NUM_DIVS][NUM_DIVS][2];
  Scalar avg_means[NUM_DIVS][NUM_DIVS][2];
  Scalar prev_avg_means[NUM_DIVS][NUM_DIVS][2];

  Scalar diffs[NUM_DIVS][NUM_DIVS][2];

  for(;;) {
    Mat motion_frame;

    prev_frame = frame.clone();

    // read the capture into a frame
    bool rSuccess = capture.read(frame);
    if(!rSuccess) {
      cout << "Cannot grab frame from capture." << endl;
      break;
    }
    
    motion_frame = estimateMotion(&frame, &prev_frame, means, avg_means, diffs);

    compensateMotion(&motion_frame, diffs);

    // display the current frame
    //imshow("motion_frame", motion_frame);
    imshow("frame", frame);

    if(waitKey(30) >= 0) break;
  }
  return 0;
}
