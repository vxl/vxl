//-*- c++ -*-------------------------------------------------------------------
#ifndef _vsl_edge_detector_params_h_
#define _vsl_edge_detector_params_h_
// .NAME vsl_edge_detector_params
// .INCLUDE vsl/vsl_edge_detector_params.h
// .FILE vsl_edge_detector_params.cxx
//
// .SECTION Description
//
// The parameter mixin for Charlie Rothwell's extended Canny
//
// .SECTION Author:
//             Joseph L. Mundy - December 1997
//             GE Corporate Research and Development

class vsl_edge_detector_params /*: public ParamMixin*/ {
public :
  vsl_edge_detector_params();
  ~vsl_edge_detector_params();

  float _sigma;	       	       // Standard deviation of the smoothing kernel
  float _low;		       // Low hysteresis threshold
  float _gauss_tail;	       // Used in determining the convolution kernel
  bool  _fill_gaps;	       // maintain topology, flag
  //                              true = fill in one-pixel holes
  //                              false = do not fill in holes
  bool _verbose;               // output debug messages
};

#endif
