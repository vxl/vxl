// This is bbas/bpgl/ihog/ihog_minimizer.h
#ifndef ihog_minimizer_h_
#define ihog_minimizer_h_
//:
// \file
// \brief Find image homography by minimizing squared intensity diff
// \author Matt Leotta
// \date April 14, 2004
// The homographic transform between images is determined by minimizing the
// sum of squared differences. The ihog transform represents a range of
// transform generality, from pure translation to projective. The solution
// is found by refining across a pyramid of image scales.
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim

#include <ihog/ihog_world_roi.h>
#include <ihog/ihog_transform_2d.h>
#include <ihog/ihog_image.h>
#include <vil/vil_pyramid_image_view.h>

//: Minimize registration error at multiple scales
class ihog_minimizer
{
 public:
  //: Constructor
  ihog_minimizer( const ihog_image<float>& image1,
                  const ihog_image<float>& image2,
                  const ihog_world_roi& roi );

  //: Constructor initialized with a mask which would not include the values in registration.
  ihog_minimizer( const ihog_image<float>& image1,
                  const ihog_image<float>& image2,
                  const ihog_image<float>& image_mask,
                  const ihog_world_roi& roi, bool image1_mask = false );

    //: Constructor initialized with masks on both images.
  ihog_minimizer( const ihog_image<float>& image1,
                  const ihog_image<float>& image2,
                  const ihog_image<float>& image1_mask,
                  const ihog_image<float>& image2_mask,
                  const ihog_world_roi& roi);

  //: custom set functions
  void set_image1_mask(ihog_image<float>& mask);
  void set_image2_mask(ihog_image<float>& mask);

  //: Run the minimization
  void minimize(ihog_transform_2d& xform);

  //: a sub-pixel minimization with an exhaustive initialization at each level of the pyramid
  void minimize_exhaustive_minfo(int radius, ihog_transform_2d& xform);

  //: Run the minimization using mutual information cost
  void minimize_using_minfo(ihog_transform_2d& xform);

  double get_end_error(){return end_error_;}
  //:debug purposes
  vil_pyramid_image_view<float>& from_pyr() {return from_pyramid_;}
  vil_pyramid_image_view<float>& to_pyr() {return to_pyramid_;}

 protected:
  vil_pyramid_image_view<float> from_pyramid_;
  vil_pyramid_image_view<float> to_pyramid_;
  vil_pyramid_image_view<float> from_mask_pyramid_;
  vil_pyramid_image_view<float> to_mask_pyramid_;
  // vimt_pyramid has w2img transforms but not worth making another class
  std::vector<ihog_transform_2d> w2img1_;
  std::vector<ihog_transform_2d> w2img2_;
  std::vector<ihog_transform_2d> w2mask_img1_;
  std::vector<ihog_transform_2d> w2mask_img2_;

  std::vector<ihog_world_roi> roi_pyramid_;

  //  static const unsigned min_level_size_ = 256;
  static const unsigned min_level_size_ = 8;
  double end_error_;
  bool from_mask_; // true if mask is associated with from_pyramid_
  bool to_mask_; // true if mask is associated with to_pyramid_
};

#endif // ihog_minimizer_h_
