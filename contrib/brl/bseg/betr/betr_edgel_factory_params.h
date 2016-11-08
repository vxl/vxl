#ifndef betr_edgel_factory_params_h_
#define betr_edgel_factory_params_h_
//:
// \file
// \brief Parameters for edgel factory
// \author J.L. Mundy
// \date October 15, 2016
//
#include <string>
#include <vcl_compiler.h>
#include "betr_params.h"
#include <sdet/sdet_detector_params.h>
class betr_edgel_factory_params : public betr_params
{
 public:
 betr_edgel_factory_params(): gradient_range_(60.0), nbins_(20), upsample_factor_(1.0), min_region_edge_length_(10){
    det_params_.aggressive_junction_closure=1;
    det_params_.filterFactor = 0.0;
    det_params_.borderp = false;
    det_params_.smooth = 1.0;
    det_params_.noise_multiplier = 0.75;
  }
  //: check values of parameters to flag illegal values
  virtual bool sanity_check(std::string& errors) const;
  //: parameter block
  double gradient_range_;
  unsigned nbins_;
  double upsample_factor_;
  double min_region_edge_length_;
  sdet_detector_params det_params_;
};
#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_edgel_factory_params.

