// This is bbas/bpgl/ihog/ihog_cost_func.cxx
#include <utility>
#include "ihog_cost_func.h"
//:
// \file

#include <vil/algo/vil_gauss_filter.h>
#include <vil/vil_math.h>


//: Constructor
ihog_cost_func::ihog_cost_func( const vil_image_view<float>& image1,
                                const vil_image_view<float>& image2,
                                ihog_world_roi  roi,
                                const ihog_transform_2d& init_xform )
 : vnl_cost_function(1),
   from_image_(image1, ihog_transform_2d()),
   to_image_(image2, init_xform),
   roi_(std::move(roi)),
   form_(init_xform.form())
{
  vnl_vector<double> params;
  init_xform.params(params);
  dim = params.size();

  // offset values by 1.0 so that it is obvious that 0.0 values
  // came from out-of-range interpolation
  vil_math_scale_and_offset_values(from_image_.image(), 1.0, 1.0);
  vil_math_scale_and_offset_values(to_image_.image(), 1.0, 1.0);

  // make a dummy mask
  mask_image_.set_size(image1.ni(),image1.nj());
  mask_image_.fill(true);
}

ihog_cost_func::ihog_cost_func( const vil_image_view<float>& image1,
                                const vil_image_view<float>& image2,
                                const vil_image_view<float>& maskimage,
                                ihog_world_roi  roi,
                                const ihog_transform_2d& init_xform )
 : vnl_cost_function(1),
   from_image_(image1, ihog_transform_2d()),
   to_image_(image2, init_xform),
   roi_(std::move(roi)),
   form_(init_xform.form())
{
  vnl_vector<double> params;
  init_xform.params(params);
  dim = params.size();
  mask_image_=maskimage;
  // offset values by 1.0 so that it is obvious that 0.0 values
  // came from out-of-range interpolation
  vil_math_scale_and_offset_values(from_image_.image(), 1.0, 1.0);
  vil_math_scale_and_offset_values(to_image_.image(), 1.0, 1.0);
}

//: The main function.
double
ihog_cost_func::f(vnl_vector<double> const& x)
{
  ihog_transform_2d new_xform;
  new_xform.set(x, form_);
  to_image_.set_world2im(new_xform);

  vnl_vector<double> s1 = roi_.sample(from_image_);
  vnl_vector<double> s2 = roi_.sample(to_image_);
  vnl_vector<double> w1 = roi_.sample_weights(from_image_);
  vnl_vector<double> w2 = roi_.sample_weights(to_image_);
  double total_weight = 0.0;
  double rms = 0.0;
  if (mask_image_) {
  ihog_image<float> mimage_(mask_image_,ihog_transform_2d());
  vnl_vector<double> s3 = roi_.sample(mimage_);

  for (unsigned int i=0; i<s1.size(); ++i) {
    if (s1[i] != 0.0 && s2[i] != 0.0 && s3[i]>0.0) {
      double diff = s2[i]-s1[i];
      diff *=std::pow(diff,5)/(std::pow(diff,5)+std::pow(100.0,5));
      rms += diff*diff*w1[i]*w2[i];
      total_weight += w1[i]*w2[i];
    }
  }
  }
  else
  {
      for (unsigned int i=0; i<s1.size(); ++i) {
          if (s1[i] != 0.0 && s2[i] != 0.0 ) {
              double diff = s2[i]-s1[i];
              //diff *=std::pow(diff,5)/(std::pow(diff,5)+std::pow(100.0,5));
              rms += diff*diff*w1[i]*w2[i];
              total_weight += w1[i]*w2[i];
          }
      }
  }
  rms /= total_weight;
  return std::sqrt(rms);
}


//: Returns the transformed second image
vil_image_view<float>
ihog_cost_func::last_xformed_image()
{
  return roi_.resample(to_image_);
}
