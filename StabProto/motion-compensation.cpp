#include "vidstab.h"

using namespace cv;
using namespace std;

void compensateMotion(Mat* frame, Scalar diffs[][NUM_DIVS][2]) {
  // filtered by median motion (frame motion vs. frame action)
  // TODO: How do we do this without warpAffine?

  Scalar global_disp; 
  global_disp = getGlobalMotion(frame, diffs); 

  Scalar weighted_disp;
  weighted_disp = weighMotion(global_disp, diffs);

  // x - weighted_disp[0]
  // y - weighted_disp[1]
  Mat warp_mat = ( Mat_<double>(2, 3) << 1, 0, 0, 0, 1, (1*weighted_disp[1]) );
  warpAffine(*frame, *frame, warp_mat, Size(frame->cols, frame->rows), WARP_INVERSE_MAP);
  
  //cout << "\r" << weighted_disp << flush;
}

Scalar weighMotion(Scalar global_disp, Scalar diffs[][NUM_DIVS][2]) {
  
  // keep a running count of acceptable LMVs
  int valid_lmv = 1;

  // take in diffs
  for(int h = 0; h < NUM_DIVS; h++) {
    for(int w = 0; w < NUM_DIVS; w++) {

      Scalar ang_diff = diffs[h][w][0];
      Scalar mag_diff = diffs[h][w][1];

      double diff_x = cos(ang_diff[0])*mag_diff[0];
      double diff_y = sin(ang_diff[0])*mag_diff[0];

      double gm_x = global_disp[0];
      double gm_y = global_disp[1];

      // establish +/- tolerances --  GM_TOL
      // check tolerance
      //if( (diff_x > (GM_TOL * gm_x) && diff_x < ((1+GM_TOL) * gm_x)) &&
      //    (diff_y > (GM_TOL * gm_y) && diff_y < ((1+GM_TOL) * gm_y)) ) {
      //  valid_lmv++;
      //}

      double num = 0.0;
      if(abs(diff_x) > num && abs(diff_y) > num) valid_lmv++;
      //cout << "\n" << valid_lmv << flush; 
    }
  }

  //Scalar weighted_disp = Scalar(0);
  //for(int i = 0; i < weighted_disp.size(); i++) {
  //  weighted_disp[i] = global_disp[i] / (double)valid_lmv;
  //}

  // how much to warp the image by
  return global_disp / (double)valid_lmv;
}
