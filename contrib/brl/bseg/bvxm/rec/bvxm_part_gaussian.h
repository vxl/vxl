// This is contrib/bvxm/rec/bvxm_part_gaussian.h
#ifndef bvxm_part_gaussian_h_
#define bvxm_part_gaussian_h_
//:
// \file
// \brief class to represent primitive parts as non-isotropic gaussian filters oriented in various ways
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/16/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim
//
//

#include <rec/bvxm_part_base.h>
#include <vcl_vector.h>
#include <vcl_utility.h>
#include <rec/bvxm_part_gaussian_sptr.h>

#include <vnl/vnl_vector_fixed.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>

class bvxm_part_gaussian : public bvxm_part_instance {

public:

  bvxm_part_gaussian(float x, float y, float strength, float lambda0, float lambda1, float theta, bool bright, unsigned type) : bvxm_part_instance(0, type, bvxm_part_instance_kind::GAUSSIAN, x, y, strength), lambda0_(lambda0), lambda1_(lambda1), theta_(theta), bright_(bright) {};

  virtual bool mark_receptive_field(vil_image_view<vxl_byte>& img, unsigned plane);
  virtual bool mark_center(vil_image_view<vxl_byte>& img, unsigned plane);
  virtual bool mark_receptive_field(vil_image_view<float>& img, float val);

  virtual vnl_vector_fixed<float,2> direction_vector(void);  // return a unit vector that gives direction of this instance in the image

  //: Print an ascii summary to the stream
  virtual void print_summary(vcl_ostream &os) const
  {
    os << "x: " << x_ << " y: " << y_ << " strength: " << strength_ << vcl_endl;
    os << "lambda0: " << lambda0_ << " lambda1: " << lambda1_ << " theta: " << theta_ << vcl_endl;
  }

  virtual bvxm_part_gaussian* cast_to_gaussian(void);

  float lambda0_;  // axis 
  float lambda1_;
  float theta_;    // orientation angle (in degrees)
  bool bright_;
  
};

//strength_threshold in [0,1] - min strength to declare the part as detected
//: extracts only one type of primitive and adds to the part vector
bool extract_gaussian_primitives(vil_image_resource_sptr img, float lambda0, float lambda1, float theta, bool bright, float strength_threshold, unsigned type, vcl_vector<bvxm_part_instance_sptr>& parts);

#endif  // bvxm_part_gaussian_h_
