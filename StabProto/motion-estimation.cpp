#include "vidstab.h"

using namespace cv;
using namespace std;

Point2d phase_diff = Point2d(0);

// returns Mat with motion drawn on
// fills diffs Scalar, representing motion
Mat estimateMotion(Mat* frame, Mat* prev_frame, Scalar means[][NUM_DIVS][2], Scalar avg_means[][NUM_DIVS][2], Scalar diffs[][NUM_DIVS][2]) {

  Mat gradX, gradY;
  Mat prev_gradX, prev_gradY;
  Mat frame_bw;
  Mat prev_frame_bw;

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
  
  cvtColor(*prev_frame, prev_frame_bw, CV_RGB2GRAY);
  GaussianBlur(prev_frame_bw, prev_frame_bw, Size(25, 25), 0, 0);
  // divide frame into cropped portions
  int hdiv = frame->rows/NUM_DIVS;
  int wdiv = frame->cols/NUM_DIVS;

  Mat cropped[NUM_DIVS][NUM_DIVS];
  Mat prev_cropped[NUM_DIVS][NUM_DIVS];
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

      //
      prev_cropped[h][w] = Mat(prev_frame_bw, crop);
      //

      // Sobel operators - X & Y
      Sobel(cropped[h][w], gradX, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
      Sobel(cropped[h][w], gradY, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);

//
      Sobel(prev_cropped[h][w], prev_gradX, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
      Sobel(prev_cropped[h][w], prev_gradY, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
//

      // THRESHOLD NOISE
      convertScaleAbs(gradX, gradX);
      convertScaleAbs(gradY, gradY);
      threshold(gradX, gradX, THRESH_NOISE, 255, THRESH_TOZERO);
      threshold(gradY, gradY, THRESH_NOISE, 255, THRESH_TOZERO);

      Mat gradient;
      addWeighted(gradX, 0.5, gradY, 0.5, 0, gradient);
//
      convertScaleAbs(prev_gradX, prev_gradX);
      convertScaleAbs(prev_gradY, prev_gradY);
      threshold(prev_gradX, prev_gradX, THRESH_NOISE, 255, THRESH_TOZERO);
      threshold(prev_gradY, prev_gradY, THRESH_NOISE, 255, THRESH_TOZERO);
      Mat prev_gradient;
      addWeighted(prev_gradX, 0.5, prev_gradY, 0.5, 0, prev_gradient);
//

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

      /*--- trying phaseCorrelate ---*/

      gradient.convertTo(gradient, CV_32FC1);
      prev_gradient.convertTo(prev_gradient, CV_32FC1);

      Point2d _phase_diff = Point2d(0);
      _phase_diff = phaseCorrelate(gradient, prev_gradient); 
      phase_diff += _phase_diff;

      //cout <<  phase_diff << endl;
      //cout << "_" << _phase_diff << endl;

      Mat clone = frame->clone();

      Mat warp_mat = ( Mat_<double>(2, 3) << 1, 0, (phase_diff.x), 0, 1, (phase_diff.y) );
      //warpAffine(clone, clone, warp_mat, frame->size(), WARP_INVERSE_MAP);
      warpAffine(clone, clone, warp_mat, frame->size());
      imshow("clone", clone);
      //imshow("gradient", gradient);
      //imshow("prev_gradient", prev_gradient);
        
      /*-----------------------------*/
      
      // RESULTS
      //Point center = Point(cropped[h][w].cols/2, cropped[h][w].rows/2);
      Point2d center = Point(cropped[h][w].cols/2, cropped[h][w].rows/2);
      //Point dest_point = Point(cos(means[h][w][0][0])*means[h][w][1][0], sin(means[h][w][0][0])*means[h][w][1][0]);
      //Point dest_point = Point(cos(avg_means[h][w][0][0])*avg_means[h][w][1][0], sin(avg_means[h][w][0][0])*avg_means[h][w][1][0]);
      Point dest_point = Point(cos(ang_diff[0])*mag_diff[0], sin(ang_diff[0])*mag_diff[0]);
      Point dest_point_x = Point(cos(ang_diff[0])*mag_diff[0], 0);
      Point dest_point_y = Point(0, sin(ang_diff[0])*mag_diff[0]);

      //line(cropped[h][w], center, ((1*dest_point)+center), Scalar(0, 255, 255));
      line(cropped[h][w], center, ((phase_diff)+center), Scalar(0, 255, 255));
      //line(cropped[h][w], center, ((1*dest_point_x)+center), Scalar(0, 255, 255));
      //line(cropped[h][w], center, ((1*dest_point_y)+center), Scalar(0, 255, 255));

      // reassemble frame 
      cropped[h][w].copyTo( frame_bw(Rect(w*wdiv, h*hdiv, wdiv, hdiv) ));
    }
  }
  
  //return frame_gm;

  return frame_bw;
}

Scalar getGlobalMotion(Mat* frame, Scalar diffs[][NUM_DIVS][2]) {

  Mat frame_gm = *frame;
  Scalar global_disp = Scalar(0);

  Point center = Point(frame->cols/2, frame->rows/2);

  double x = 0.0;
  double y = 0.0;

  for(int h = 0; h < NUM_DIVS; h++) {
    for(int w = 0; w < NUM_DIVS; w++) {

      Scalar ang_diff = diffs[h][w][0];
      Scalar mag_diff = diffs[h][w][1];
      
      x += cos(ang_diff[0])*mag_diff[0];
      y += sin(ang_diff[0])*mag_diff[0];
    }
  }

  Point dest_point_x = Point(x, 0);
  Point dest_point_y = Point(0, y);

  //line(frame_gm, center, ((1*Point(x, y))+center), Scalar(0, 55, 55));
  //line(frame_gm, center, ((1*dest_point_x)+center), Scalar(0, 255, 255));
  //line(frame_gm, center, ((1*dest_point_y)+center), Scalar(0, 255, 255));

  global_disp[0] = x;
  global_disp[1] = y;

  return global_disp;

}

