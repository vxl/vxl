#ifndef boxm2_batch_functors_h_
#define boxm2_batch_functors_h_
//:
// \file

#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <boxm2/cpp/algo/boxm2_gauss_grey_processor.h>
#include <bsta/algo/bsta_sigma_normalizer.h>
#include <boxm2/io/boxm2_stream_cache.h>

//: accumulate seg_lengths and intensities over all rays that pass through a cell to compute normalized intensity later
class boxm2_batch_update_pass0_functor
{
 public:
  //: "default" constructor (does nothing)
  boxm2_batch_update_pass0_functor() {}

  bool init_data(vcl_vector<boxm2_data_base*> & datas, vil_image_view<float> * input_img)
  {
    aux0_data_=new boxm2_data<BOXM2_AUX0>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    aux1_data_=new boxm2_data<BOXM2_AUX1>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    aux2_data_=new boxm2_data<BOXM2_NUM_OBS_SINGLE>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());
    input_img_=input_img;
    return true;
  }
  inline bool step_cell(float seg_len,int index,unsigned i,unsigned j)
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
  boxm2_batch_update_pass1_functor() {}

  bool init_data(vcl_vector<boxm2_data_base*> & datas, vil_image_view<float>* pre_img, vil_image_view<float>* vis_img)
  {
    aux0_data_=new boxm2_data<BOXM2_AUX0>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    aux1_data_=new boxm2_data<BOXM2_AUX1>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());
    //mog3_data_=new boxm2_data<BOXM2_MOG3_GREY>(datas[3]->data_buffer(),datas[3]->buffer_length(),datas[3]->block_id());
    mog3_data_=new boxm2_data<APM_TYPE>(datas[3]->data_buffer(),datas[3]->buffer_length(),datas[3]->block_id());

    alpha_integral_.set_size(pre_img->ni(), pre_img->nj(),1);
    alpha_integral_.fill(0.0f);
    pre_img_ = pre_img;
    vis_img_ = vis_img;
    pre_img_->fill(0.0f);
    vis_img_->fill(1.0f);

    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i,unsigned j)
  {
    boxm2_data<BOXM2_AUX0>::datatype & aux0 = aux0_data_->data()[index];
    boxm2_data<BOXM2_AUX1>::datatype & aux1 = aux1_data_->data()[index];

    // compute average intensity for the cell
    float mean_obs =aux0/aux1;

    // compute appearance probability of observation
    //float PI=boxm2_data_traits<BOXM2_MOG3_GREY>::processor::prob_density(mog3_data_->data()[index], mean_obs);
    float PI=boxm2_data_traits<APM_TYPE>::processor::prob_density(mog3_data_->data()[index], mean_obs);

    float vis=(*vis_img_)(i,j);
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];

    // update alpha integral
    alpha_integral_(i,j) += alpha * seg_len;
    // compute new visibility probability with updated alpha_integral
    const float vis_prob_end = vcl_exp(-alpha_integral_(i,j));
    // compute weight for this cell
    const float Omega = vis - vis_prob_end;
    // and update pre
    (*pre_img_)(i,j) +=  PI * Omega;
    // update visibility probability
    (*vis_img_)(i,j) = vis_prob_end;

    return true;
  }
 private:
  boxm2_data<BOXM2_AUX0> * aux0_data_;
  boxm2_data<BOXM2_AUX1> * aux1_data_;
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  //boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
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
  boxm2_batch_update_pass2_functor() {}

  bool init_data(vcl_vector<boxm2_data_base*> & datas, vil_image_view<float> * pre_inf)
  {
    aux0_data_=new boxm2_data<BOXM2_AUX0>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    aux1_data_=new boxm2_data<BOXM2_AUX1>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());
    //mog3_data_=new boxm2_data<BOXM2_MOG3_GREY>(datas[3]->data_buffer(),datas[3]->buffer_length(),datas[3]->block_id());
    mog3_data_=new boxm2_data<APM_TYPE>(datas[3]->data_buffer(),datas[3]->buffer_length(),datas[3]->block_id());
    aux_data_ = new boxm2_data<BOXM2_AUX>(datas[4]->data_buffer(),datas[4]->buffer_length(),datas[4]->block_id());

    alpha_integral_.set_size(pre_inf->ni(), pre_inf->nj(),1);
    alpha_integral_.fill(0.0f);
    pre_img_.set_size(pre_inf->ni(), pre_inf->nj(),1);
    pre_img_.fill(0.0f);
    vis_img_.set_size(pre_inf->ni(), pre_inf->nj(),1);
    vis_img_.fill(1.0f);
    pre_inf_ = pre_inf;

    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i,unsigned j)
  {
    boxm2_data<BOXM2_AUX0>::datatype & aux0 = aux0_data_->data()[index];
    boxm2_data<BOXM2_AUX1>::datatype & aux1 = aux1_data_->data()[index];

    // compute average intensity for the cell
    float mean_obs =aux0/aux1;

    // compute appearance probability of observation
    //float PI=boxm2_data_traits<BOXM2_MOG3_GREY>::processor::prob_density(mog3_data_->data()[index], mean_obs);
    float PI=boxm2_data_traits<APM_TYPE>::processor::prob_density(mog3_data_->data()[index], mean_obs);

    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    // update alpha integral
    alpha_integral_(i,j) += alpha * seg_len;

    // compute new visibility probability with updated alpha_integral
    float vis_prob_end = vcl_exp(-alpha_integral_(i,j));

    // grab this cell's pre and vis value
    float pre = pre_img_(i,j);
    float vis = vis_img_(i,j);

    // compute weight for this cell
    float Omega = vis - vis_prob_end;

    float cell_value = PI*Omega;
    float post = (*pre_inf_)(i,j)-pre-cell_value;
    //: now correct post
    post /= vcl_exp(-alpha*seg_len);

    // update vis and pre
    pre_img_(i,j) +=  cell_value;
    vis_img_(i,j) = vis_prob_end;

    boxm2_data<BOXM2_AUX>::datatype & aux = aux_data_->data()[index];
    // accumulate aux sample values
    aux[0] += pre * seg_len;
    aux[1] += vis * seg_len;
    aux[2] += post * seg_len;

    return true;
  }
 private:
  boxm2_data<BOXM2_AUX0> * aux0_data_;
  boxm2_data<BOXM2_AUX1> * aux1_data_;
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  //boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
  boxm2_data<APM_TYPE> * mog3_data_;
  boxm2_data<BOXM2_AUX> * aux_data_;

  vil_image_view<float> alpha_integral_;
  vil_image_view<float> pre_img_;  // these 2 can be local for this functor
  vil_image_view<float> vis_img_;
  vil_image_view<float> * pre_inf_;
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
  boxm2_batch_update_functor() {}

  bool init_data(boxm2_data_base *alph, boxm2_data_base *mog, boxm2_stream_cache_sptr str_cache, bsta_sigma_normalizer_sptr n_table)
  {
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(alph->data_buffer(),alph->buffer_length(),alph->block_id());
    //mog3_data_ = new boxm2_data<BOXM2_MOG3_GREY>(mog->data_buffer(), mog->buffer_length(), mog->block_id());
    mog_data_ = new boxm2_data<APM_TYPE>(mog->data_buffer(), mog->buffer_length(), mog->block_id());
    str_cache_ = str_cache;
    id_ = alph->block_id();
    n_table_ = n_table;
    return true;
  }

  inline bool process_cell(int index)
  {
    boxm2_data<BOXM2_ALPHA>::datatype & alpha=alpha_data_->data()[index];
    //boxm2_data<BOXM2_MOG3_GREY>::datatype & mog3=mog3_data_->data()[index];
    typename boxm2_data<APM_TYPE>::datatype & mog=mog_data_->data()[index];

    
    vcl_vector<aux0_datatype> out0 = str_cache_->get_next<BOXM2_AUX0>(id_, index);
    vcl_vector<aux1_datatype> out1 = str_cache_->get_next<BOXM2_AUX1>(id_, index);
    vcl_vector<aux_datatype> out = str_cache_->get_next<BOXM2_AUX>(id_, index);
    vcl_vector<nrays_datatype> nrays = str_cache_->get_next<BOXM2_NUM_OBS_SINGLE>(id_, index);

    int cell_no = 2000000;
    
    vcl_vector<aux0_datatype> obs;
    vcl_vector<float> vis;
    vcl_vector<float> pre;
    float term1 = 1.0f;  // product of the likelihoods that cell is a surface
    float term2 = 1.0f;  // product of the likelihoods that cell is not a surface
    unsigned nimgs = (unsigned)out0.size();

    float max_obs_seg_len = 0.0f;  // max gives the best idea about the size of the cell
    for (unsigned m = 0; m < nimgs; m++) {
      float obs_seg_len = out1[m];
      float mean_obs = out0[m]/obs_seg_len;
      obs.push_back(mean_obs);

      max_obs_seg_len = max_obs_seg_len > obs_seg_len/nrays[m] ? max_obs_seg_len : obs_seg_len/nrays[m];

      //float PI = boxm2_data_traits<BOXM2_MOG3_GREY>::processor::prob_density(mog3, mean_obs);
      float PI = boxm2_data_traits<APM_TYPE>::processor::prob_density(mog, mean_obs);
      float pre_i = out[m][0]/obs_seg_len; // mean pre
      pre.push_back(pre_i);
      float vis_i = out[m][1]/obs_seg_len; // mean vis
      vis.push_back(vis_i);
      float post_i = out[m][2]/obs_seg_len; // mean post

      term1 *= pre_i + vis_i*PI;
      term2 *= pre_i + post_i;    // no infinity term for now

      if (index == cell_no) {
        vcl_cout << "\t m: " << m << " pre_i: " << pre_i << " vis_i: " << vis_i << " post_i: " << post_i << vcl_endl;
        vcl_cout << "obs_seg_len: " << obs_seg_len << " PI: " << PI << " mean_obs: " << mean_obs << "\n";
        vcl_cout << "current term1: " << term1 << " term2: " << term2 << "\n";
      }
    }
    float p_q = 1.0f-vcl_exp(-alpha*max_obs_seg_len);
    if (index == cell_no) {
      vcl_cout << "current alpha: " << alpha << " p_q: " << p_q << "\n";
    }

    //: compute new alpha value
    float p_q_new = p_q*term1 / (p_q*term1 + (1.0f-p_q)*term2);
    alpha = alpha*p_q_new;
    //alpha = -vcl_log(1.0f-p_q_new)/max_obs_seg_len;

    if (index == cell_no) {
      vcl_cout << "after update alpha: " << alpha << "\n";
    }

    float alpha_min = -vcl_log(1.f-0.0001f)/max_obs_seg_len;
    float alpha_max = -vcl_log(1.f-0.995f)/max_obs_seg_len;

    if (alpha < alpha_min)
      alpha = alpha_min;

    //float max_cell_P = 0.995f;
    //float max_alpha = -vcl_log(1.0f - max_cell_P)/mean_len;
    //if (alpha > max_alpha)
    //  alpha = max_alpha;
    if (alpha > alpha_max)
     alpha = alpha_max;
     
    if (index == cell_no) {
      vcl_cout << " alpha_min: " << alpha_min << " alpha_max: " << alpha_max << vcl_endl;
      vcl_cout << " p_q_new: " << p_q_new << vcl_endl;
      vcl_cout << "term1: " << term1 << " term2: " << term2 << vcl_endl;
    }

    //: compute new appearance model
    //boxm2_data_traits<BOXM2_MOG3_GREY>::processor::compute_gauss_mixture_3(mog3,obs, vis,n_table_,0.03f);
    boxm2_data_traits<APM_TYPE>::processor::compute_app_model(mog,obs, vis,n_table_,0.03f);
    //boxm2_data_traits<APM_TYPE>::processor::compute_app_model(mog,obs, pre,vis,n_table_,0.03f);
    
    return true;
  }

 private:
  boxm2_data<BOXM2_ALPHA>* alpha_data_;
  //boxm2_data<BOXM2_MOG3_GREY>* mog_data_;
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
  typedef boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::datatype nrays_datatype;
  typedef boxm2_data_traits<BOXM2_AUX>::datatype aux_datatype;

  //: "default" constructor
  boxm2_batch_update_app_functor() {}

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
    
    vcl_vector<aux0_datatype> out0 = str_cache_->get_next<BOXM2_AUX0>(id_, index);
    vcl_vector<aux1_datatype> out1 = str_cache_->get_next<BOXM2_AUX1>(id_, index);
    vcl_vector<aux_datatype> out = str_cache_->get_next<BOXM2_AUX>(id_, index);
    vcl_vector<nrays_datatype> nrays = str_cache_->get_next<BOXM2_NUM_OBS_SINGLE>(id_, index);

    int cell_no = 2000000;
    
    vcl_vector<aux0_datatype> obs;
    vcl_vector<float> vis;
    vcl_vector<float> pre;
    float term1 = 1.0f;  // product of the likelihoods that cell is a surface
    float term2 = 1.0f;  // product of the likelihoods that cell is not a surface
    unsigned nimgs = (unsigned)out0.size();

    float max_obs_seg_len = 0.0f;  // max gives the best idea about the size of the cell
    for (unsigned m = 0; m < nimgs; m++) {
      float obs_seg_len = out1[m];
      float mean_obs = out0[m]/obs_seg_len;
      obs.push_back(mean_obs);  

      max_obs_seg_len = max_obs_seg_len > obs_seg_len/nrays[m] ? max_obs_seg_len : obs_seg_len/nrays[m];

      float pre_i = out[m][0]/obs_seg_len; // mean pre
      pre.push_back(pre_i);
      float vis_i = out[m][1]/obs_seg_len; // mean vis
      vis.push_back(vis_i);
    
      if (index == cell_no) {
        vcl_cout << "\t m: " << m << " pre_i: " << pre_i << " vis_i: " << vis_i << vcl_endl;
        vcl_cout << "obs_seg_len: " << obs_seg_len << " mean_obs: " << mean_obs << "\n";
      }
    }
    
    //: compute new appearance model
    //boxm2_data_traits<BOXM2_MOG3_GREY>::processor::compute_gauss_mixture_3(mog3,obs, vis,n_table_,0.03f);
    boxm2_data_traits<APM_TYPE>::processor::compute_app_model(mog,obs, vis,n_table_,0.03f);
    //boxm2_data_traits<APM_TYPE>::processor::compute_app_model(mog,obs, pre,vis,n_table_,0.03f);
    
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
  boxm2_batch_update_alpha_functor() {}

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
    
    vcl_vector<aux0_datatype> out0 = str_cache_->get_next<BOXM2_AUX0>(id_, index);
    vcl_vector<aux1_datatype> out1 = str_cache_->get_next<BOXM2_AUX1>(id_, index);
    vcl_vector<aux_datatype> out = str_cache_->get_next<BOXM2_AUX>(id_, index);
    vcl_vector<nrays_datatype> nrays = str_cache_->get_next<BOXM2_NUM_OBS_SINGLE>(id_, index);

    int cell_no = 2000000;
    
    vcl_vector<aux0_datatype> obs;
    vcl_vector<float> vis;
    vcl_vector<float> pre;
    float term1 = 1.0f;  // product of the likelihoods that cell is a surface
    float term2 = 1.0f;  // product of the likelihoods that cell is not a surface
    unsigned nimgs = (unsigned)out0.size();

    float max_obs_seg_len = 0.0f;  // max gives the best idea about the size of the cell
    for (unsigned m = 0; m < nimgs; m++) {
      float obs_seg_len = out1[m];
      float mean_obs = out0[m]/obs_seg_len;
      obs.push_back(mean_obs);  

      max_obs_seg_len = max_obs_seg_len > obs_seg_len/nrays[m] ? max_obs_seg_len : obs_seg_len/nrays[m];

      float PI = boxm2_data_traits<APM_TYPE>::processor::prob_density(mog, mean_obs);
      float pre_i = out[m][0]/obs_seg_len; // mean pre
      pre.push_back(pre_i);
      float vis_i = out[m][1]/obs_seg_len; // mean vis
      vis.push_back(vis_i);
      float post_i = out[m][2]/obs_seg_len; // mean post
   
      term1 *= pre_i + vis_i*PI;
      term2 *= pre_i + post_i;    // no infinity term for now

      if (index == cell_no) {
        vcl_cout << "\t m: " << m << " pre_i: " << pre_i << " vis_i: " << vis_i << " post_i: " << post_i << vcl_endl;
        vcl_cout << "obs_seg_len: " << obs_seg_len << " PI: " << PI << " mean_obs: " << mean_obs << "\n";
        vcl_cout << "current term1: " << term1 << " term2: " << term2 << "\n";
      }
    }
    float p_q = 1.0f-vcl_exp(-alpha*max_obs_seg_len);
    if (index == cell_no) {
      vcl_cout << "current alpha: " << alpha << " p_q: " << p_q << "\n";
    }

    //: compute new alpha value
    float p_q_new = p_q*term1 / (p_q*term1 + (1.0f-p_q)*term2);
    //alpha = alpha*p_q_new;
    alpha = -vcl_log(1.0f-p_q_new)/max_obs_seg_len;

    if (index == cell_no) {
      vcl_cout << "after update alpha: " << alpha << "\n";
    }

    float alpha_min = -vcl_log(1.f-0.0001f)/max_obs_seg_len;
    float alpha_max = -vcl_log(1.f-0.995f)/max_obs_seg_len;

    if (alpha < alpha_min)
      alpha = alpha_min;

    //float max_cell_P = 0.995f;
    //float max_alpha = -vcl_log(1.0f - max_cell_P)/mean_len;
    //if (alpha > max_alpha)
    //  alpha = max_alpha;
    if (alpha > alpha_max)
     alpha = alpha_max;
     
    if (index == cell_no) {
      vcl_cout << " alpha_min: " << alpha_min << " alpha_max: " << alpha_max << vcl_endl;
      vcl_cout << " p_q_new: " << p_q_new << vcl_endl;
      vcl_cout << "term1: " << term1 << " term2: " << term2 << vcl_endl;
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
