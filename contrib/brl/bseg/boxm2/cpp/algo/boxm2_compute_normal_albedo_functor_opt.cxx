// This is brl/bseg/boxm2/cpp/algo/boxm2_compute_normal_albedo_functor_opt.cxx
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
#include <vnl/algo/vnl_brent_minimizer.h>

#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>
#include <brad/brad_illum_util.h>

#include "boxm2_compute_normal_albedo_functor_opt.h"


double boxm2_compute_normal_albedo_cost_function::f(vnl_vector<double> const& x)
{
   const double rho = x[0];
   double log_prob = 0.0;
   for (unsigned int m=0; m<num_images_; ++m) {
      double rad_var = radiance_var_scales_[m]*rho*rho + radiance_var_offsets_[m];
#if 0
      double weight = visibilities_[m];
      if (rad_var <= 0.0) {
         // indicates that surface with given normal is not visible from this view
         double background_density = 0.01; // TODO: compute actual uniform density value
         log_prob += std::log((1.0 - weight)*background_density);
      }
      else {
         double diff = (radiance_scales_[m]*rho + radiance_offsets_[m] - radiances_[m]);
         double log_norm = -0.5 * std::log(vnl_math::twopi * rad_var);
         log_prob += weight * (log_norm - (diff*diff / (2.0*rad_var)));
      }
#endif
      double prob = 0.0;
      if (rad_var > 0.0) {
         double diff = (radiance_scales_[m]*rho + radiance_offsets_[m] - radiances_[m]);
         prob = visibilities_[m] * std::exp(-diff*diff/(2.0*rad_var)) / std::sqrt(vnl_math::twopi*rad_var);
      }
      constexpr double background_density = 0.01; // TODO: compute actual uniform density value
      prob += (1.0 - visibilities_[m])*background_density;
      if (!(prob >= 1e-6)) {
         prob = 1e-6;
      }
      log_prob += std::log(prob);
   }
   return -log_prob;
}


bool boxm2_compute_normal_albedo_functor_opt::init_data(std::vector<brad_image_metadata> const& metadata,
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
      std::cerr << "ERROR: boxm2_compute_normal_albedo_functor_opt: image metadata.size() = "
               << num_images_ << ", atm_params_.size() = " << atm_params_.size() << '\n';
      return false;
   }
   // get normal directions
   std::vector<vgl_vector_3d<double> > normals = boxm2_normal_albedo_array::get_normals();
   num_normals_ = normals.size();

   // compute offsets and scales for linear radiance model
   //reflectance_scales_.resize(num_normals_);
   //reflectance_offsets_.resize(num_normals_);
   radiance_scales_.resize(num_normals_);
   radiance_offsets_.resize(num_normals_);
   radiance_var_scales_.resize(num_normals_);
   radiance_var_offsets_.resize(num_normals_);

   for (unsigned int n=0; n<num_normals_; ++n) {
      radiance_scales_[n].resize(num_images_);
      radiance_offsets_[n].resize(num_images_);
      radiance_var_scales_[n].resize(num_images_);
      radiance_var_offsets_[n].resize(num_images_);
      for (unsigned int m=0; m<num_images_; ++m) {
         // compute offsets as radiance of surface with 0 reflectance
         double radiance_offset = brad_expected_radiance_chavez(0.0, normals[n], metadata_[m], atm_params_[m]);
         radiance_offsets_[n][m] = radiance_offset;
         // use perfect reflector to compute radiance scale
         double radiance = brad_expected_radiance_chavez(1.0, normals[n], metadata_[m], atm_params_[m]);
         radiance_scales_[n][m] = radiance - radiance_offset;
         // compute offset of radiance variance
         double var_offset = brad_radiance_variance_chavez(0.0, normals[n], metadata_[m], atm_params_[m], reflectance_var, optical_depth_var, skylight_var, airlight_var);
         radiance_var_offsets_[n][m] = var_offset;
         // compute scale
         double var = brad_radiance_variance_chavez(1.0, normals[n], metadata_[m], atm_params_[m], reflectance_var, optical_depth_var, skylight_var, airlight_var);
         radiance_var_scales_[n][m] = var - var_offset;
      }
   }
   return true;
}


bool boxm2_compute_normal_albedo_functor_opt::process_cell(unsigned int index, bool  /*is_leaf*/, float  /*side_len*/)
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

   if (aux0_raw.size() != num_images_) {
      std::cerr << "ERROR: boxm2_compute_normal_albedo_functor_opt : aux0_raw.size() " << aux0_raw.size() << " != num_images_ " << num_images_ << '\n';
      return false;
   }

   std::vector<double> radiances(num_images_, 0.0);
   std::vector<double> vis_vals(num_images_, 0.0);

   for (unsigned m = 0; m < num_images_; ++m) {
      if (aux0_raw[m]>1e-10f)
      {
         radiances[m] = aux1_raw[m] / aux0_raw[m];
         vis_vals[m] = aux2_raw[m] / aux0_raw[m];
         //aux3_raw[m] /= aux0_raw[m];
      }
   }

   double prob_sum = 0.0;
   std::vector<double> normal_probs(num_normals_);
   for (unsigned int n=0; n<num_normals_; ++n) {
      boxm2_compute_normal_albedo_cost_function cost_fun(radiances, vis_vals, radiance_scales_[n], radiance_offsets_[n], radiance_var_scales_[n], radiance_var_offsets_[n]);
      vnl_brent_minimizer bmin(cost_fun);
      double albedo = bmin.minimize_given_bounds(0.0, 0.2, 1.0);
      naa_model.set_albedo(n,float(albedo));
      double opt_prob = std::exp(-1.0 * bmin.f_at_last_minimum());
      normal_probs[n] = opt_prob;
      prob_sum += opt_prob;
   }
   if (prob_sum > 1e-100) {
      for (unsigned int n=0; n<num_normals_; ++n) {
         naa_model.set_probability(n,float(normal_probs[n]/prob_sum));
      }
   }
   else {
      for (unsigned int n=0; n<num_normals_; ++n) {
         naa_model.set_probability(n,1.0f/num_normals_);
      }
   }
   return true;
}
