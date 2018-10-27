// This is brl/bbas/bpgl/ihog/ihog_minfo_cost_func.cxx
#include <utility>
#include "ihog_minfo_cost_func.h"
//:
// \file

#include <vil/algo/vil_gauss_filter.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_1d.h>


//: Constructor
ihog_minfo_cost_func::ihog_minfo_cost_func( const ihog_image<float>& image1,
                                            const ihog_image<float>& image2,
                                            ihog_world_roi  roi,
                                            const ihog_transform_2d& init_xform,
                                            unsigned nbins)
 : //vnl_least_squares_function(1,1),
   vnl_cost_function(2),
   from_image_(image1),
   to_image_(image2),
   roi_(std::move(roi)),
   form_(init_xform.form()),
   from_mask_(false),
   to_mask_(false),
   nbins_(nbins)
{
  vnl_vector<double> params;
  init_xform.params(params);
  from_samples_ = roi_.sample(from_image_);
  //use_gradient_ = false;
  //int number_of_residuals = from_samples_.size();
#if 0
  int number_of_residuals = 1;  // just the mutual info
  vnl_least_squares_function::init(params.size(), number_of_residuals);
#endif
}


ihog_minfo_cost_func::ihog_minfo_cost_func( const ihog_image<float>& image1,
                                            const ihog_image<float>& image2,
                                            const ihog_image<float>& mask,
                                            ihog_world_roi  roi,
                                            const ihog_transform_2d& init_xform, bool image1_mask, unsigned nbins)
 : //vnl_least_squares_function(1,1),
   vnl_cost_function(2),
   from_image_(image1),
   to_image_(image2),
   roi_(std::move(roi)),
   form_(init_xform.form()),
   from_mask_(image1_mask),
   to_mask_(!image1_mask),
   nbins_(nbins)
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
#if 0
  int number_of_residuals = from_samples_.size();
      number_of_residuals = 1;  // just the mutual info
  use_gradient_ = false;
  vnl_least_squares_function::init(params.size(), number_of_residuals);
#endif
}

ihog_minfo_cost_func::ihog_minfo_cost_func(const ihog_image<float>& image1,
                                           const ihog_image<float>& image2,
                                           const ihog_image<float>& mask1,
                                           const ihog_image<float>& mask2,
                                           ihog_world_roi  roi,
                                           const ihog_transform_2d& init_xform, unsigned nbins)
 : //vnl_least_squares_function(1,1),
   vnl_cost_function(2),
   from_image_(image1),
   to_image_(image2),
   from_mask_image_(mask1),
   to_mask_image_(mask2),
   roi_(std::move(roi)),
   form_(init_xform.form()),
   from_mask_(true),
   to_mask_(true),
   nbins_(nbins)
{
  vnl_vector<double> params;
  init_xform.params(params);
  from_samples_ = roi_.sample(from_image_);
#if 0
  int number_of_residuals = from_samples_.size();
      number_of_residuals = 1;  // just the mutual info
  use_gradient_ = false;
  vnl_least_squares_function::init(params.size(), number_of_residuals);
#endif
}


//: The main function.
//  Given the parameter vector x, compute the vector of residuals fx.
//  Fx has been sized appropriately before the call.  it should have dimension 1
//void
//ihog_minfo_cost_func::f(vnl_vector<double> const& x, vnl_vector<double>& fx)
double ihog_minfo_cost_func::f(vnl_vector<double> const& x)
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
      if (from_mask_) {
        // mask_samples already filled in - multiply by second mask to get AND
        mask_samples = element_product<double>(mask_samples,to_mask_samples);
      }
      else {
        // no previous mask - mask is to_mask
        mask_samples = to_mask_samples;
      }
    }
    //fx[0] = entropy_diff(mask_samples, to_samples);
    return entropy_diff(mask_samples, from_samples_, to_samples, nbins_);
  }
  else
  {
    vnl_vector<double> mask_samples(from_samples_.size());
    mask_samples.fill(1.0f);
    //fx[0] = entropy_diff(mask_samples, to_samples);
    return entropy_diff(mask_samples, from_samples_, to_samples, nbins_);
  }
}


//: Returns the transformed second image
vil_image_view<float>
ihog_minfo_cost_func::last_xformed_image()
{
  return roi_.resample(to_image_);
}


double ihog_minfo_cost_func::entropy_diff(vnl_vector<double>& mask_samples, vnl_vector<double>& from_samples, vnl_vector<double>& to_samples, int nbins)
{
  double scl = 1.0/(256.0/nbins);
  vbl_array_2d<double> h(nbins, nbins, 0.0);

  //compute the intensity histogram
  double total_weight = 0.0;
  for (unsigned i = 0; i<to_samples.size(); ++i)
    if (mask_samples[i]>0.0) {
      //match the gpu implementation, which does a floor operation
      auto id = static_cast<unsigned>(std::floor(from_samples[i]*scl)),
               is = static_cast<unsigned>(std::floor(to_samples[i]*scl));

      if (id+1>(unsigned)nbins || is+1>(unsigned)nbins)
        continue;
      h[id][is] += 1.0;
      total_weight += 1.0;
    }
  // convert to probability
  for (int r = 0; r<nbins; ++r)
    for (int c = 0; c<nbins; ++c)
      h[r][c] /= total_weight;

  auto nr = (unsigned)h.rows(), nc = (unsigned)h.cols();
  //marginal distribution for mapped dest intensities
  vbl_array_1d<double> pmr(nc,0.0);
  for (unsigned r = 0; r<nr; ++r)
    for (unsigned c = 0; c<nc; ++c)
      pmr[c]+=h[r][c];
  double jsum = 0.0, msum = 0.0;
  for (unsigned c = 0; c<nc; ++c)
  {
    double pr = pmr[c];
    if (pr>0)
      msum += pr*std::log(pr);
  }
  for (unsigned r = 0; r<nr; ++r)
    for (unsigned c = 0; c<nc; ++c) {
        double prc = h[r][c];
        if (prc>0)
          jsum+= prc*std::log(prc);
    }
  double ent_dif = jsum - msum;
  return -ent_dif/std::log(2.0);
}
