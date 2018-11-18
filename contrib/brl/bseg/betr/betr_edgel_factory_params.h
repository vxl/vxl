#ifndef betr_edgel_factory_params_h_
#define betr_edgel_factory_params_h_
//:
// \file
// \brief Parameters for edgel factory
// \author J.L. Mundy
// \date October 15, 2016
//
//  The sdet_detector for edges has many parameters in the det_params_ member
//  The sdet_detector parameters to be changed from the defaults are:
//  aggressive_junction_closure = 1 -  widens the search for neigboring edge chains
//                                   to form longer chains
//  filterFactor = 0.0 - An overall scale factor for determining
//                        gradient threshold Nominally 2.0.
//  borderp = false - Don't enclose the image with a connected edge chain border.
//                    Required in algorithms that attempt to form a complete image topology
//  smooth  -  the standard deviation of the Gaussian smoothing kernel normally 1 pixel
//  noise_multiplier - factor to determine the gradient threshold normally 2.0. Smaller
//                     means lower gradient threshold
//  edgel_factor parameters:
//   gradient_range_         - the maximum value for the gradient histogram bin range
//   nbins_                  - the number of bins in the gradient histogram
//   upsample_factor_        - increase in resolution of the image using bicubic interpolation
//   min_region_edge_length_ - the smallest dimension (in pixels) allowed for the region of interest for
//                             computing edges.
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
  bool sanity_check(std::string& errors) const override;
  void serialize( Json::Value& root ) const override{
    root["gradient_range"] = gradient_range_;
    root["nbins"] = nbins_;
    root["upsample_factor"] = upsample_factor_;
    root["min_region_edge_length"] = min_region_edge_length_;
  }
  void deserialize( Json::Value& root) override{
    gradient_range_ = root.get("gradient_range",gradient_range_).asDouble();
    nbins_ = root.get("nbins", nbins_).asInt();
    upsample_factor_ = root.get("upsample_factor",upsample_factor_).asDouble();
    min_region_edge_length_ = root.get("min_region_edge_length",min_region_edge_length_).asDouble();
  }
  //: parameter block
  double gradient_range_;
  unsigned nbins_;
  double upsample_factor_;
  double min_region_edge_length_;
  sdet_detector_params det_params_;
};
std::ostream&  operator<<(std::ostream& s, betr_edgel_factory_params const& efp);
std::istream&  operator>>(std::istream& s, betr_edgel_factory_params& efp);

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_edgel_factory_params.
