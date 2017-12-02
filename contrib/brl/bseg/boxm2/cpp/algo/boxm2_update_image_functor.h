#ifndef boxm2_update_image_functor_h
#define boxm2_update_image_functor_h
//:
// \file
#include <vector>
#include <limits>
#include <iostream>
#include <cmath>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <boxm2/cpp/algo/boxm2_gauss_grey_processor.h>
#include <vil/vil_image_view.h>
#include <vcl_compiler.h>

class boxm2_update_pass0_functor
{
 public:
  //: "default" constructor
  boxm2_update_pass0_functor() {}

  bool init_data(std::vector<boxm2_data_base*> & datas, vil_image_view<float> * input_img)
  {
    aux_data_=new boxm2_data<BOXM2_AUX>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    input_img_=input_img;
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i,unsigned j, float abs_depth=0.0f)
  {
    boxm2_data<BOXM2_AUX>::datatype & aux=aux_data_->data()[index];
    aux[0]+=seg_len;
    aux[1]+=seg_len*(*input_img_)(i,j);

    return true;
  }
 private:
  boxm2_data<BOXM2_AUX> * aux_data_;
  vil_image_view<float> * input_img_;
};

template <boxm2_data_type APM_TYPE>
class boxm2_update_pass1_functor
{
 public:
  //: "default" constructor
  boxm2_update_pass1_functor() {}

  bool init_data(std::vector<boxm2_data_base*> & datas, vil_image_view<float> * pre_img,vil_image_view<float> * vis_img)
  {
    aux_data_=new boxm2_data<BOXM2_AUX>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    mog3_data_=new boxm2_data<APM_TYPE>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());
    pre_img_=pre_img;
    vis_img_=vis_img;
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i,unsigned j, float abs_depth=0.0f)
  {
    typename boxm2_data<BOXM2_AUX>::datatype aux=aux_data_->data()[index];
    if (aux[0]<1e-10f)return true;

    float mean_obs=aux[1]/aux[0];
    float PI=boxm2_processor_type<APM_TYPE>::type::prob_density(mog3_data_->data()[index], mean_obs);

    float vis=(*vis_img_)(i,j);
    float pre=(*pre_img_)(i,j);
    typename boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    float temp=std::exp(-seg_len*alpha);
    pre+=vis*(1-temp)*PI;
    vis*=temp;
    (*pre_img_)(i,j)=pre;
    (*vis_img_)(i,j)=vis;
    return true;
  }
 private:
  boxm2_data<BOXM2_AUX> * aux_data_;
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  boxm2_data<APM_TYPE> * mog3_data_;
  vil_image_view<float> * pre_img_;
  vil_image_view<float> * vis_img_;
};

template <boxm2_data_type APM_TYPE>
class boxm2_update_pass2_functor
{
 public:
  //: "default" constructor
  boxm2_update_pass2_functor() {}

  bool init_data(std::vector<boxm2_data_base*> & datas,
                 vil_image_view<float> * pre_img,vil_image_view<float> * vis_img,
                 vil_image_view<float> * norm_img)
  {
    aux_data_=new boxm2_data<BOXM2_AUX>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    mog3_data_=new boxm2_data<APM_TYPE>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());
    pre_img_=pre_img;
    vis_img_=vis_img;
    norm_img_=norm_img;
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i,unsigned j, float abs_depth=0.0f)
  {
    typename boxm2_data<BOXM2_AUX>::datatype & aux=aux_data_->data()[index];
    if (aux[0]<1e-10f)return true;
    float mean_obs=aux[1]/aux[0];
    float PI=boxm2_processor_type<APM_TYPE>::type::prob_density(mog3_data_->data()[index], mean_obs);

    //if (PI==std::numeric_limits<float>::infinity())
    float vis=(*vis_img_)(i,j);
    float pre=(*pre_img_)(i,j);
    typename boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    float omega=(1-std::exp(-seg_len*alpha));
    if ((*norm_img_)(i,j)>1e-10f)
    {
        aux[2]+=((pre+vis*PI)/((*norm_img_)(i,j))*seg_len);
        aux[3]+=vis*seg_len;
    }
    pre+=vis*omega*PI;
    vis=vis*(1-omega);
    (*vis_img_)(i,j)=vis;
    (*pre_img_)(i,j)=pre;
    return true;
  }
 private:
  boxm2_data<BOXM2_AUX> * aux_data_;
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  boxm2_data<APM_TYPE> * mog3_data_;
  vil_image_view<float> * pre_img_;
  vil_image_view<float> * vis_img_;
  vil_image_view<float> * norm_img_;
};

template <boxm2_data_type APM_TYPE>
class boxm2_update_data_functor
{
 public:
  //: "default" constructor
  boxm2_update_data_functor() {}

  bool init_data(std::vector<boxm2_data_base*> & datas, float block_len, int max_levels)
  {
    aux_data_=new boxm2_data<BOXM2_AUX>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    mog3_data_=new boxm2_data<APM_TYPE>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());
    nobs_data_=new boxm2_data<BOXM2_NUM_OBS>(datas[3]->data_buffer(),datas[3]->buffer_length(),datas[3]->block_id());
    alpha_min_ = -std::log(1.f-0.0001f)/float(block_len/max_levels);

    return true;
  }
  inline bool process_cell(int index)
  {
    typename boxm2_data<BOXM2_AUX>::datatype & aux=aux_data_->data()[index];

    if (aux[0]>1e-8f)
    {
      float beta=aux[2]/aux[0];
      float vis =aux[3]/aux[0];
      float mean_obs=aux[1]/aux[0];

      typename boxm2_data<BOXM2_ALPHA>::datatype & alpha=alpha_data_->data()[index];
      alpha=std::max(alpha_min_,alpha*beta);
      typename boxm2_data<APM_TYPE>::datatype & mog3=mog3_data_->data()[index];
      typename boxm2_data<BOXM2_NUM_OBS>::datatype & nobs=nobs_data_->data()[index];
      vnl_vector_fixed<float,4> nobs_float;
      nobs_float[0]=(float)nobs[0];
      nobs_float[1]=(float)nobs[1];
      nobs_float[2]=(float)nobs[2];
      //: converting flot to short
      nobs_float[3]=((float)nobs[3])/100.0f;
      boxm2_processor_type<APM_TYPE>::type::update_app_model(mog3,nobs_float, mean_obs,vis,0.09f, 0.03f);
      nobs[0]=(unsigned short)nobs_float[0];
      nobs[1]=(unsigned short)nobs_float[1];
      nobs[2]=(unsigned short)nobs_float[2];
      nobs[3]=(unsigned short)(nobs_float[3]*100.0f);
      aux[0]=0.0;
      aux[1]=0.0;
      aux[2]=0.0;
      aux[3]=0.0;
    }
    return true;
  }
 private:
  boxm2_data<BOXM2_AUX>       * aux_data_;
  boxm2_data<BOXM2_ALPHA>     * alpha_data_;
  boxm2_data<APM_TYPE> * mog3_data_;
  boxm2_data<BOXM2_NUM_OBS>   * nobs_data_;
  float alpha_min_;
};

#endif
