#ifndef breg3d_homography_generator_h_
#define breg3d_homography_generator_h_
//:
// \file
// \brief homography generator base class
//
// \author Daniel Crispell
// \date Mar 01, 2008
// \verbatim
//  Modifications
//   Mar 25 2008 dec  moved to contrib/dec/breg3d
//   Aug 09 2010 jlm  moved to brl/bseg/bvxm/breg3d
// \endverbatim


#include <ihog/ihog_transform_2d.h>
#include <vil/vil_image_view.h>

class breg3d_homography_generator
{
 public:

  breg3d_homography_generator() : use_mask0_(false), use_mask1_(false), compute_projective_(false) {}
  virtual ~breg3d_homography_generator() = default;

  void set_image0(vil_image_view<float> *img0) { img0_ = img0; }
  void set_image1(vil_image_view<float> *img1) { img1_ = img1; }
  void set_mask0(vil_image_view<float> *mask0) { mask0_ = mask0; use_mask0_ = true; }
  void set_mask1(vil_image_view<float> *mask1) { mask1_ = mask1; use_mask1_ = true; }
  //: set generator to compute a projective transformation.  default is affine.
  void set_projective(bool use_projective) {compute_projective_ = use_projective; }

  virtual ihog_transform_2d compute_homography() = 0;

 protected:
  bool use_mask0_;
  bool use_mask1_;
  bool compute_projective_;

  vil_image_view<float> *img0_;
  vil_image_view<float> *img1_;
  vil_image_view<float> *mask0_;
  vil_image_view<float> *mask1_;
};


#endif
