#ifndef vsl_fit_lines_params_h_
#define vsl_fit_lines_params_h_
// The parameter mixin for Charlie's FitLines
//
// .SECTION Author:
//             Joseph L. Mundy - December 1997
//             GE Corporate Research and Development

class vsl_fit_lines_params {
public :
  vsl_fit_lines_params(int min_fit_length = 10, 
		       bool use_sq_fit = true,
		       double threshold = 0.3,
		       double theta = 5.0, 
		       bool dc_only = false, 
		       bool incremtl = true, 
		       int ignore_end_edgels = 3);
  
  // Check parameters for consistency
  bool SanityCheck();
  
  //The parameter members
  int _min_fit_length;   // Minimum number of pixels to fit lines to
  bool _use_square_fit;
  double _threshold;    // RMS fitting distance threshold
  double _theta;          // Lines must be within theta to be merged
  bool _dc_only;           // Set to 1 to prevent refit to non-dc's
  bool _incremtl;          // Set to 0 to use simple (PAB) fit
  int _ignore_end_edgels;  // Number of (often garbage) edgels to ignore from the 
  // start and end of a segment when fitting a line
};

#endif
