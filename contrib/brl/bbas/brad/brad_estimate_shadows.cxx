#include <vil/vil_image_view.h>
#include <vnl/vnl_math.h>

#include "brad_image_metadata.h"
#include "brad_atmospheric_parameters.h"
#include "brad_illum_util.h"
#include "brad_estimate_shadows.h"

bool brad_estimate_shadows(vil_image_view<float> const& radiance_image, brad_image_metadata const& mdata, brad_atmospheric_parameters const& atm_params, vil_image_view<float> &shadow_prob)
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
   double radiance_sigma = sqrt(radiance_var);
   double shadow_pdf_norm = vnl_math::one_over_sqrt2pi / radiance_sigma;

   vcl_cout << "estimated shadow radiance: " << shadow_rad << ", sigma = " << radiance_sigma << vcl_endl;

   double uniform_min = brad_expected_radiance_chavez(0.0, vgl_vector_3d<double>(0,0,1), mdata, atm_params);
   double uniform_max = brad_expected_radiance_chavez(1.0, vgl_vector_3d<double>(0,0,1), mdata, atm_params);

   double uniform_pd = 1.0 / (uniform_max - uniform_min);
   if (!(uniform_max > uniform_min)) {
      vcl_cerr << "ERROR: brad_estimate_shadows: max radiance less than min radiance " << vcl_endl;
      return false;
   }

   shadow_prob.set_size(radiance_image.ni(), radiance_image.nj());
   for (unsigned int j=0; j<radiance_image.nj(); ++j) {
      for (unsigned int i=0; i<radiance_image.ni(); ++i) {
         double diff = radiance_image(i,j) - shadow_rad;
         double shadow_pd = shadow_pdf_norm * exp(-diff*diff/(2*radiance_var));
         shadow_prob(i,j) = shadow_pd / (shadow_pd + uniform_pd);
      }
   }

   return true;
}

