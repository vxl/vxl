// This is bbas/bpgl/ihog/ihog_lsqr_cost_func.cxx
#include <utility>
#include "ihog_lsqr_cost_func.h"
//:
// \file

#include <vil/algo/vil_gauss_filter.h>


//: Constructor
ihog_lsqr_cost_func::ihog_lsqr_cost_func( const ihog_image<float>& image1,
                                          const ihog_image<float>& image2,
                                          ihog_world_roi  roi,
                                          const ihog_transform_2d& init_xform )
 : vnl_least_squares_function(1,1),
   from_image_(image1),
   to_image_(image2),
   roi_(std::move(roi)),
   form_(init_xform.form()),
   from_mask_(false),
   to_mask_(false)
{
  vnl_vector<double> params;
  init_xform.params(params);
  from_samples_ = roi_.sample(from_image_);
  use_gradient_ = false;
  int number_of_residuals = from_samples_.size();
  vnl_least_squares_function::init(params.size(), number_of_residuals);
}


ihog_lsqr_cost_func::ihog_lsqr_cost_func( const ihog_image<float>& image1,
                                          const ihog_image<float>& image2,
                                          const ihog_image<float>& mask,
                                          ihog_world_roi  roi,
                                          const ihog_transform_2d& init_xform, bool image1_mask)
 : vnl_least_squares_function(1,1),
   from_image_(image1),
   to_image_(image2),
   roi_(std::move(roi)),
   form_(init_xform.form()),
   from_mask_(image1_mask),
   to_mask_(!image1_mask)
{
  if (from_mask_) {
    from_mask_image_ = mask;
  }
  else {
    to_mask_image_ = mask;
  }
  vnl_vector<double> params;
  init_xform.params(params);
  from_samples_ = roi_.sample(from_image_);
  int number_of_residuals = from_samples_.size();
  use_gradient_ = false;
  vnl_least_squares_function::init(params.size(), number_of_residuals);
}

ihog_lsqr_cost_func::ihog_lsqr_cost_func(const ihog_image<float>& image1,
                                         const ihog_image<float>& image2,
                                         const ihog_image<float>& mask1,
                                         const ihog_image<float>& mask2,
                                         ihog_world_roi  roi,
                                         const ihog_transform_2d& init_xform)
 : vnl_least_squares_function(1,1),
   from_image_(image1),
   to_image_(image2),
   from_mask_image_(mask1),
   to_mask_image_(mask2),
   roi_(std::move(roi)),
   form_(init_xform.form()),
   from_mask_(true),
   to_mask_(true)
{
  vnl_vector<double> params;
  init_xform.params(params);
  from_samples_ = roi_.sample(from_image_);
  int number_of_residuals = from_samples_.size();
  use_gradient_ = false;
  vnl_least_squares_function::init(params.size(), number_of_residuals);
}


//: The main function.
//  Given the parameter vector x, compute the vector of residuals fx.
//  Fx has been sized appropriately before the call.
void
ihog_lsqr_cost_func::f(vnl_vector<double> const& x, vnl_vector<double>& fx)
{
  ihog_transform_2d new_xform;
  new_xform.set(x, form_);
  ihog_image<float> test_image(to_image_);
  test_image.set_world2im(new_xform*to_image_.world2im());
  vnl_vector<double> to_samples = roi_.sample(test_image);

  if (from_mask_ || to_mask_) {
    vnl_vector<double> mask_samples;
    if (from_mask_) {
      mask_samples = roi_.sample(from_mask_image_);
    }
    if (to_mask_) {
      ihog_image<float> mask_test_image(to_mask_image_);
      mask_test_image.set_world2im(new_xform*to_image_.world2im());
      vnl_vector<double> to_mask_samples = roi_.sample(mask_test_image);
      // NORMALIZE based on number of pixels in sum
      //double nsamples = to_mask_samples.sum();
      //if (nsamples)
      //  to_mask_samples /= nsamples;
      //else
      //  std::cerr << "error: mask sums to 0\n";
      if (from_mask_) {
        // mask_samples already filled in - multiply by second mask to get AND
        mask_samples = element_product<double>(mask_samples,to_mask_samples);
      }
      else {
        // no previous mask - mask is to_mask
        mask_samples = to_mask_samples;
      }
    }
    fx = element_product<double>(mask_samples,(from_samples_ - to_samples));
  }
  else
  {
    fx = from_samples_;
    vnl_vector<double>::iterator sit = fx.begin();
    for (unsigned i = 0; sit != fx.end(); ++sit, ++i)
      (*sit)-=to_samples[i];
  }
}


//: Returns the transformed second image
vil_image_view<float>
ihog_lsqr_cost_func::last_xformed_image()
{
  return roi_.resample(to_image_);
}
