#include "vidstab.h"

using namespace cv;
using namespace std;

//Point2d phase_diff[NUM_DIVS][NUM_DIVS];
Point2d phase_diff = Point2d(0);
Point2d phase_diff2 = Point2d(0);

// returns frame offset for compensation
Mat estimateMotion(Mat* frame, Mat* unstab, Mat* prev_gradient, int return_rate, int stab, int disp_grad) {

  Mat gradX, gradY;
  Mat frame_bw;

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
  Mat orig_cropped[NUM_DIVS][NUM_DIVS];

  Mat prev_cropped;
  Rect crop;

  Mat comp_frame = (*frame).clone();

  frame->copyTo((*unstab));

  for(int h = 0; h < NUM_DIVS; h++) {
    for(int w = 0; w < NUM_DIVS; w++) {
      // crop frame
      crop = Rect(w*wdiv, h*hdiv, wdiv, hdiv);

      orig_cropped[h][w] = Mat(*frame, crop);
      cropped[h][w] = Mat(frame_bw, crop);

      prev_cropped = Mat(*prev_gradient, crop);

      // Sobel operators - X & Y
      Sobel(cropped[h][w], gradX, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
      Sobel(cropped[h][w], gradY, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);

      // THRESHOLD NOISE
      convertScaleAbs(gradX, gradX);
      convertScaleAbs(gradY, gradY);

      threshold(gradX, gradX, THRESH_NOISE, 255, THRESH_TOZERO);
      threshold(gradY, gradY, THRESH_NOISE, 255, THRESH_TOZERO);

      Mat gradient;
      addWeighted(gradX, 0.5, gradY, 0.5, 0, gradient);

      gradient.convertTo(gradient, CV_32FC1);

      Point2d _phase_diff = Point2d(0);
      
      if(stab){
        _phase_diff = phaseCorrelate(gradient, prev_cropped);
      }
      
      phase_diff = phase_diff * ((float)(100 - return_rate)/100); // Return rate defined in percents. Divide by 100
      
      phase_diff += _phase_diff;
      cout << "\r                                                                         " << flush;
      cout << "\rPhase diff:\t[" << phase_diff.x << ",\t" << phase_diff.y << "]" << flush;

      // reassemble frame 
      Rect roi = Rect(w*wdiv, h*hdiv, wdiv, hdiv);
      
      gradient.copyTo((*prev_gradient)(roi));
      
      if(stab){
        Mat warp_mat = ( Mat_<double>(2, 3) << 1, 0, phase_diff.x, 0, 1, phase_diff.y);
        if(disp_grad){
          warpAffine(gradient, gradient, warp_mat, Size(frame->cols, frame->rows));
        } else {
          warpAffine(orig_cropped[h][w], orig_cropped[h][w], warp_mat, Size(frame->cols, frame->rows));
        }
      }
      

      //*prev_gradient = gradient;

      if(disp_grad){
        Mat conversion;
        gradient.convertTo(conversion, comp_frame.type() );
        Mat temp[] = {conversion, conversion, conversion};
        merge(temp, 3, comp_frame(roi));
      } else {
        orig_cropped[h][w](roi).copyTo( comp_frame(roi) );
      }
      
    }
  }

  return comp_frame;
}

//Scalar getGlobalMotion(Mat* frame, Scalar diffs[][NUM_DIVS][2]) {
//
//  Mat frame_gm = *frame;
//  Scalar global_disp = Scalar(0);
//
//  Point center = Point(frame->cols/2, frame->rows/2);
//
//  double x = 0.0;
//  double y = 0.0;
//
//  for(int h = 0; h < NUM_DIVS; h++) {
//    for(int w = 0; w < NUM_DIVS; w++) {
//
//      Scalar ang_diff = diffs[h][w][0];
//      Scalar mag_diff = diffs[h][w][1];
//      
//      x += cos(ang_diff[0])*mag_diff[0];
//      y += sin(ang_diff[0])*mag_diff[0];
//    }
//  }
//
//  Point dest_point_x = Point(x, 0);
//  Point dest_point_y = Point(0, y);
//
//  //line(frame_gm, center, ((1*Point(x, y))+center), Scalar(0, 55, 55));
//  //line(frame_gm, center, ((1*dest_point_x)+center), Scalar(0, 255, 255));
//  //line(frame_gm, center, ((1*dest_point_y)+center), Scalar(0, 255, 255));
//
//  global_disp[0] = x;
//  global_disp[1] = y;
//
//  return global_disp;
//
//}

