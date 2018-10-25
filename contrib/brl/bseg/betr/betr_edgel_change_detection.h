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
#include "betr_edgel_change_detection_params.h"
class betr_edgel_change_detection : public betr_algorithm
{
 public:

 betr_edgel_change_detection(): betr_algorithm("edgel_change_detection"),js_div_(0.0), i_offset_(0), j_offset_(0), change_img_(nullptr){
   multiple_ref_ = false;
         params_ =  new betr_edgel_change_detection_params();}


 betr_edgel_change_detection(betr_edgel_change_detection_params const& params): betr_algorithm("edgel_change_detection"),
    js_div_(0.0), i_offset_(0), j_offset_(0), change_img_(nullptr){
   multiple_ref_ = false;
                params_ =  new betr_edgel_change_detection_params();}

  // offset and alpha are  sigmoid parameters to convert js_divergence to a change probability
 betr_edgel_change_detection(betr_edgel_change_detection_params const& params, double offset, double alpha): betr_algorithm("edgel_change_detection", offset, alpha), js_div_(0.0),
         i_offset_(0), j_offset_(0), change_img_(nullptr){
   multiple_ref_ = false;
                 params_ =  new betr_edgel_change_detection_params();}
  //: process change
  bool process() override;

  //: resulting change probability
  double prob_change() const override{return js_div_;} //for now not really a probability

  //: image of edgel gradient magnitudes
  vil_image_resource_sptr change_image(unsigned& i_offset, unsigned& j_offset) const override{
    i_offset = i_offset_; j_offset = j_offset_; return change_img_;}

 private:
  //:results
  double js_div_;
  //: parameters
  unsigned i_offset_;
  unsigned j_offset_;
  vil_image_resource_sptr change_img_;
};
#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_edgel_change_detection.
