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
class betr_edgel_reference_cd : public betr_algorithm
{
 public:
 betr_edgel_reference_cd(): betr_algorithm("edgel_reference_cd"), sigma_(1.0), noise_mul_(1.5), js_div_(0.0){}

 betr_edgel_reference_cd(double sigma, double noise_mul): betr_algorithm("edgel_reference_cd"), sigma_(sigma),noise_mul_(noise_mul),
    js_div_(0.0){}

  // offset and alpha are  sigmoid parameters to convert js_divergence to a change probability
 betr_edgel_reference_cd(double sigma, double noise_mul, double offset, double alpha): betr_algorithm("edgel_reference_cd", offset, alpha),
    sigma_(sigma),noise_mul_(noise_mul), js_div_(0.0){}
  //: process change
  virtual bool process();

  //: resulting change probability
  virtual double prob_change() const{return js_div_;} //for now not really a probability

 private:
  //:results
  double js_div_;
  //: parameters
  double sigma_;
  double noise_mul_;
};
#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_edgel_reference_cd.
