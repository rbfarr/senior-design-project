#include <iostream>
#include <iomanip>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/core/core.hpp"

#define PI 3.14159265359
#define ALPHA 0.1
//#define THRESH_NOISE 25
#define THRESH_NOISE 0

// LMV tolerance compared to GM
#define GM_TOL 0.55

// num_divs of frame for LMVs
#define NUM_DIVS 1

using namespace cv;
using namespace std;

// returned Scalars represent x and y components of motion
Mat estimateMotion(Mat*, Mat*, Scalar[][NUM_DIVS][2], Scalar[][NUM_DIVS][2], Scalar[][NUM_DIVS][2]);
Scalar getGlobalMotion(Mat*, Scalar[][NUM_DIVS][2]);
void compensateMotion(Mat*, Scalar[][NUM_DIVS][2]);
Scalar weighMotion(Scalar, Scalar[][NUM_DIVS][2]);
