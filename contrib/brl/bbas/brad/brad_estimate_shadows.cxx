#include <iostream>
#include <cmath>
#include <vil/vil_image_view.h>
#include <vnl/vnl_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "brad_image_metadata.h"
#include "brad_atmospheric_parameters.h"
#include "brad_illum_util.h"
#include "brad_estimate_shadows.h"

bool brad_estimate_shadow_prob_density(vil_image_view<float> const& radiance_image, brad_image_metadata const& mdata, brad_atmospheric_parameters const& atm_params, vil_image_view<float> &shadow_prob_density)
{
   // set sun irradiance to 0 in metadata
   brad_image_metadata mdata_nosun = mdata;
   mdata_nosun.sun_irradiance_ = 0.0;
   double mean_reflectance = 0.2;
   double reflectance_sigma = 0.15;
   double airlight_sigma = 5.0;
   double skylight_sigma = 2.5;
   double optical_depth_sigma = 0.02;

   double shadow_rad = brad_expected_radiance_chavez(mean_reflectance, vgl_vector_3d<double>(0,0,1), mdata_nosun, atm_params);
   double radiance_var = brad_radiance_variance_chavez(0.2,vgl_vector_3d<double>(0,0,1), mdata_nosun, atm_params, reflectance_sigma*reflectance_sigma, optical_depth_sigma*optical_depth_sigma, skylight_sigma*skylight_sigma, airlight_sigma*airlight_sigma);
   double radiance_sigma = std::sqrt(radiance_var);
   double shadow_pdf_norm = vnl_math::one_over_sqrt2pi / radiance_sigma;

   std::cout << "estimated shadow radiance: " << shadow_rad << ", sigma = " << radiance_sigma << std::endl;

   shadow_prob_density.set_size(radiance_image.ni(), radiance_image.nj());
   for (unsigned int j=0; j<radiance_image.nj(); ++j) {
      for (unsigned int i=0; i<radiance_image.ni(); ++i) {
         double diff = radiance_image(i,j) - shadow_rad;
         shadow_prob_density(i,j) = float(shadow_pdf_norm * std::exp(-diff*diff/(2*radiance_var)));
      }
   }
   return true;
}

bool brad_estimate_shadow_prob(vil_image_view<float> const& radiance_image, brad_image_metadata const& mdata, brad_atmospheric_parameters const& atm_params, vil_image_view<float> &shadow_prob)
{
  vil_image_view<float> shadow_density(radiance_image.ni(), radiance_image.nj());
  bool result = brad_estimate_shadow_prob_density(radiance_image, mdata, atm_params, shadow_density);
  if (!result){
    return false;
  }

  double uniform_min = brad_expected_radiance_chavez(0.0, vgl_vector_3d<double>(0,0,1), mdata, atm_params);
  double uniform_max = brad_expected_radiance_chavez(1.0, vgl_vector_3d<double>(0,0,1), mdata, atm_params);

  float uniform_pd = 1.0f / float(uniform_max - uniform_min);
  if (!(uniform_max > uniform_min)) {
    std::cerr << "ERROR: brad_estimate_shadows: max radiance less than min radiance\n";
    return false;
  }

  shadow_prob.set_size(radiance_image.ni(), radiance_image.nj());

  for (unsigned int j=0; j<radiance_image.nj(); ++j) {
    for (unsigned int i=0; i<radiance_image.ni(); ++i) {
       float shadow_pd = shadow_density(i,j);
       shadow_prob(i,j) = shadow_pd / (shadow_pd + uniform_pd);
    }
  }
  return true;
}
