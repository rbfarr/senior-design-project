#include "vidstab.h"

using namespace cv;
using namespace std;

Mat estimateMotion(Mat* frame, Scalar means[][NUM_DIVS][2], Scalar avg_means[][NUM_DIVS][2], Scalar diffs[][NUM_DIVS][2]) {

  Mat gradX, gradY;
  Mat frame_bw;

  Mat ang_mat = Mat::zeros(frame->rows, frame->cols, CV_32F);
  Mat mag_mat = Mat::zeros(frame->rows, frame->cols, CV_32F);

  // [][][0] = ang
  // [][][1] = mag
  Scalar prev_means[NUM_DIVS][NUM_DIVS][2];
  Scalar prev_avg_means[NUM_DIVS][NUM_DIVS][2];

  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;

  // cvt to grayscale
  cvtColor(*frame, frame_bw, CV_RGB2GRAY);
  GaussianBlur(frame_bw, frame_bw, Size(25, 25), 0, 0);
  
  // divide frame into cropped portions
  int hdiv = frame->rows/NUM_DIVS;
  int wdiv = frame->cols/NUM_DIVS;

  Mat cropped[NUM_DIVS][NUM_DIVS];
  Rect crop;

  for(int h = 0; h < NUM_DIVS; h++) {
    for(int w = 0; w < NUM_DIVS; w++) {
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
      mag_diff = prev_avg_means[h][w][1] - avg_means[h][w][1]; 

      diffs[h][w][0] = ang_diff;
      diffs[h][w][1] = mag_diff;
      
      // RESULTS
      Point center = Point(cropped[h][w].cols/2, cropped[h][w].rows/2);
      Point dest_point = Point(cos(means[h][w][0][0])*means[h][w][1][0], sin(means[h][w][0][0])*means[h][w][1][0]);
      //Point dest_point = Point(cos(avg_means[h][w][0][0])*avg_means[h][w][1][0], sin(avg_means[h][w][0][0])*avg_means[h][w][1][0]);
      //Point dest_point = Point(cos(ang_diff[0])*mag_diff[0], sin(ang_diff[0])*mag_diff[0]);
      //Point dest_point_x = Point(cos(ang_diff[0])*mag_diff[0], 0);
      //Point dest_point_y = Point(0, sin(ang_diff[0])*mag_diff[0]);

      //line(cropped[h][w], center, ((10*dest_point)+center), Scalar(0, 255, 255));
      //line(cropped[h][w], center, ((1*dest_point_x)+center), Scalar(0, 255, 255));
      //line(cropped[h][w], center, ((1*dest_point_y)+center), Scalar(0, 255, 255));

      // reassemble frame 
      cropped[h][w].copyTo( frame_bw(Rect(w*wdiv, h*hdiv, wdiv, hdiv) ));
    }
  }
  
  //return frame_gm;

  return frame_bw;
}

int main(int argc, char* argv[]) {
  Mat im, im2;
  im = imread("./test.jpg", CV_LOAD_IMAGE_COLOR);
  im2 = imread("./test2.jpg", CV_LOAD_IMAGE_COLOR);

  if(!im.data || !im2.data) {
    cout << "Errs err'where. No image data." << endl;
    return -1;
  }

  Mat ang_mat = Mat::zeros(im.rows, im.cols, CV_32F);
  Mat mag_mat = Mat::zeros(im.rows, im.cols, CV_32F);

  // [][][0] = ang
  // [][][1] = mag
  Scalar means[NUM_DIVS][NUM_DIVS][2];
  Scalar prev_means[NUM_DIVS][NUM_DIVS][2];
  Scalar avg_means[NUM_DIVS][NUM_DIVS][2];
  Scalar prev_avg_means[NUM_DIVS][NUM_DIVS][2];

  Scalar diffs[NUM_DIVS][NUM_DIVS][2];

  Mat motion_im = estimateMotion(&im, means, prev_means, diffs);
  Mat motion_im2 = estimateMotion(&im2, means, prev_means, diffs);

  motion_im.convertTo(motion_im, CV_32FC1);
  motion_im2.convertTo(motion_im2, CV_32FC1);

  resize(motion_im2, motion_im2, motion_im.size());

  Point2d phase_diff;
  phase_diff = phaseCorrelate(motion_im, motion_im2);
  cout << phase_diff << endl;

  Point2d center = Point(motion_im.cols/2, motion_im.rows/2);

  line(motion_im2, center, (phase_diff+center), Scalar(0, 255, 255));

  Mat warp_mat = ( Mat_<double>(2, 3) << 1, 0, (1*phase_diff.x), 0, 1, (1*phase_diff.y) );
  warpAffine(motion_im2, motion_im2, warp_mat, motion_im2.size(), WARP_INVERSE_MAP);

  // display the current frame
  for(;;) {
    imshow("im", motion_im/255);
    imshow("im2", motion_im2/255);

    if(waitKey(30) >= 0) break;
  }

  return 0;
}
