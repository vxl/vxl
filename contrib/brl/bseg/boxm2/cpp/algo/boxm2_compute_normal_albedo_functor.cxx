// This is brl/bseg/boxm2/cpp/algo/boxm2_compute_normal_albedo_functor.cxx
#include <vector>
#include <iostream>
#include <algorithm>
#include <boxm2/io/boxm2_stream_cache.h>
#include <bsta/bsta_histogram.h>
#include <boxm2/boxm2_data_traits.h>
#include <vgl/vgl_vector_3d.h>
#include <vnl/vnl_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_erf.h>

#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>
#include <brad/brad_illum_util.h>

#include "boxm2_compute_normal_albedo_functor.h"

bool boxm2_compute_normal_albedo_functor::init_data(std::vector<brad_image_metadata> const& metadata,
                                                    std::vector<brad_atmospheric_parameters> const& atm_params,
                                                    const boxm2_stream_cache_sptr& str_cache,
                                                    boxm2_data_base * alpha_data,
                                                    boxm2_data_base * normal_albedo_model)
{
   // variances
   const double reflectance_var = boxm2_normal_albedo_array_constants::sigma_albedo * boxm2_normal_albedo_array_constants::sigma_albedo;
   const double airlight_var = boxm2_normal_albedo_array_constants::sigma_airlight * boxm2_normal_albedo_array_constants::sigma_airlight;
   const double optical_depth_var = boxm2_normal_albedo_array_constants::sigma_optical_depth * boxm2_normal_albedo_array_constants::sigma_optical_depth;
   const double skylight_var = boxm2_normal_albedo_array_constants::sigma_skylight * boxm2_normal_albedo_array_constants::sigma_skylight;

   naa_model_data_ = new boxm2_data<BOXM2_NORMAL_ALBEDO_ARRAY>(normal_albedo_model->data_buffer(),
                                                               normal_albedo_model->buffer_length(),
                                                               normal_albedo_model->block_id());

   alpha_data_ = new boxm2_data<BOXM2_ALPHA>(alpha_data->data_buffer(),
                                             alpha_data->buffer_length(),
                                             alpha_data->block_id());
   str_cache_ = str_cache;
   id_ = normal_albedo_model->block_id();
   metadata_ = metadata;
   atm_params_ = atm_params;
   // fill in number of images
   num_images_ = metadata.size();
   // sanity check on atmospheric parameters
   if (atm_params_.size() != num_images_) {
      std::cerr << "ERROR: boxm2_compute_normal_albedo_functor: image metadata.size() = "
               << num_images_ << ", atm_params_.size() = " << atm_params_.size() << '\n';
      return false;
   }
   // get normal directions
   normals_ = boxm2_normal_albedo_array::get_normals();
   num_normals_ = normals_.size();

   // compute offsets and scales for linear radiance model
   reflectance_scales_.resize(num_images_);
   reflectance_offsets_.resize(num_images_);
   radiance_scales_.resize(num_images_);
   radiance_offsets_.resize(num_images_);
   radiance_var_scales_.resize(num_images_);
   radiance_var_offsets_.resize(num_images_);
   sun_positions_.resize(num_images_);
   for (unsigned int m=0; m<num_images_; ++m) {
      // convert sun and view az,el into 3d vectors
      double az = metadata_[m].sun_azimuth_ * vnl_math::pi_over_180; //convert to radians
      double el = metadata_[m].sun_elevation_ * vnl_math::pi_over_180; //convert to radians
      double x = std::sin(az)*std::cos(el);
      double y = std::cos(az)*std::cos(el);
      double z = std::sin(el);
      sun_positions_[m] = vgl_vector_3d<double>(x,y,z);

      reflectance_scales_[m].resize(num_normals_);
      reflectance_offsets_[m].resize(num_normals_);
      radiance_scales_[m].resize(num_normals_);
      radiance_offsets_[m].resize(num_normals_);
      radiance_var_scales_[m].resize(num_normals_);
      radiance_var_offsets_[m].resize(num_normals_);
      for (unsigned n=0; n < num_normals_; ++n) {
         // compute offsets as radiance of surface with 0 reflectance
         double radiance_offset = brad_expected_radiance_chavez(0.0, normals_[n], metadata_[m], atm_params_[m]);
         radiance_offsets_[m][n] = radiance_offset;
         // use perfect reflector to compute radiance scale
         double radiance = brad_expected_radiance_chavez(1.0, normals_[n], metadata_[m], atm_params_[m]);
         radiance_scales_[m][n] = radiance - radiance_offset;
         // compute offset of radiance variance
         double var_offset = brad_radiance_variance_chavez(0.0, normals_[n], metadata_[m], atm_params_[m], reflectance_var, optical_depth_var, skylight_var, airlight_var);
         radiance_var_offsets_[m][n] = var_offset;
         // compute scale
         double var = brad_radiance_variance_chavez(1.0, normals_[n], metadata_[m], atm_params_[m], reflectance_var, optical_depth_var, skylight_var, airlight_var);
         radiance_var_scales_[m][n] = var - var_offset;
         // compute offset for reflectance calculation
         double reflectance_offset = brad_expected_reflectance_chavez(0.0, normals_[n], metadata_[m], atm_params_[m]);
         reflectance_offsets_[m][n] = reflectance_offset;
         // compute scale
         double reflectance = brad_expected_reflectance_chavez(1.0, normals_[n], metadata_[m], atm_params_[m]);
         reflectance_scales_[m][n] = reflectance - reflectance_offset;
      }
   }
   return true;
}


bool boxm2_compute_normal_albedo_functor::process_cell(unsigned int index, bool  /*is_leaf*/, float  /*side_len*/)
{
   if (index >= naa_model_data_->data().size()) {
      std::cerr << "ERROR: index = " << index << ", naa_model_data_->data().size = " << naa_model_data_->data().size() << '\n'
               << "   alpha_data_->data().size() = " << alpha_data_->data().size() << '\n';
      return false;
   }
   if (index >= alpha_data_->data().size()) {
      std::cerr << "ERROR: index = " << index << ", alpha_data_->data().size = " << alpha_data_->data().size() << '\n';
      return false;
   }
   boxm2_data<BOXM2_NORMAL_ALBEDO_ARRAY>::datatype & naa_model = naa_model_data_->data()[index];
   std::vector<aux0_datatype> aux0_raw = str_cache_->get_next<BOXM2_AUX0>(id_, index); // seg_len
   std::vector<aux1_datatype> aux1_raw = str_cache_->get_next<BOXM2_AUX1>(id_, index); // mean_obs
   std::vector<aux2_datatype> aux2_raw = str_cache_->get_next<BOXM2_AUX2>(id_, index); // vis
   std::vector<aux3_datatype> aux3_raw = str_cache_->get_next<BOXM2_AUX3>(id_, index); // pre

   for (unsigned m = 0; m < aux0_raw.size(); ++m) {
      if (aux0_raw[m]>1e-10f)
      {
         aux1_raw[m] /=aux0_raw[m];
         aux2_raw[m] /=aux0_raw[m];
         aux3_raw[m] /=aux0_raw[m];
      }
      else
      {
         aux1_raw[m] = 0.0;
         aux2_raw[m] = 0.0;
      }
   }

   std::vector<aux1_datatype> radiances;
   std::vector<aux2_datatype> vis_vals;

   vis_vals.insert(vis_vals.begin(), aux2_raw.begin(), aux2_raw.end());
   radiances.insert(radiances.begin(), aux1_raw.begin(), aux1_raw.end());

   if (radiances.size() != num_images_) {
      std::cerr << "ERROR: boxm2_compute_normal_albedo_functor: aux data size does not match number of images" << '\n';
      return false;
   }

   float sum_weights = 0.0f ;
   for (unsigned i=0; i<num_images_; ++i)
   {
      sum_weights += vis_vals[i];
   }

   //////////////////////////////////////

   double prob_sum = 0.0;

   // for each normal, compute optimal reflectance
   std::vector<double> obs_probs(num_normals_);
   for (unsigned int n=0; n<num_normals_; ++n) {
      vgl_vector_3d<double> normal = normals_[n];
      double numerator = 0.0;
      double denominator = 0.0;
      for (unsigned int m=0; m<num_images_; ++m) {
         double sun_dot = dot_product(sun_positions_[m],normal);
         if (sun_dot <= 0.1) {
            // self shadow or grazing sun angle, don't use intensity for albedo computation
            continue;
         }
         double rho = reflectance_scales_[m][n] * radiances[m] + reflectance_offsets_[m][n];
         if (rho < 0.0) {
            rho = 0.0;
         }
         if (rho > 1.0) {
            rho = 1.0;
         }
         double pred_var = radiance_var_scales_[m][n]*rho*rho + radiance_var_offsets_[m][n];
         if (!(pred_var > 0.1)) {
            std::cerr << "------- ERROR: prediction variance = " << pred_var << '\n';
            pred_var = 1.0;
         }
         double weight = vis_vals[m] / (1.0 + std::sqrt(pred_var));
         numerator += weight*rho;
         denominator += weight;
      }
      double albedo = 0.0;
      if (denominator > 0.0) {
         albedo = numerator / denominator;
      }
      naa_model.set_albedo(n,(float)albedo);

      // compute "score" based on agreement between predicted and actual intensities
      double log_pred_prob = 0.0;
      for (unsigned int m=0; m<num_images_; ++m) {
         double predicted = radiance_scales_[m][n]*albedo + radiance_offsets_[m][n];
         double prediction_var = radiance_var_scales_[m][n]*albedo*albedo + radiance_var_offsets_[m][n];
         if (prediction_var < 1e-6) {
            prediction_var = 1e-6;
            std::cerr << "ERROR: boxm2_compute_normal_albedo_functor: prediction variance = " << prediction_var << std::endl;
         }
         double weight = vis_vals[m];
         double radiance = radiances[m];
         double intensity_diff = predicted - radiance;
         double gauss_norm = vnl_math::one_over_sqrt2pi / std::sqrt(prediction_var);
         double intensity_prob = gauss_norm * std::exp((-intensity_diff*intensity_diff)/(2.0*prediction_var));
         double airlight_mean = radiance_offsets_[m][n];
         double marginal_density;
         if (radiance_scales_[m][n] > 1.0) {
            constexpr double Lmin = 0;
            const double Lmax = radiance_scales_[m][n];
            // assume radiance is a sum of uniform random variable (function of albedo) + gaussian distributed variable (airlight)
            //marginal_density = (vnl_erf(vnl_math::sqrt2*(Lmin + airlight_mean - radiance)/(2.0*boxm2_normal_albedo_array_constants::sigma_airlight)) - vnl_erf(vnl_math::sqrt2*(Lmax + airlight_mean - radiance)/(2.0*boxm2_normal_albedo_array_constants::sigma_airlight)))/(2.0*(Lmax-Lmin));
            // appoximate with just uniform density
            marginal_density = 1.0 / (Lmax - Lmin);
         }
         else {
            // assume radiance is Gaussian distributed around airlight
            const double diff = radiance - airlight_mean;
            const double sigma = boxm2_normal_albedo_array_constants::sigma_airlight;
            marginal_density = vnl_math::one_over_sqrt2pi / sigma * std::exp(-diff*diff/(2*sigma*sigma));
         }
         double prob = weight * intensity_prob + (1.0 - weight)*marginal_density;
         if (!(prob > 1e-4)) {
            prob = 1e-4;
         }
         log_pred_prob += std::log(prob);
         if (!(log_pred_prob <= 0)){
            std::cerr << "error\n";
         }
      }
      double pred_prob = std::exp(log_pred_prob);
      obs_probs[n] = pred_prob;
      prob_sum += pred_prob;
   }
   if (prob_sum <= 1e-40) {
      for (unsigned int n=0; n<num_normals_; ++n) {
         naa_model.set_probability(n,1.0f/num_normals_);
      }
   }
   else {
      for (unsigned int n=0; n<num_normals_; ++n) {
         double normal_prob = obs_probs[n]/prob_sum;
         if (!(normal_prob >= 0)) {
            std::cerr << "ERROR: normal_prob[" << n << "] = " << normal_prob << '\n';
            normal_prob = 0.0;
         }
         naa_model.set_probability(n,(float)normal_prob);
      }
   }
#if 0
   boxm2_data<BOXM2_ALPHA>::datatype & alpha = alpha_data_->data()[index];
   if (update_alpha_) {
      const double surface_prior = 1.0 - std::exp(-alpha*side_len);
      double total_obs_prob = surface_prior * surface_obs_density + (1.0-surface_prior)*empty_obs_density; // uniform density for "empty" model
      double vox_prob = surface_prior*surface_obs_density/total_obs_prob;
      double uncertainty = std::min(vox_prob,1.0-vox_prob)*2.0;
      double belief = vox_prob - uncertainty/2.0;
      alpha = -std::log(1.0 - belief) / side_len;
      }
#endif
   return true;
}
