#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
  // opens video capture - in this case, 0 represents default webcam
  VideoCapture capture(0);

  if(!capture.isOpened()) 
    cout << "Cannot open default camera." << endl;

  while(1) {
    Mat frame;
    
    // read the capture into a frame
    bool rSuccess = capture.read(frame);
    if(!rSuccess) {
      cout << "Cannot grab frame from capture." << endl;
      break;
    }
    
    // ---------------------------------//
    // insert code manipulating frame here
    // ---------------------------------//

    // display the current frame
    imshow("default webcam", frame);
    if(waitKey(30) >= 0) break;
  }
  return 0;
}
