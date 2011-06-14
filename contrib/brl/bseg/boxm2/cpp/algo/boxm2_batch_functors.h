#ifndef boxm2_batch_functors_h_
#define boxm2_batch_functors_h_
//:
// \file

#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <bsta/algo/bsta_sigma_normalizer.h>

//: accumulate seg_lens and intensities over all rays that pass through a cell to compute normalized intensity later
class boxm2_batch_update_pass0_functor
{
 public:
  //: "default" constructor
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
    mog3_data_=new boxm2_data<BOXM2_MOG3_GREY>(datas[3]->data_buffer(),datas[3]->buffer_length(),datas[3]->block_id());

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
    float PI=boxm2_data_traits<BOXM2_MOG3_GREY>::processor::prob_density(mog3_data_->data()[index], mean_obs);
    
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
  boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
    
  vil_image_view<float> alpha_integral_;
  vil_image_view<float> * pre_img_;
  vil_image_view<float> * vis_img_;

};

//: compute average pre_i, vis_i and post_i for each cell, save the values in aux
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
    mog3_data_=new boxm2_data<BOXM2_MOG3_GREY>(datas[3]->data_buffer(),datas[3]->buffer_length(),datas[3]->block_id());
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
    float PI=boxm2_data_traits<BOXM2_MOG3_GREY>::processor::prob_density(mog3_data_->data()[index], mean_obs);

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
  boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
  boxm2_data<BOXM2_AUX> * aux_data_;
    
  vil_image_view<float> alpha_integral_;
  vil_image_view<float> pre_img_;  // these 2 can be local for this functor
  vil_image_view<float> vis_img_;
  vil_image_view<float> * pre_inf_;

};

class boxm2_batch_update_functor
{
 public:
  typedef boxm2_data_traits<BOXM2_AUX0>::datatype aux0_datatype;
  typedef boxm2_data_traits<BOXM2_AUX1>::datatype aux1_datatype;
  typedef boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::datatype nrays_datatype;
  typedef boxm2_data_traits<BOXM2_AUX>::datatype aux_datatype;

  //: "default" constructor
  boxm2_batch_update_functor() {}

  bool init_data(boxm2_data_base *alph, boxm2_data_base *mog, boxm2_stream_cache_sptr str_cache, bsta_sigma_normalizer_sptr n_table, float block_len, int max_levels)
  {
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(alph->data_buffer(),alph->buffer_length(),alph->block_id());
    mog3_data_ = new boxm2_data<BOXM2_MOG3_GREY>(mog->data_buffer(), mog->buffer_length(), mog->block_id());
    str_cache_ = str_cache;
    id_ = alph->block_id();
    alpha_min_ = -vcl_log(1.f-0.0001f)/float(block_len/max_levels);
    n_table_ = n_table;
    return true;
  }

  inline bool process_cell(int index)
  {
    boxm2_data<BOXM2_ALPHA>::datatype & alpha=alpha_data_->data()[index];
    boxm2_data<BOXM2_MOG3_GREY>::datatype & mog3=mog3_data_->data()[index];

    vcl_vector<aux0_datatype> out0 = str_cache_->get_next<BOXM2_AUX0,aux0_datatype>(id_, index);
    vcl_vector<aux1_datatype> out1 = str_cache_->get_next<BOXM2_AUX1,aux1_datatype>(id_, index);
    vcl_vector<aux_datatype> out = str_cache_->get_next<BOXM2_AUX,aux_datatype>(id_, index);
    vcl_vector<nrays_datatype> nrays = str_cache_->get_next<BOXM2_NUM_OBS_SINGLE, nrays_datatype>(id_, index);

    vcl_vector<aux0_datatype> obs;
    vcl_vector<float> vis;
    float term1 = 1.0f;  // product of the likelihoods that cell is a surface
    float term2 = 1.0f;  // product of the likelihoods that cell is not a surface
    unsigned nimgs = (unsigned)out0.size();
    for (unsigned m = 0; m < nimgs; m++) {
      float mean_obs = out0[m]/out1[m];
      obs.push_back(mean_obs);  
      float PI = boxm2_data_traits<BOXM2_MOG3_GREY>::processor::prob_density(mog3, mean_obs);
      float pre_i = out[m][0]/out1[m]; // mean pre
      float vis_i = out[m][1]/out1[m]; // mean vis
      vis.push_back(vis_i);
      float post_i = out[m][2]/out1[m]; // mean post
   
      term1 *= pre_i + vis_i*PI;
      term2 *= pre_i + post_i;    // no infinity term for now
    }
    //: find the mean length of all the rays that pierced through this cell from all the images
    float sum_len = 0.0f;
    for (unsigned m = 0; m < nimgs; m++)
      sum_len += out1[m]/nrays[m];
    float mean_len = sum_len/nimgs;
    
    //: compute new alpha value
    float p_q = 1.0f-vcl_exp(-alpha*mean_len);
    float p_q_new = p_q*term1 / (p_q*term1 + (1.0f-p_q)*term2);
    alpha = -vcl_log(1.0f-p_q_new)/mean_len;
    if (alpha < alpha_min_)
      alpha = alpha_min_;

    //: compute new appearance model
    boxm2_data_traits<BOXM2_MOG3_GREY>::processor::compute_gauss_mixture_3(mog3,obs, vis,n_table_,0.03f);

    return true;
  }

 private:
  boxm2_data<BOXM2_ALPHA>* alpha_data_;
  boxm2_data<BOXM2_MOG3_GREY>* mog3_data_;
  boxm2_stream_cache_sptr str_cache_;
  boxm2_block_id id_;
  float alpha_min_;
  bsta_sigma_normalizer_sptr n_table_;
};




#endif // boxm2_batch_functors_h_
