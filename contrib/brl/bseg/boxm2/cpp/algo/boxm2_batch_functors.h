#ifndef boxm2_batch_functors_h_
#define boxm2_batch_functors_h_
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

//: accumulate seg_lengths and intensities over all rays that pass through a cell to compute normalized intensity later
class boxm2_batch_update_pass0_functor
{
 public:
  //: "default" constructor (does nothing)
  boxm2_batch_update_pass0_functor() = default;

  bool init_data(std::vector<boxm2_data_base*> & datas, vil_image_view<float> * input_img)
  {
    aux0_data_=new boxm2_data<BOXM2_AUX0>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    aux1_data_=new boxm2_data<BOXM2_AUX1>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    aux2_data_=new boxm2_data<BOXM2_NUM_OBS_SINGLE>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());
    input_img_=input_img;
    return true;
  }
  inline bool step_cell(float seg_len,int index,unsigned i,unsigned j, float abs_depth = 0.0)
  {
    boxm2_data<BOXM2_AUX0>::datatype & aux0 = aux0_data_->data()[index];
    boxm2_data<BOXM2_AUX1>::datatype & aux1 = aux1_data_->data()[index];
    boxm2_data<BOXM2_NUM_OBS_SINGLE>::datatype & aux2 = aux2_data_->data()[index];

    aux0 += seg_len*(*input_img_)(i,j);
    aux1 += seg_len;
    aux2 += 1;  // keep number of rays

    return true;
  }
 private:
  boxm2_data<BOXM2_AUX0> * aux0_data_;
  boxm2_data<BOXM2_AUX1> * aux1_data_;
  boxm2_data<BOXM2_NUM_OBS_SINGLE> * aux2_data_;
  vil_image_view<float> * input_img_;
};


//: compute pre_inf, vis_inf
template <boxm2_data_type APM_TYPE>
class boxm2_batch_update_pass1_functor
{
 public:
  //: "default" constructor
  boxm2_batch_update_pass1_functor() = default;

  bool init_data(std::vector<boxm2_data_base*> & datas, vil_image_view<float>* pre_img, vil_image_view<float>* vis_img)
  {
    aux0_data_=new boxm2_data<BOXM2_AUX0>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    aux1_data_=new boxm2_data<BOXM2_AUX1>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());
    mog3_data_=new boxm2_data<APM_TYPE>(datas[3]->data_buffer(),datas[3]->buffer_length(),datas[3]->block_id());

    alpha_integral_.set_size(pre_img->ni(), pre_img->nj(),1);
    alpha_integral_.fill(0.0f);
    pre_img_ = pre_img;
    vis_img_ = vis_img;
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i,unsigned j, float abs_depth = 0.0)
  {
    boxm2_data<BOXM2_AUX0>::datatype & aux0 = aux0_data_->data()[index];
    boxm2_data<BOXM2_AUX1>::datatype & aux1 = aux1_data_->data()[index];

    if (aux1<1e-10f) return true;

    // compute average intensity for the cell
    float mean_obs =aux0/aux1;

    // compute appearance probability of observation
    float PI=boxm2_processor_type<APM_TYPE>::type::prob_density(mog3_data_->data()[index], mean_obs);
#if 0
    float vis=(*vis_img_)(i,j);
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];

    // update alpha integral
    alpha_integral_(i,j) += alpha * seg_len;
    // compute new visibility probability with updated alpha_integral
    const float vis_prob_end = std::exp(-alpha_integral_(i,j));
    // compute weight for this cell
    const float Omega = vis - vis_prob_end;
    // and update pre
    (*pre_img_)(i,j) +=  PI * Omega;
    // update visibility probability
    (*vis_img_)(i,j) = vis_prob_end;
#endif // 0

    float vis=(*vis_img_)(i,j);
    float pre=(*pre_img_)(i,j);
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    float temp=std::exp(-seg_len*alpha);
    pre+=vis*(1-temp)*PI;
    vis*=temp;
#if 0
    if (vis*(1-temp) < 0)
      std::cout << "ERROR" << std::endl;
#endif
    (*pre_img_)(i,j)=pre;
    (*vis_img_)(i,j)=vis;

    return true;
  }
 private:
  boxm2_data<BOXM2_AUX0> * aux0_data_;
  boxm2_data<BOXM2_AUX1> * aux1_data_;
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  boxm2_data<APM_TYPE> * mog3_data_;

  vil_image_view<float> alpha_integral_;
  vil_image_view<float> * pre_img_;
  vil_image_view<float> * vis_img_;
};


//: compute average pre_i, vis_i and post_i for each cell, save the values in aux
template <boxm2_data_type APM_TYPE>
class boxm2_batch_update_pass2_functor
{
 public:
  //: "default" constructor
  boxm2_batch_update_pass2_functor() = default;

  bool init_data(std::vector<boxm2_data_base*> & datas, vil_image_view<float> * pre_img, vil_image_view<float> * vis_img,
                 vil_image_view<float> * pre_inf,vil_image_view<float> * vis_inf)
  {
    aux0_data_=new boxm2_data<BOXM2_AUX0>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    aux1_data_=new boxm2_data<BOXM2_AUX1>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());
    mog3_data_=new boxm2_data<APM_TYPE>(datas[3]->data_buffer(),datas[3]->buffer_length(),datas[3]->block_id());
    aux_data_ = new boxm2_data<BOXM2_AUX>(datas[4]->data_buffer(),datas[4]->buffer_length(),datas[4]->block_id());

    alpha_integral_.set_size(pre_inf->ni(), pre_inf->nj(),1);
    alpha_integral_.fill(0.0f);
    pre_img_ = pre_img;
    vis_img_ = vis_img;
    pre_inf_ = pre_inf;
    vis_inf_ = vis_inf;

    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i,unsigned j, float abs_depth = 0.0)
  {
    boxm2_data<BOXM2_AUX0>::datatype & aux0 = aux0_data_->data()[index];
    boxm2_data<BOXM2_AUX1>::datatype & aux1 = aux1_data_->data()[index];

    if (aux1<1e-10f) return true;

    // compute average intensity for the cell
    float mean_obs = aux0/aux1;

    // compute appearance probability of observation
    float PI=boxm2_processor_type<APM_TYPE>::type::prob_density(mog3_data_->data()[index], mean_obs);

#if 0
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    // update alpha integral
    alpha_integral_(i,j) += alpha * seg_len;

    // compute new visibility probability with updated alpha_integral
    float vis_prob_end = std::exp(-alpha_integral_(i,j));

    // grab this cell's pre and vis value
    float pre = pre_img_(i,j);
    float vis = vis_img_(i,j);

    // compute weight for this cell
    float Omega = vis - vis_prob_end;

    float cell_value = PI*Omega;
    float post = (*pre_inf_)(i,j)-pre-cell_value;
    // now correct post
    post /= std::exp(-alpha*seg_len);

    // update vis and pre
    pre_img_(i,j) +=  cell_value;
    vis_img_(i,j) = vis_prob_end;
#endif // 0

    float vis=(*vis_img_)(i,j);
    float pre=(*pre_img_)(i,j);
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    float temp=std::exp(-seg_len*alpha);

    float post = (*pre_inf_)(i,j) - pre - vis*(1-temp)*PI;
    // now correct post
    if (post > 1e-8f)
      post /= temp;
    else
      post = 0;

    float PI_inf = 1; //assume uniformly distributed background model.
    post +=  (*vis_inf_)(i,j) * PI_inf;

    boxm2_data<BOXM2_AUX>::datatype & aux = aux_data_->data()[index];
    // accumulate aux sample values
    aux[0] += pre * seg_len;
    aux[1] += vis * seg_len;
    aux[2] += post * seg_len;

    pre+=vis*(1-temp)*PI;
    vis*=temp;
    (*pre_img_)(i,j)=pre;
    (*vis_img_)(i,j)=vis;

    return true;
  }
 private:
  boxm2_data<BOXM2_AUX0> * aux0_data_;
  boxm2_data<BOXM2_AUX1> * aux1_data_;
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  boxm2_data<APM_TYPE> * mog3_data_; // was: boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
  boxm2_data<BOXM2_AUX> * aux_data_;

#if 0
  vil_image_view<float> alpha_integral_;
  vil_image_view<float> pre_img_;  // these 2 can be local for this functor
  vil_image_view<float> vis_img_;
  vil_image_view<float> * pre_inf_;
#endif
  vil_image_view<float> alpha_integral_;
  vil_image_view<float> * pre_img_;
  vil_image_view<float> * vis_img_;
  vil_image_view<float> * pre_inf_;
  vil_image_view<float> * vis_inf_;
};

template <boxm2_data_type APM_TYPE>
class boxm2_batch_update_functor
{
 public:
  typedef boxm2_data_traits<BOXM2_AUX0>::datatype aux0_datatype;
  typedef boxm2_data_traits<BOXM2_AUX1>::datatype aux1_datatype;
  typedef boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::datatype nrays_datatype;
  typedef boxm2_data_traits<BOXM2_AUX>::datatype aux_datatype;

  //: "default" constructor
  boxm2_batch_update_functor() = default;

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

    std::vector<aux0_datatype> out0 = str_cache_->get_next<BOXM2_AUX0>(id_, index);
    std::vector<aux1_datatype> out1 = str_cache_->get_next<BOXM2_AUX1>(id_, index);
    std::vector<aux_datatype> out = str_cache_->get_next<BOXM2_AUX>(id_, index);
    std::vector<nrays_datatype> nrays = str_cache_->get_next<BOXM2_NUM_OBS_SINGLE>(id_, index);

    std::vector<aux0_datatype> obs;
    std::vector<float> vis;
    std::vector<float> pre;
    float term1 = 0.0f;  // product of the likelihoods that cell is a surface
    float term2 = 0.0f;  // product of the likelihoods that cell is not a surface
    unsigned nimgs = (unsigned)out0.size();

    float max_obs_seg_len = 0.0f;  // max gives the best idea about the size of the cell
    for (unsigned m = 0; m < nimgs; m++) {
      float obs_seg_len = out1[m];

      if (obs_seg_len>1e-8f) {
        float mean_obs = out0[m]/obs_seg_len;
        obs.push_back(mean_obs);

        if (max_obs_seg_len < obs_seg_len/nrays[m]) max_obs_seg_len = obs_seg_len/nrays[m];

        float PI = boxm2_processor_type<APM_TYPE>::type::prob_density(mog, mean_obs);
        float pre_i = out[m][0]/obs_seg_len; // mean pre
        pre.push_back(pre_i);
        float vis_i = out[m][1]/obs_seg_len; // mean vis
        vis.push_back(vis_i);
        float post_i = out[m][2]/obs_seg_len; // mean post

        term1 += std::log(pre_i + vis_i*PI);
        term2 += std::log(pre_i + post_i);    // no infinity term for now
      }
    }

    // update alpha and mog if ray hit the cell significantly
    if (max_obs_seg_len>1e-8f) {
      float p_q = 1.0f-std::exp(-alpha*max_obs_seg_len);

      //correct term1 and term2
      term1 = std::exp(term1);
      term2 = std::exp(term2);

      // compute new alpha value
      float p_q_new = p_q*term1 / (p_q*term1 + (1.0f-p_q)*term2);
      alpha = -std::log(1.0f-p_q_new)/max_obs_seg_len; //alpha *= p_q_new;

      float alpha_min = -std::log(1.f-0.0001f)/max_obs_seg_len;
      float alpha_max = -std::log(1.f-0.995f)/max_obs_seg_len;
      if (alpha < alpha_min)
        alpha = alpha_min;
      if (alpha > alpha_max)
       alpha = alpha_max;

      // compute new appearance model
      boxm2_processor_type<APM_TYPE>::type::compute_app_model(mog,obs,pre,vis,n_table_,0.03f);
    }
    return true;
  }

 private:
  boxm2_data<BOXM2_ALPHA>* alpha_data_;
  boxm2_data<APM_TYPE>* mog_data_;
  boxm2_stream_cache_sptr str_cache_;
  boxm2_block_id id_;
  float alpha_min_;
  float alpha_max_;
  bsta_sigma_normalizer_sptr n_table_;
};

template <boxm2_data_type APM_TYPE>
class boxm2_batch_update_app_functor
{
 public:
  typedef boxm2_data_traits<BOXM2_AUX0>::datatype aux0_datatype;
  typedef boxm2_data_traits<BOXM2_AUX1>::datatype aux1_datatype;
  typedef boxm2_data_traits<BOXM2_AUX2>::datatype aux2_datatype;
  typedef boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::datatype nrays_datatype;
  typedef boxm2_data_traits<BOXM2_AUX>::datatype aux_datatype;

  //: "default" constructor
  boxm2_batch_update_app_functor() = default;

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
    typename boxm2_data<APM_TYPE>::datatype & mog=mog_data_->data()[index];

    std::vector<aux0_datatype> out0 = str_cache_->get_next<BOXM2_AUX0>(id_, index);
    std::vector<aux1_datatype> out1 = str_cache_->get_next<BOXM2_AUX1>(id_, index);
    std::vector<aux2_datatype> out2 = str_cache_->get_next<BOXM2_AUX2>(id_, index);
#if 0
    std::vector<aux_datatype> out = str_cache_->get_next<BOXM2_AUX>(id_, index);
    std::vector<nrays_datatype> nrays = str_cache_->get_next<BOXM2_NUM_OBS_SINGLE>(id_, index);

    constexpr int cell_no = 2000000;
#endif

    std::vector<aux0_datatype> obs;
    std::vector<float> vis;
    std::vector<float> pre;
    unsigned nimgs = (unsigned)out0.size();

    for (unsigned m = 0; m < nimgs; m++) {
      float obs_seg_len = out0[m];
      if (obs_seg_len > 1e-12f) {
        float mean_obs = out1[m]/obs_seg_len;
        obs.push_back(mean_obs);

        //pre.push_back(out[m][0]/obs_seg_len); // mean pre
        float vis_i = out2[m]/obs_seg_len; // mean vis
        vis.push_back(vis_i);
#if 0
        if (index == cell_no) {
          std::cout << "\t m: " << m << " pre_i: " << pre_i << " vis_i: " << vis_i << '\n'
                   << "obs_seg_len: " << obs_seg_len << " mean_obs: " << mean_obs << std::endl;
        }
#endif
      }
    }

    // compute new appearance model
    boxm2_processor_type<APM_TYPE>::type::compute_app_model(mog,obs, vis,n_table_,0.03f);
    // was: boxm2_processor_type<APM_TYPE>::type::compute_app_model(mog,obs, pre,vis,n_table_,0.03f);

    return true;
  }

 private:
  boxm2_data<BOXM2_ALPHA>* alpha_data_;
  boxm2_data<APM_TYPE>* mog_data_;
  boxm2_stream_cache_sptr str_cache_;
  boxm2_block_id id_;
  bsta_sigma_normalizer_sptr n_table_;
};


template <boxm2_data_type APM_TYPE>
class boxm2_batch_update_alpha_functor
{
 public:
  typedef boxm2_data_traits<BOXM2_AUX0>::datatype aux0_datatype;
  typedef boxm2_data_traits<BOXM2_AUX1>::datatype aux1_datatype;
  typedef boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::datatype nrays_datatype;
  typedef boxm2_data_traits<BOXM2_AUX>::datatype aux_datatype;

  //: "default" constructor
  boxm2_batch_update_alpha_functor() = default;

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

    std::vector<aux0_datatype> out0 = str_cache_->get_next<BOXM2_AUX0>(id_, index);
    std::vector<aux1_datatype> out1 = str_cache_->get_next<BOXM2_AUX1>(id_, index);
    std::vector<aux_datatype> out = str_cache_->get_next<BOXM2_AUX>(id_, index);
    std::vector<nrays_datatype> nrays = str_cache_->get_next<BOXM2_NUM_OBS_SINGLE>(id_, index);

    int cell_no = 2000000;

    std::vector<aux0_datatype> obs;
    std::vector<float> vis;
    std::vector<float> pre;
    float term1 = 1.0f;  // product of the likelihoods that cell is a surface
    float term2 = 1.0f;  // product of the likelihoods that cell is not a surface
    unsigned nimgs = (unsigned)out0.size();

    float max_obs_seg_len = 0.0f;  // max gives the best idea about the size of the cell
    for (unsigned m = 0; m < nimgs; m++) {
      float obs_seg_len = out1[m];
      float mean_obs = out0[m]/obs_seg_len;
      obs.push_back(mean_obs);

      if (max_obs_seg_len < obs_seg_len/nrays[m]) max_obs_seg_len = obs_seg_len/nrays[m];

      float PI = boxm2_processor_type<APM_TYPE>::type::prob_density(mog, mean_obs);
      float pre_i = out[m][0]/obs_seg_len; // mean pre
      pre.push_back(pre_i);
      float vis_i = out[m][1]/obs_seg_len; // mean vis
      vis.push_back(vis_i);
      float post_i = out[m][2]/obs_seg_len; // mean post

      term1 *= pre_i + vis_i*PI;
      term2 *= pre_i + post_i;    // no infinity term for now

      if (index == cell_no) {
        std::cout << "\t m: " << m << " pre_i: " << pre_i << " vis_i: " << vis_i << " post_i: " << post_i << '\n'
                 << "obs_seg_len: " << obs_seg_len << " PI: " << PI << " mean_obs: " << mean_obs << '\n'
                 << "current term1: " << term1 << " term2: " << term2 << std::endl;
      }
    }
    float p_q = 1.0f-std::exp(-alpha*max_obs_seg_len);
    if (index == cell_no) {
      std::cout << "current alpha: " << alpha << " p_q: " << p_q << '\n';
    }

    // compute new alpha value
    float p_q_new = p_q*term1 / (p_q*term1 + (1.0f-p_q)*term2);
    alpha = -std::log(1.0f-p_q_new)/max_obs_seg_len; // was: alpha *= p_q_new;

    if (index == cell_no) {
      std::cout << "after update alpha: " << alpha << '\n';
    }

    float alpha_min = -std::log(1.f-0.0001f)/max_obs_seg_len;
    float alpha_max = -std::log(1.f-0.995f)/max_obs_seg_len;

    if (alpha < alpha_min)
      alpha = alpha_min;
#if 0
    float max_cell_P = 0.995f;
    float max_alpha = -std::log(1.0f - max_cell_P)/mean_len;
    if (alpha > max_alpha)
      alpha = max_alpha;
#endif
    if (alpha > alpha_max)
     alpha = alpha_max;

    if (index == cell_no) {
      std::cout << " alpha_min: " << alpha_min << " alpha_max: " << alpha_max << '\n'
               << " p_q_new: " << p_q_new << '\n'
               << "term1: " << term1 << " term2: " << term2 << std::endl;
    }

    return true;
  }

 private:
  boxm2_data<BOXM2_ALPHA>* alpha_data_;
  boxm2_data<APM_TYPE>* mog_data_;
  boxm2_stream_cache_sptr str_cache_;
  boxm2_block_id id_;
  float alpha_min_;
  float alpha_max_;
  bsta_sigma_normalizer_sptr n_table_;
};


#endif // boxm2_batch_functors_h_
