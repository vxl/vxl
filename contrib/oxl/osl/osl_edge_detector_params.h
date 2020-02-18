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

  float sigma_{3.0f};        // Standard deviation of the smoothing kernel
  float low_{6};             // Low hysteresis threshold
  float gauss_tail_{0.015f}; // Used in determining the convolution kernel
  bool fill_gaps_{true};     // maintain topology, flag
                             // true = fill in one-pixel holes
                             // false = do not fill in holes
  bool verbose_{false};      // output debug messages
};

#endif // osl_edge_detector_params_h_
