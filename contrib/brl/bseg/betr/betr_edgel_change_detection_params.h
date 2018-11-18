#ifndef betr_edgel_change_detection_params_h_
#define betr_edgel_change_detection_params_h_
//:
// \file
// \brief Parameters for the edgel change_detection algorithm
// \author J.L. Mundy
// \date October 16, 2016
//
// sigma - the Gaussian standard deviation for smoothing the image prior to edge_detection
//         typically 1 pixel
//
// noise_mul - determines the threshold for detected edge points
//             typically 2.0, but here 0.75 provides a more complete set of edges
//             since a histogram is formed and the background gradient values
//             accumulate in the low bins of the histogram and normally wouldn't
//             contribute to change but just in case...
//
// betr_edgel_factory_params::upsample_factor_
//   - upsample the image resolution using bilinear interpolation.
//     Allows more resolved discrete pixel locations for gradient masks.
//     A value of 2.0 means the image resolution is increased by 2X.
//
#include <string>
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "betr_params.h"
#include <sdet/sdet_detector_params.h>
#include "betr_edgel_factory_params.h"
class betr_edgel_change_detection_params : public betr_params
{
 public:
 betr_edgel_change_detection_params(): sigma_(1.0), noise_mul_(0.75){
 betr_edgel_factory_params* fparams = new betr_edgel_factory_params();
    fparams->upsample_factor_ = 2.0;//default
    edgel_factory_params_ = fparams;
 }

  //: check values of parameters to flag illegal values
  bool sanity_check(std::string& errors) const override;

  void serialize( Json::Value& root ) const override{
    root["sigma"] = sigma_;
    root["noise_mul"] = noise_mul_;
    Json::Value factory_params;
    betr_edgel_factory_params* efparams = dynamic_cast<betr_edgel_factory_params*>(edgel_factory_params_.ptr());
    efparams->serialize(factory_params);
    root["edgel_factory_params"]=factory_params;
  }
  void deserialize( Json::Value& root) override{
    Json::Value null;
    Json::Value factory_params = root.get("edgel_factory_params",null);
    if(!factory_params.isNull()){
      betr_edgel_factory_params* efparams = new betr_edgel_factory_params();
      efparams->deserialize(factory_params);
      edgel_factory_params_ = efparams;
    }
    sigma_ = root.get("sigma",sigma_).asDouble();
    noise_mul_ = root.get("noise_mul",noise_mul_).asDouble();
  }
  //: parameter block
  double sigma_;
  double noise_mul_;
  betr_params_sptr edgel_factory_params_;
};
std::ostream&  operator<<(std::ostream& s, betr_edgel_change_detection_params const& ecdp);
std::istream&  operator>>(std::istream& s, betr_edgel_change_detection_params& ecdp);
#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_edgel_change_detection_params.
