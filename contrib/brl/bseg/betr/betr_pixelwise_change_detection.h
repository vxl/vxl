#ifndef betr_pixelwise_change_detection_h_
#define betr_pixelwise_change_detection_h_
//:
// \file
// \brief Change detection using pixel-wise methods from baml
// \author Tom Pollard
// \date October 12, 2016
//

#include <string>
#include "betr_algorithm.h"

//: Metric used for pixel-wise comparison
enum betr_pixelwise_cd_metric { 
  BT, // Birchfield-Tomasi 
  CENSUS,
  GRAD };


class betr_pixelwise_change_detection : public betr_algorithm
{
public:

  betr_pixelwise_change_detection(): 
    betr_algorithm("pixelwise_change_detection"),
    avg_prob_(0.0), metric_( GRAD ), i_offset_(0), j_offset_(0), change_img_(VXL_NULLPTR){}

  //: select the pixelwise appearance metric
  inline void set_metric( betr_pixelwise_cd_metric m ){
    metric_ = m; }

  //: process change
  virtual bool process();

  //: resulting change probability
  virtual double prob_change() const{return avg_prob_;} //for now not really a probability

  //: image of pixel-wise census probabilities
  virtual vil_image_resource_sptr change_image(unsigned& i_offset, unsigned& j_offset) const{
    i_offset = i_offset_; j_offset = j_offset_; return change_img_;}

private:
  
   //:results
  double avg_prob_;

  //: parameters
  betr_pixelwise_cd_metric metric_;
  unsigned i_offset_;
  unsigned j_offset_;
  vil_image_resource_sptr change_img_;
};
#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_pixelwise_change_detection.
