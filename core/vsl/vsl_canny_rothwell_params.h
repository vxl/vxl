#ifndef vsl_canny_rothwell_params_h_
#define vsl_canny_rothwell_params_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vsl_canny_rothwell_params
// .INCLUDE vsl/vsl_canny_rothwell_params.h
// .FILE vsl_canny_rothwell_params.cxx
// \author fsm@robots.ox.ac.uk

class vsl_canny_rothwell_params {
public:
  vsl_canny_rothwell_params();
  ~vsl_canny_rothwell_params();
  
  float sigma;
  float low;
  float high;
  float range;
  bool verbose;
};

#endif // vsl_canny_rothwell_params_h_
