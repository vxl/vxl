// This is vpgl/ihog/ihog_minimizer.h
#ifndef ihog_minimizer_h_
#define ihog_minimizer_h_
//:
// \file
// \brief Minimize registration error at mulitiple scales 
// \author Matt Leotta
// \date 4/14/04
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <vnl/vnl_vector.h>
#include <vil/vil_image_view.h>
#include <ihog/ihog_world_roi.h>
#include <ihog/ihog_transform_2d.h>
#include <ihog/ihog_image.h>
#include <vil/vil_pyramid_image_view.h>

//: Minimize registration error at mulitiple scales 
class ihog_minimizer 
{
public:
  //: Constructor
  ihog_minimizer( const ihog_image<float>& image1,
                   const ihog_image<float>& image2,
                   const ihog_world_roi& roi );

  //: Constructor intialized with a mask which would not include the values in registration. 
  ihog_minimizer( const ihog_image<float>& image1,
                   const ihog_image<float>& image2,
                   const ihog_image<float>& image_mask,
                   const ihog_world_roi& roi, bool image1_mask = false );

    //: Constructor intialized with masks on both images. 
  ihog_minimizer( const ihog_image<float>& image1,
                   const ihog_image<float>& image2,
                   const ihog_image<float>& image1_mask,
                   const ihog_image<float>& image2_mask,
                   const ihog_world_roi& roi);

  //: Run the minimization
  void minimize(ihog_transform_2d& xform);

  double get_end_error(){return end_error_;}


protected:
  vil_pyramid_image_view<float> from_pyramid_;
  vil_pyramid_image_view<float> to_pyramid_;
  vil_pyramid_image_view<float> from_mask_pyramid_;
  vil_pyramid_image_view<float> to_mask_pyramid_;
  ihog_transform_2d form1_;
  ihog_transform_2d form2_;
  ihog_transform_2d mask_form1_;
  ihog_transform_2d mask_form2_;
  vcl_vector<ihog_world_roi> roi_pyramid_;

  static const unsigned min_level_size_ = 256;
  double end_error_; 
  bool from_mask_; // true if mask is associated with from_pyramid_
  bool to_mask_; // true if mask is associated with to_pyramid_

};

#endif // ihog_minimizer_h_

