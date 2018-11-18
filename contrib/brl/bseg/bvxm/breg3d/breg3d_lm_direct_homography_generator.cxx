#include "breg3d_lm_direct_homography_generator.h"

#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_2x3.h>

#include <ihog/ihog_transform_2d.h>
#include <ihog/ihog_image.h>
#include <ihog/ihog_world_roi.h>
#include <ihog/ihog_minimizer.h>

ihog_transform_2d breg3d_lm_direct_homography_generator::compute_homography()
{
  int border = 2;
  ihog_world_roi roi(img0_->ni()- 2*border, img0_->nj()- 2*border,vgl_point_2d<double>(border,border));

  ihog_transform_2d init_xform;
  if (this->compute_projective_)
    init_xform.set_projective(vnl_double_3x3().set_identity());
  else
    init_xform.set_affine(vnl_double_2x3().set_identity());

  ihog_minimizer *minimizer = nullptr;
  // no masks
  if (!use_mask0_ && !use_mask1_) {
    ihog_image<float> from_img(*img0_, init_xform);
    ihog_image<float> to_img(*img1_, ihog_transform_2d());
    minimizer = new ihog_minimizer(from_img, to_img, roi);
  }
  // one mask
  else if (!use_mask0_ || !use_mask1_) {
    ihog_image<float> from_img(*img0_, init_xform);
    ihog_image<float> to_img(*img1_, ihog_transform_2d());
    if (use_mask0_) {
      ihog_image<float> mask_img(*mask0_, init_xform);
      minimizer = new ihog_minimizer(from_img, to_img, mask_img, roi, false);
    }
    else {
      ihog_image<float> mask_img(*mask1_, ihog_transform_2d());
      minimizer = new ihog_minimizer(from_img, to_img, mask_img, roi, true);
    }
  }
  // both masks
  else {
    ihog_image<float> from_img(*img0_, init_xform);
    ihog_image<float> from_mask(*mask0_, init_xform);
    ihog_image<float> to_img(*img1_, ihog_transform_2d());
    ihog_image<float> to_mask(*mask1_, ihog_transform_2d());
    minimizer = new ihog_minimizer(from_img, to_img, from_mask, to_mask, roi);
  }

  std::cout << " minimizing image error..";
  minimizer->minimize(init_xform);
  std::cout << "..done." << std::endl;
  double curr_error = minimizer->get_end_error();
  std::cout << "end error = " << curr_error << std::endl;
  // computed homography maps pixels in current image to pixels in base image
  //vnl_double_3x3 H = init_xform.get_inverse().get_matrix();
  delete minimizer;
  return init_xform;
}
