#ifndef osl_fit_lines_params_h_
#define osl_fit_lines_params_h_
// .NAME osl_fit_lines_params - The parameter mixin for Charlie's FitLines
// .INCLUDE osl/osl_fit_lines_params.h
// .FILE osl_fit_lines_params.cxx
//
// .SECTION Author:
//             Joseph L. Mundy - December 1997
//             GE Corporate Research and Development

class osl_fit_lines_params
{
 public :
  osl_fit_lines_params(unsigned int min_fit_length = 10,
                       bool use_sq_fit = true,
                       double threshold = 0.3,
                       double theta = 5.0,
                       bool dc_only = false,
                       bool incremtl = true,
                       unsigned int ignore_end_edgels = 3);

  // Check parameters for consistency
  bool SanityCheck();

  //The parameter members
  unsigned int min_fit_length_;   // Minimum number of pixels to fit lines to
  bool use_square_fit_;
  double threshold_;    // RMS fitting distance threshold
  double theta_;          // Lines must be within theta to be merged
  bool dc_only_;           // Set to 1 to prevent refit to non-dc's
  bool incremtl_;          // Set to 0 to use simple (PAB) fit
  unsigned int ignore_end_edgels_;  // Number of (often garbage) edgels to ignore from the
  // start and end of a segment when fitting a line
};

#endif // osl_fit_lines_params_h_
