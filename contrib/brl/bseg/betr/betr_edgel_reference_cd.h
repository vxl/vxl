#ifndef betr_edgel_reference_cd_h_
#define betr_edgel_reference_cd_h_
//:
// \file
// \brief Change detection based on edgel strength histograms but with respect to the reference region (single image)
// \author J.L. Mundy
// \date August 7, 2016
//
#include <string>
#include "betr_algorithm.h"
#include "betr_edgel_reference_cd_params.h"
class betr_edgel_reference_cd : public betr_algorithm
{
 public:
 betr_edgel_reference_cd(): betr_algorithm("edgel_reference_cd"), js_div_(0.0){
    params_ = new betr_edgel_reference_cd_params();}

 betr_edgel_reference_cd(betr_edgel_reference_cd_params const& params): betr_algorithm("edgel_reference_cd"),
    js_div_(0.0){params_ = new betr_edgel_reference_cd_params(params);}

  // offset and alpha are  sigmoid parameters to convert js_divergence to a change probability
 betr_edgel_reference_cd(betr_edgel_reference_cd_params const& params, double offset, double alpha): betr_algorithm("edgel_reference_cd", offset, alpha), js_div_(0.0){params_ = new betr_edgel_reference_cd_params(params);}
  //: process change
  bool process() override;

  //: resulting change probability
  double prob_change() const override{return js_div_;} //for now not really a probability

 private:
  //:results
  double js_div_;
  //: parameters
  //UNUSED double sigma_;
  //UNUSED double noise_mul_;
};
#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_edgel_reference_cd.
