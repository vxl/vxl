#ifndef osl_harris_params_h_
#define osl_harris_params_h_

// .NAME osl_harris_params - parameter block class for the osl_harris class
// .INCLUDE osl/osl_harris_params.h
// .FILE osl_harris_params.cxx
// .SECTION Author
//    Andrew W. Fitzgibbon, Oxford RRG, 07 Feb 97
// .SECTION Modifications
//    fsm. various ports and modifications.

class osl_harris_params {
public:
  osl_harris_params();
  osl_harris_params(int max_corners_max, float gauss_sigma);
  osl_harris_params(int max_corners_max, float gauss_sigma, float relative_minimum, float scale_factor);
  void set_adaptive(int corner_count_low, int adaptive_window_size, float density_thresh);

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

#endif // osl_harris_params_h_
