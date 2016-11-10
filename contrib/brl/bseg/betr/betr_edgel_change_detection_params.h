#ifndef betr_edgel_change_detection_params_h_
#define betr_edgel_change_detection_params_h_
//:
// \file
// \brief Parameters for edgel change_detection
// \author J.L. Mundy
// \date October 16, 2016
//
#include <string>
#include <vcl_compiler.h>
#include "betr_params.h"
#include <sdet/sdet_detector_params.h>
class betr_edgel_change_detection_params : public betr_params
{
 public:
 betr_edgel_change_detection_params(): sigma_(1.0), noise_mul_(0.75), upsample_factor_(2.0){}
  //: check values of parameters to flag illegal values
  virtual bool sanity_check(std::string& errors) const;
  //: parameter block
  double sigma_;
  double noise_mul_;
  double upsample_factor_;
};
#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_edgel_change_detection_params.

