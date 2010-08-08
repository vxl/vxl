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
#include <vimt/vimt_transform_2d.h>
#include <vimt/vimt_image_pyramid.h>

//: Minimize registration error at mulitiple scales 
class ihog_minimizer 
{
public:
  //: Constructor
  ihog_minimizer( const vimt_image_2d_of<float>& image1,
                   const vimt_image_2d_of<float>& image2,
                   const ihog_world_roi& roi );

  //: Constructor intialized with a mask which would not include the values in registration. 
  ihog_minimizer( const vimt_image_2d_of<float>& image1,
                   const vimt_image_2d_of<float>& image2,
                   const vimt_image_2d_of<float>& image_mask,
                   const ihog_world_roi& roi, bool image1_mask = false );

    //: Constructor intialized with masks on both images. 
  ihog_minimizer( const vimt_image_2d_of<float>& image1,
                   const vimt_image_2d_of<float>& image2,
                   const vimt_image_2d_of<float>& image1_mask,
                   const vimt_image_2d_of<float>& image2_mask,
                   const ihog_world_roi& roi);

  //: Run the minimization
  void minimize(vimt_transform_2d& xform);

  double get_end_error(){return end_error_;}


protected:
  vimt_image_pyramid from_pyramid_;
  vimt_image_pyramid to_pyramid_;
  vimt_image_pyramid from_mask_pyramid_;
  vimt_image_pyramid to_mask_pyramid_;
  vcl_vector<ihog_world_roi> roi_pyramid_;

  static const unsigned min_level_size_ = 256;
  double end_error_; 
  bool from_mask_; // true if mask is associated with from_pyramid_
  bool to_mask_; // true if mask is associated with to_pyramid_

};

#endif // ihog_minimizer_h_

