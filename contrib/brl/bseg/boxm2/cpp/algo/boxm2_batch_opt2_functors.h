#ifndef boxm2_batch_opt2_functors_h_
#define boxm2_batch_opt2_functors_h_
//:
// \file

#include <iostream>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <boxm2/cpp/algo/boxm2_gauss_grey_processor.h>
#include <bsta/algo/bsta_sigma_normalizer.h>
#include <boxm2/io/boxm2_stream_cache.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: compute average pre_i, vis_i and post_i for each cell, save the values in aux
template <boxm2_data_type APM_TYPE>
class boxm2_batch_update_opt2_pass2_functor
{
 public:
  //: "default" constructor
  boxm2_batch_update_opt2_pass2_functor() = default;

  bool init_data(std::vector<boxm2_data_base*> & datas,
                 vil_image_view<float> * pre_img,
                 vil_image_view<float> * vis_img,
                 vil_image_view<float> * beta_denom,
                 float model_prior,
                 vil_image_view<float> * alt_prob_img)
  {
    aux0_data_=new boxm2_data<BOXM2_AUX0>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    aux1_data_=new boxm2_data<BOXM2_AUX1>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());
    mog3_data_=new boxm2_data<APM_TYPE>(datas[3]->data_buffer(),datas[3]->buffer_length(),datas[3]->block_id());
    aux_data_ = new boxm2_data<BOXM2_AUX>(datas[4]->data_buffer(),datas[4]->buffer_length(),datas[4]->block_id());

    alpha_integral_.set_size(beta_denom->ni(), beta_denom->nj(),1);
    alpha_integral_.fill(0.0f);
    pre_img_ = pre_img;
    vis_img_ = vis_img;

    beta_denom_ = beta_denom;
    alt_prob_img_ = alt_prob_img;
    model_prior_ = model_prior;

    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i,unsigned j, float abs_depth =0.0)
  {
    boxm2_data<BOXM2_AUX0>::datatype & aux0 = aux0_data_->data()[index];
    boxm2_data<BOXM2_AUX1>::datatype & aux1 = aux1_data_->data()[index];

    if (aux1<1e-10f)return true;

    /////////////
    // compute appearance probability of observation
    float mean_obs =aux0/aux1;
    float PI=boxm2_processor_type<APM_TYPE>::type::prob_density(mog3_data_->data()[index], mean_obs);

    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    // update alpha integral
    alpha_integral_(i,j) += alpha * seg_len;

    // compute new visibility probability with updated alpha_integral
    float vis_prob_end = std::exp(-alpha_integral_(i,j));
    // grab this cell's pre and vis value
    float pre = (*pre_img_)(i,j);
    float vis = (*vis_img_)(i,j);

    // compute weight for this cell
    const float Omega = vis - vis_prob_end;

    // update vis and pre
    (*pre_img_)(i,j) +=  PI * Omega;
    (*vis_img_)(i,j) = vis_prob_end;

    boxm2_data<BOXM2_AUX>::datatype & aux = aux_data_->data()[index];

    // accumulate aux sample values
    aux[0] += (pre * model_prior_ + (*alt_prob_img_)(i,j)) * seg_len;
    aux[1] += vis * model_prior_ * seg_len;

    const float beta_num = pre + vis*PI;
    const float beta_num_expanded = (beta_num  * model_prior_) + (*alt_prob_img_)(i,j);
    const float beta_denom = (*beta_denom_)(i,j);
    const float beta_denom_expanded = (beta_denom * model_prior_) + (*alt_prob_img_)(i,j);

    float beta = 1.0f;
    if (beta_denom_expanded > 1e-5f) {
      beta =  beta_num_expanded / beta_denom_expanded;
    }
    const float old_PQ = (float)(1.0 - std::exp(-alpha*seg_len));
    const float new_PQ = old_PQ * beta;
    const float pass_prob_old = 1.0f - old_PQ;
    float pass_prob = 1.0f - new_PQ;

    // compute expected information gained from update
    const float weight = new_PQ * std::log(new_PQ / old_PQ) + pass_prob * std::log(pass_prob / pass_prob_old);
    // ensure log doesn't go to infinity
    if (pass_prob < 1e-5f) {
      pass_prob = 1e-5f;
    }
    aux[2] += std::log(pass_prob) * weight/seg_len; //aux_val.log_pass_prob_sum_ +=
    aux[3] += weight; // aux_val.weighted_seg_len_sum_

    return true;
  }
 private:
  boxm2_data<BOXM2_AUX0> * aux0_data_;
  boxm2_data<BOXM2_AUX1> * aux1_data_;
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  boxm2_data<APM_TYPE> * mog3_data_;
  boxm2_data<BOXM2_AUX> * aux_data_;

  vil_image_view<float> alpha_integral_;
  vil_image_view<float> * pre_img_;
  vil_image_view<float> * vis_img_;
  vil_image_view<float> * beta_denom_;
  vil_image_view<float> * alt_prob_img_;
  float model_prior_;
};


template <boxm2_data_type APM_TYPE>
class boxm2_batch_update_opt2_functor
{
 public:
  typedef boxm2_data_traits<BOXM2_AUX0>::datatype aux0_datatype;
  typedef boxm2_data_traits<BOXM2_AUX1>::datatype aux1_datatype;
  typedef boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::datatype nrays_datatype;
  typedef boxm2_data_traits<BOXM2_AUX>::datatype aux_datatype;

  //: "default" constructor
  boxm2_batch_update_opt2_functor() = default;

  bool init_data(boxm2_data_base *alph, boxm2_data_base *mog, boxm2_stream_cache_sptr str_cache, bsta_sigma_normalizer_sptr n_table)
  {
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(alph->data_buffer(),alph->buffer_length(),alph->block_id());
    mog_data_ = new boxm2_data<APM_TYPE>(mog->data_buffer(), mog->buffer_length(), mog->block_id());
    str_cache_ = str_cache;
    id_ = alph->block_id();
    n_table_ = n_table;
    return true;
  }

  inline bool process_cell(int index)
  {
    boxm2_data<BOXM2_ALPHA>::datatype & alpha=alpha_data_->data()[index];
    typename boxm2_data<APM_TYPE>::datatype & mog=mog_data_->data()[index];

    std::vector<aux0_datatype> aux0 = str_cache_->get_next<BOXM2_AUX0>(id_, index);
    std::vector<aux1_datatype> aux1 = str_cache_->get_next<BOXM2_AUX1>(id_, index);
    std::vector<aux_datatype> aux = str_cache_->get_next<BOXM2_AUX>(id_, index);
    std::vector<nrays_datatype> nrays = str_cache_->get_next<BOXM2_NUM_OBS_SINGLE>(id_, index);

    std::vector<float> pre_vector;
    std::vector<float> vis_vector;
    std::vector<float> obs_vector;

    // UPDATE METHOD #1 : "OR" of occlusion probabilities
    double log_pass_prob_sum = 0.0;
    double weighted_seg_len_sum = 0.0;
    float max_obs_seg_len = 0.0f;  // max gives the best idea about the size of the cell

#if 0
    int cell_no = 2000000;

    if (index == cell_no) {
      std::cout << "stream cache read:\n"
               << "aux0: ";
      for (unsigned int s=0; s<aux.size(); ++s)
        std::cout << aux0[s] << ' ';
      std::cout << "\naux1: ";
      for (unsigned int s=0; s<aux.size(); ++s)
        std::cout << aux1[s] << ' ';
      std::cout << "\naux[0]: ";
      for (unsigned int s=0; s<aux.size(); ++s)
        std::cout << aux[s][0] << ' ';
      std::cout << "\naux[1]: ";
      for (unsigned int s=0; s<aux.size(); ++s)
        std::cout << aux[s][1] << ' ';
      std::cout << "\naux[2]: ";
      for (unsigned int s=0; s<aux.size(); ++s)
        std::cout << aux[s][2] << ' ';
      std::cout << "\naux[3]: ";
      for (unsigned int s=0; s<aux.size(); ++s)
        std::cout << aux[s][3] << ' ';
      std::cout << "\nnrays ";
      for (unsigned int s=0; s<aux.size(); ++s)
        std::cout << nrays[s] << ' ';
    }
#endif
    for (unsigned int s=0; s<aux.size(); ++s) {
      float obs_seg_len = aux1[s]; // .seg_len_;
      max_obs_seg_len = max_obs_seg_len > obs_seg_len/nrays[s] ? max_obs_seg_len : obs_seg_len/nrays[s];
      float obs_weighted_seg_len = aux[s][3]; //.weighted_seg_len_sum_;

      if (obs_seg_len > 1e-6) {
        if (obs_weighted_seg_len > 1e-6) {
          log_pass_prob_sum += aux[s][2]; //.log_pass_prob_sum_;
          weighted_seg_len_sum += obs_weighted_seg_len;
        }
        // add observation and vis,pre to list
        obs_vector.push_back(aux0[s] /*.obs_*/ / obs_seg_len);
        pre_vector.push_back(aux[s][0] /*.pre_ */ / obs_seg_len);
        vis_vector.push_back(aux[s][1] /*.vis_ */ / obs_seg_len);
      }
#if 0
      if (index == cell_no) {
        float mean_obs = aux0[s]/obs_seg_len;
        float PI = boxm2_processor_type<APM_TYPE>::type::prob_density(mog, mean_obs);
        std::cout << "\t m: " << s << " pre_i: " << aux[s][0]/obs_seg_len << " vis_i: " << aux[s][1]/obs_seg_len << '\n'
                 << "obs_seg_len: " << obs_seg_len << " PI: " << PI << " mean_obs: " << mean_obs << '\n'
                 << "max_obs_seg_len: " << max_obs_seg_len << std::endl;
      }
#endif
    }
#if 0
    if (index == cell_no) {
      float p_q = 1.0f-std::exp(-alpha*max_obs_seg_len);
      std::cout << "current alpha: " << alpha << " p_q: " << p_q << std::endl;
    }
#endif

    // update the occlusion density
    if (weighted_seg_len_sum > 1e-6) {
      alpha = (float)(-log_pass_prob_sum / weighted_seg_len_sum);
    }
#if 0
    if (index == cell_no) {
      float p_q_new = 1.0f-std::exp(-alpha*max_obs_seg_len);
      std::cout << "after update alpha: " << alpha << " p_q_new: " << p_q_new << std::endl;
    }
#endif

    float alpha_min = -std::log(1.f-0.0001f)/max_obs_seg_len;
    float alpha_max = -std::log(1.f-0.995f)/max_obs_seg_len;

    if (alpha > alpha_max) {
      alpha = alpha_max;
    }
    if (alpha < alpha_min) {
      alpha = alpha_min;
    }

    // update with new appearance
    const float min_sigma = 0.02f;
    //boxm_apm_traits<APM>::apm_processor::compute_appearance(obs_vector, pre_vector, vis_vector, data.appearance_, min_sigma);
    //boxm2_processor_type<APM_TYPE>::type::compute_app_model(mog,obs_vector, vis_vector,n_table_,min_sigma);
    boxm2_processor_type<APM_TYPE>::type::compute_app_model(mog,obs_vector, pre_vector,vis_vector,n_table_,min_sigma);

    return true;
  }

 private:
  boxm2_data<BOXM2_ALPHA>* alpha_data_;
  boxm2_data<APM_TYPE>* mog_data_;
  boxm2_stream_cache_sptr str_cache_;
  boxm2_block_id id_;
  bsta_sigma_normalizer_sptr n_table_;
};


#endif // boxm2_batch_opt2_functors_h_
