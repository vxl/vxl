#ifndef osl_canny_rothwell_params_h_
#define osl_canny_rothwell_params_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME osl_canny_rothwell_params
// .INCLUDE osl/osl_canny_rothwell_params.h
// .FILE osl_canny_rothwell_params.cxx
// \author fsm@robots.ox.ac.uk

class osl_canny_rothwell_params {
public:
  osl_canny_rothwell_params();
  ~osl_canny_rothwell_params();
  
  float sigma;
  float low;
  float high;
  float range;
  bool verbose;
};

#endif // osl_canny_rothwell_params_h_
