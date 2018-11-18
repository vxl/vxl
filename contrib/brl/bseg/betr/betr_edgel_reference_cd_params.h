#ifndef betr_edgel_reference_cd_params_h_
#define betr_edgel_reference_cd_params_h_
//:
// \file
// \brief Parameters for edgel reference_cd
// \author J.L. Mundy
// \date October 16, 2016
//
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "betr_params.h"
#include <sdet/sdet_detector_params.h>
#include "betr_edgel_factory_params.h"
class betr_edgel_reference_cd_params : public betr_params
{
 public:
 betr_edgel_reference_cd_params(): sigma_(1.0), noise_mul_(0.75){
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
std::ostream&  operator<<(std::ostream& s, betr_edgel_reference_cd_params const& ercdp);
std::istream&  operator>>(std::istream& s, betr_edgel_reference_cd_params& ercdp);

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_edgel_reference_cd_params.
