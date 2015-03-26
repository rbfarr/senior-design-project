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

  // [][][0] = ang
  // [][][1] = mag
  Scalar means[NUM_DIVS][NUM_DIVS][2];
  Scalar prev_means[NUM_DIVS][NUM_DIVS][2];
  Scalar avg_means[NUM_DIVS][NUM_DIVS][2];
  Scalar prev_avg_means[NUM_DIVS][NUM_DIVS][2];

  Scalar diffs[NUM_DIVS][NUM_DIVS][2];

  for(;;) {
    Mat frame, motion_frame;

    // read the capture into a frame
    bool rSuccess = capture.read(frame);
    if(!rSuccess) {
      cout << "Cannot grab frame from capture." << endl;
      break;
    }

    motion_frame = estimateMotion(&frame, means, avg_means, diffs);

    compensateMotion(&motion_frame, diffs);

    // display the current frame
    imshow("motion_frame", motion_frame);
    imshow("frame", frame);

    if(waitKey(30) >= 0) break;
  }
  return 0;
}
