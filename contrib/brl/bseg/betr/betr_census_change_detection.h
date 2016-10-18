#ifndef betr_census_change_detection_h_
#define betr_census_change_detection_h_
//:
// \file
// \brief Change detection based on the census metric
// \author Tom Pollard
// \date October 12, 2016
//

#include <string>
#include "betr_algorithm.h"

class betr_census_change_detection : public betr_algorithm
{
 public:
 betr_census_change_detection(): 
   betr_algorithm("census_change_detection"), sigma_(1.0), noise_mul_(0.75),
   avg_prob_(0.0), i_offset_(0), j_offset_(0), change_img_(VXL_NULLPTR){}

 betr_census_change_detection(double sigma, double noise_mul): 
   betr_algorithm("census_change_detection"), sigma_(sigma),
   noise_mul_(noise_mul), avg_prob_(0.0), i_offset_(0), j_offset_(0), 
   change_img_(VXL_NULLPTR){}

  // offset and alpha are  sigmoid parameters to convert js_divergence to a change probability
  betr_census_change_detection(double sigma, double noise_mul, double offset, double alpha): 
    betr_algorithm("census_change_detection", offset, alpha), sigma_(sigma),
    noise_mul_(noise_mul), avg_prob_(0.0), i_offset_(0), j_offset_(0), 
    change_img_(VXL_NULLPTR){}

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
  double sigma_;
  double noise_mul_;
  unsigned i_offset_;
  unsigned j_offset_;
  vil_image_resource_sptr change_img_;
};
#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_census_change_detection.
