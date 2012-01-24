#ifndef boxm2_compute_normal_albedo_functor_h_
#define boxm2_compute_normal_albedo_functor_h_
//:
// \file

#include <boxm2/io/boxm2_stream_cache.h>
#include <bsta/bsta_histogram.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/boxm2_normal_albedo_array.h>
#include <bpro/core/bbas_pro/bbas_1d_array_float.h>
#include <vgl/vgl_vector_3d.h>
#include <vnl/vnl_random.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

class boxm2_compute_normal_albedo_functor
{
 public:
  typedef boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::datatype naa_datatype;
  typedef boxm2_data_traits<BOXM2_ALPHA>::datatype alpha_datatype;
  typedef boxm2_data_traits<BOXM2_AUX0>::datatype aux0_datatype;
  typedef boxm2_data_traits<BOXM2_AUX1>::datatype aux1_datatype;
  typedef boxm2_data_traits<BOXM2_AUX2>::datatype aux2_datatype;
  typedef boxm2_data_traits<BOXM2_AUX3>::datatype aux3_datatype;

  //: standard constructor
  boxm2_compute_normal_albedo_functor(bool update_alpha)  : update_alpha_(update_alpha) {}

  bool init_data(bbas_1d_array_float_sptr sun_azim,
                 bbas_1d_array_float_sptr sun_elev,
                 bbas_1d_array_float_sptr image_irrad,
                 boxm2_stream_cache_sptr str_cache,
                 boxm2_data_base * alpha_data,
                 boxm2_data_base * normal_albedo_model)
  {
    naa_model_data_ = new boxm2_data<BOXM2_NORMAL_ALBEDO_ARRAY>(normal_albedo_model->data_buffer(),
                                                                normal_albedo_model->buffer_length(),
                                                                normal_albedo_model->block_id());

    alpha_data_ = new boxm2_data<BOXM2_ALPHA>(alpha_data->data_buffer(),
                                              alpha_data->buffer_length(),
                                              alpha_data->block_id());
    str_cache_ = str_cache;
    id_ = normal_albedo_model->block_id();
    sun_elev_ = sun_elev;
    sun_azim_ = sun_azim;
    image_irrad_ = image_irrad;

    return true;
  }

  inline bool process_cell(unsigned int index, bool is_leaf = false, float side_len = 0.0)
  {
    if (index >= naa_model_data_->data().size()) {
      vcl_cerr << "ERROR: index = " << index << ", naa_model_data_->data().size = " << naa_model_data_->data().size() << vcl_endl
               << "   alpha_data_->data().size() = " << alpha_data_->data().size() << vcl_endl;
      return false;
    }
    if (index >= alpha_data_->data().size()) {
      vcl_cerr << "ERROR: index = " << index << ", alpha_data_->data().size = " << alpha_data_->data().size() << vcl_endl;
      return false;
    }
    boxm2_data<BOXM2_NORMAL_ALBEDO_ARRAY>::datatype & naa_model = naa_model_data_->data()[index];
    boxm2_data<BOXM2_ALPHA>::datatype & alpha = alpha_data_->data()[index];
    vcl_vector<aux0_datatype> aux0_raw = str_cache_->get_next<BOXM2_AUX0>(id_, index); // seg_len
    vcl_vector<aux1_datatype> aux1_raw = str_cache_->get_next<BOXM2_AUX1>(id_, index); // mean_obs
    vcl_vector<aux2_datatype> aux2_raw = str_cache_->get_next<BOXM2_AUX2>(id_, index); // vis
    vcl_vector<aux3_datatype> aux3_raw = str_cache_->get_next<BOXM2_AUX3>(id_, index); // pre

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

    vcl_vector<aux1_datatype> intensities;
    vcl_vector<aux2_datatype> vis_vals;

    vis_vals.insert(vis_vals.begin(), aux2_raw.begin(), aux2_raw.end());
#if 1
    intensities.insert(intensities.begin(), aux1_raw.begin(), aux1_raw.end());
#else
    vnl_random randgen;
    for (unsigned int i=0; i<vis_vals.size(); ++i) {
      intensities.push_back(randgen.drand64());
    }
#endif


    //vcl_cout << "Iobs.size() = " << intensities.size() << vcl_endl;
    //vcl_cout << "vis.size() = " << vis_vals.size() << vcl_endl;

    const unsigned int num_images = intensities.size();

    // sanity check on sun azimuth array
    if (sun_azim_->data_array.size() != num_images) {
      vcl_cerr << "ERROR: sun_azim array size = " << sun_azim_->data_array.size() << ", num_images = " << num_images << vcl_endl;
      return false;
    }
    // sanity check on sun elevation array
    if (sun_elev_->data_array.size() != num_images) {
      vcl_cerr << "ERROR: sun_azim array size = " << sun_elev_->data_array.size() << ", num_images = " << num_images << vcl_endl;
      return false;
    }
    // sanity check on irradiance array
    if (image_irrad_->data_array.size() != num_images) {
       vcl_cerr << "ERROR: irradiance array size = " << image_irrad_->data_array.size() << ", num_images = " << num_images << vcl_endl;
      return false;
    }

    vcl_vector<vgl_vector_3d<double> > sun_positions;
    // convert sun az,el into 3d vectors
    for (unsigned int i=0; i<num_images; ++i) {
      double az = sun_azim_->data_array[i] * vnl_math::pi_over_180; //convert to radians
      double el = sun_elev_->data_array[i] * vnl_math::pi_over_180; //convert to radians
      double x = vcl_sin(az)*vcl_cos(el);
      double y = vcl_cos(az)*vcl_cos(el);
      double z = vcl_sin(el);
      sun_positions.push_back(vgl_vector_3d<double>(x,y,z));
    }

    float sum_weights = 0.0f ;
    for (unsigned i=0; i<num_images; ++i)
    {
      sum_weights += vis_vals[i];
    }

    //////////////////////////////////////
    vcl_vector<vgl_vector_3d<double> > normals = boxm2_normal_albedo_array::get_normals();
    unsigned int num_normals = normals.size();

    // fixed parameters

    const double sigma_sqrd_shadow = boxm2_normal_albedo_array_constants::sigma_shadow * boxm2_normal_albedo_array_constants::sigma_shadow;
    const double sigma_sqrd_irrad =  boxm2_normal_albedo_array_constants::sigma_irrad * boxm2_normal_albedo_array_constants::sigma_irrad;
    const double sigma_sqrd_albedo = boxm2_normal_albedo_array_constants::sigma_albedo * boxm2_normal_albedo_array_constants::sigma_albedo;

    const double uniform_density = 1.0;
    const double surface_prior = 1.0 - vcl_exp(-alpha*side_len);
//  const double normal_prior = surface_prior/num_normals;
    double prob_sum = 0.0;

    // for each normal, compute optimal albedo
    vcl_vector<double> obs_probs(num_normals);
    for (unsigned int n=0; n<num_normals; ++n) {
      vgl_vector_3d<double> normal = normals[n];
      double numerator = 0.0;
      double denominator = 0.0;
      for (unsigned int m=0; m<num_images; ++m) {
        double sun_dot = dot_product(sun_positions[m],normal);
        if (sun_dot <= 0.0) {
          // self shadow, don't use intensity for albedo computation
          continue;
        }
        double weight = vis_vals[m]; // weight observation by visibility probability
        double intensity_norm = intensities[m] / image_irrad_->data_array[m];
        numerator += weight*sun_dot*intensity_norm;
        denominator += weight*sun_dot*sun_dot;
      }
      double albedo = 0.0;
      if (denominator > 0.0) {
        albedo = numerator / denominator;
      }
      naa_model.set_albedo(n,albedo);

      // compute "score" based on agreement between predicted and actual intensities
      double log_pred_prob = 0.0;
      for (unsigned int m=0; m<num_images; ++m) {
        double sun_dot = dot_product(sun_positions[m],normal);
        double irrad = image_irrad_->data_array[m];
        double predicted, pred_sigma_sqrd;
        if (sun_dot <= 0.0) {
          // should be in shadow
          predicted = 0.0;
          pred_sigma_sqrd = sigma_sqrd_shadow;
        }
        else {
          predicted = irrad*albedo*sun_dot;
          // note this estimate of the prediction variance ignores second order term (product of albedo and irradiance errors)
          pred_sigma_sqrd = sun_dot*sun_dot*(irrad*irrad*sigma_sqrd_albedo + albedo*albedo*sigma_sqrd_irrad);
        }
        double weight = vis_vals[m];
        double intensity = intensities[m];
        double intensity_diff = predicted - intensity;
        double gauss_norm = vnl_math::one_over_sqrt2pi / vcl_sqrt(pred_sigma_sqrd);
        double intensity_prob = gauss_norm * vcl_exp((-intensity_diff*intensity_diff)/(2.0*pred_sigma_sqrd));
        double prob = weight * intensity_prob + (1.0 - weight)*uniform_density;
        //vcl_cout << "predicted=" <<predicted <<" actual=" << intensity_norm << "  prob["<<m<<"]=" << prob << "  ";
        log_pred_prob += vcl_log(prob);
        //vcl_cout << "predicted = " << predicted << " sigma = " << vcl_sqrt(pred_sigma_sqrd) << " actual = " << intensity << " albedo = " << albedo << " irrad = " << irrad << " weight = " << weight << " prob = " << prob << vcl_endl;
      }
      double pred_prob = vcl_exp(log_pred_prob);
      obs_probs[n] = pred_prob;
      prob_sum += pred_prob;
      //prob_sqrd_sum += pred_prob*pred_prob;
      //vcl_cout << "obs_prob[" << n << "] = " << obs_probs[n] << "  ";
    }
    //vcl_cout << vcl_endl;
    double surface_obs_density = 0.0;
    for (unsigned int n=0; n<num_normals; ++n) {
      double normal_prob = obs_probs[n]/prob_sum;
      naa_model.set_probability(n,normal_prob);
      surface_obs_density += normal_prob * obs_probs[n];
      //vcl_cout << " prob[" << n << "]= " << naa_model.get_probability(n) << "  ";
    }
    if (update_alpha_) {
      double total_obs_prob = surface_prior * surface_obs_density + (1.0-surface_prior)*uniform_density; // uniform density for "empty" model
      //vcl_cout << "TOTAL_OBS_PROB = " << total_obs_prob << "   ";
      double vox_prob = surface_prior*surface_obs_density/total_obs_prob;
      double uncertainty = vcl_min(vox_prob,1.0-vox_prob)*2.0;
      double belief = vox_prob - uncertainty/2.0;
#ifdef DEBUG
      vcl_cout << "surface_prior = " << surface_prior << "   surface_obs_density = " << surface_obs_density
               << "   posterior = " << vox_prob << "  belief = " << belief << vcl_endl;
#endif
      alpha = -vcl_log(1.0 - belief) / side_len;
    }
    /////////////////////////////////////

    return true;
  }

 private:
  boxm2_data<BOXM2_NORMAL_ALBEDO_ARRAY>* naa_model_data_;
  boxm2_data<BOXM2_ALPHA>* alpha_data_;

  boxm2_stream_cache_sptr str_cache_;
  boxm2_block_id id_;
  bbas_1d_array_float_sptr sun_elev_;
  bbas_1d_array_float_sptr sun_azim_;
  bbas_1d_array_float_sptr image_irrad_;

  bool update_alpha_;
};

#endif // boxm2_compute_normal_albedo_functor_h_
