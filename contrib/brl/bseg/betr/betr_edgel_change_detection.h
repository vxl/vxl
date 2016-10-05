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
 betr_edgel_change_detection(): betr_algorithm("edgel_change_detection"), sigma_(1.0), noise_mul_(0.75), js_div_(0.0), i_offset_(0), j_offset_(0), change_img_(VXL_NULLPTR){}

 betr_edgel_change_detection(double sigma, double noise_mul): betr_algorithm("edgel_change_detection"), sigma_(sigma),noise_mul_(noise_mul),
    js_div_(0.0), i_offset_(0), j_offset_(0), change_img_(VXL_NULLPTR){}

  // offset and alpha are  sigmoid parameters to convert js_divergence to a change probability
 betr_edgel_change_detection(double sigma, double noise_mul, double offset, double alpha): betr_algorithm("edgel_change_detection", offset, alpha),
    sigma_(sigma),noise_mul_(noise_mul), js_div_(0.0), i_offset_(0), j_offset_(0), change_img_(VXL_NULLPTR){}
  //: process change
  virtual bool process();

  //: resulting change probability
  virtual double prob_change() const{return js_div_;} //for now not really a probability

  //: image of edgel gradient magnitudes
  virtual vil_image_resource_sptr change_image(unsigned& i_offset, unsigned& j_offset) const{
    i_offset = i_offset_; j_offset = j_offset_; return change_img_;}

 private:
  //:results
  double js_div_;
  //: parameters
  double sigma_;
  double noise_mul_;
  unsigned i_offset_;
  unsigned j_offset_;
  vil_image_resource_sptr change_img_;
};
#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_edgel_change_detection.
