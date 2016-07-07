#ifndef betr_edgel_change_detection_h_
#define betr_edgel_change_detection_h_
//:
// \file
// \brief Change detection based on edgel strength histograms
// \author J.L. Mundy
// \date June 28, 2016
//
#include <string>
#include "betr_algorithm.h"
class betr_edgel_change_detection : public betr_algorithm
{
 public:
 betr_edgel_change_detection(): betr_algorithm("edgel_change_detection"), sigma_(1.0), noise_mul_(1.5), js_div_(0.0){}

 betr_edgel_change_detection(double sigma, double noise_mul): betr_algorithm("edgel_change_detection"), sigma_(sigma),noise_mul_(noise_mul), js_div_(0.0){}

 betr_edgel_change_detection(double sigma, double noise_mul, double offset, double alpha):
  betr_algorithm("edgel_change_detection", offset, alpha), sigma_(sigma),noise_mul_(noise_mul), js_div_(0.0){}
  //: process change
  virtual bool process();
  //: resulting change probability
  virtual double prob_change() const{return js_div_;} //for now
 private:
  double js_div_;
  double sigma_;
  double noise_mul_;
};
#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_edgel_change_detection.
