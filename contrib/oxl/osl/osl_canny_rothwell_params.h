// This is oxl/osl/osl_canny_rothwell_params.h
#ifndef osl_canny_rothwell_params_h_
#define osl_canny_rothwell_params_h_
//:
// \file
// \author fsm
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

class osl_canny_rothwell_params
{
 public:
  osl_canny_rothwell_params() : sigma(1.0), low(2.0), high(12.0), range(2.0), verbose(true) {}
  ~osl_canny_rothwell_params() {}

  float sigma;
  float low;
  float high;
  float range;
  bool verbose;
};

#endif // osl_canny_rothwell_params_h_
