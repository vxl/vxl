#ifndef osl_edge_detector_params_h_
#define osl_edge_detector_params_h_
// .NAME    osl_edge_detector_params
// .INCLUDE osl/osl_edge_detector_params.h
// .FILE    osl_edge_detector_params.cxx
//
// .SECTION Description
//
// The parameter mixin for Charlie Rothwell's extended Canny
//
// .SECTION Author:
//             Joseph L. Mundy - December 1997
//             GE Corporate Research and Development

class osl_edge_detector_params /*: public ParamMixin*/ {
public :
  osl_edge_detector_params();
  ~osl_edge_detector_params();

  float sigma_;       // Standard deviation of the smoothing kernel
  float low_;         // Low hysteresis threshold
  float gauss_tail_;  // Used in determining the convolution kernel
  bool  fill_gaps_;   // maintain topology, flag
                      // true = fill in one-pixel holes
                      // false = do not fill in holes
  bool verbose_;      // output debug messages
};

#endif // osl_edge_detector_params_h_
