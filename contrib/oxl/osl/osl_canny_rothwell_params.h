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
   osl_canny_rothwell_params() = default;
   ~osl_canny_rothwell_params() = default;

   float sigma{1.0};
   float low{2.0};
   float high{12.0};
   float range{2.0};
   bool verbose{true};
};

#endif // osl_canny_rothwell_params_h_
