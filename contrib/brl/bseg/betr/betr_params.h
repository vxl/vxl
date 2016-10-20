#ifndef betr_params_h_
#define betr_params_h_
//:
// \file
// \brief A base class for holding parameters
// \author J.L. Mundy
// \date October 15, 2016
//
#include <string>
#include <vcl_compiler.h>
#include <vbl/vbl_ref_count.h>
class betr_params: public vbl_ref_count
{
 public:
  betr_params() {}
  virtual ~betr_params() {}
  //: check values of parameters to flag illegal values
  virtual bool sanity_check(std::string& errors) const = 0;
};
#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_params.
#include "betr_params_sptr.h"
