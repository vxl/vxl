#ifndef harris_params_h_
#define harris_params_h_

// Class : harris_params
//
// .SECTION Description
//    HarrisParams is a class that awf hasn't documented properly. FIXME
//
// Author: Andrew W. Fitzgibbon, Oxford RRG, 07 Feb 97
// Modification: fsm. various ports and modifications.

class harris_params {
public:
  harris_params();
  harris_params(int corner_count_max, float gauss_sigma);
  harris_params(int corner_count_max, float gauss_sigma, float relative_minimum, float scale_factor);
  void set_adaptive(int corner_count_low_, int adaptive_window_size_, float density_thresh_);

  //------------------------------------------------------------

  //
  bool verbose;

  // origin of the original region of interest.
  int col_start_index;
  int row_start_index;

  //
  bool pab_emulate;

  //------------------------------------------------------------
  
  //  max no of corners wanted.
  int corner_count_max;

  //  smoothing in the corner detector. 
  float gauss_sigma;

  //  used to set the lowest acceptable corner strength, relative to the measured maximum. 
  float relative_minimum;
  
  //  the Harris auto-correlation formula. 
  float scale_factor;
  
  // Adaptive parameters
  bool adaptive;
  int adaptive_window_size;
  int corner_count_low; // Accept all corners with the top "corner_count_low" strengths
  float density_thresh; // Accept corners between low and max if the local density is less than this.
  // "density" means number of corners in a 10x10 window.

private:  
  void set_defaults();
};

#endif
