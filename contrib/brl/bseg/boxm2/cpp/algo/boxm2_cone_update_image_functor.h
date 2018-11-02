#ifndef boxm2_cone_update_image_functor_h
#define boxm2_cone_update_image_functor_h
//:
// \file
#include <vector>
#include <limits>
#include <iostream>
#include <cmath>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class boxm2_cone_update_pass0_functor
{
 public:
  //: "default" constructor
  boxm2_cone_update_pass0_functor() = default;

  bool init_data(std::vector<boxm2_data_base*> & datas,
                 vil_image_view<float> * pre_img,
                 vil_image_view<float> * vis_img,
                 vil_image_view<float> * input_img)
  {
    aux_data_=new boxm2_data<BOXM2_AUX>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    alpha_data_=new boxm2_data<BOXM2_GAMMA>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    mog3_data_=new boxm2_data<BOXM2_MOG3_GREY>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());
    pre_img_=pre_img;
    vis_img_=vis_img;
    PI_cum=0.0;
    vol_cum=0.0;
    vis_cum=1.0;

    input_img_=input_img;
    return true;
  }

  inline bool step_cell(float ray_vol,int index,unsigned i,unsigned j)
  {
    boxm2_data<BOXM2_AUX>::datatype & aux=aux_data_->data()[index];
    aux[0]+=ray_vol;
    aux[1]+=ray_vol*(*input_img_)(i,j);
    float PI=boxm2_processor_type<BOXM2_MOG3_GREY>::type::prob_density(mog3_data_->data()[index],(*input_img_)(i,j));
    boxm2_data<BOXM2_GAMMA>::datatype gamma=alpha_data_->data()[index];
    float temp=std::exp(-ray_vol*gamma);
    PI_cum+=PI*ray_vol;
    vol_cum+=ray_vol;
    vis_cum*=temp;
    return true;
  }

  inline bool compute_ball_properties(unsigned i,unsigned j)
  {
          float vis=(*vis_img_)(i,j);
    float pre=(*pre_img_)(i,j);

    float PI=0.0;
    if (vol_cum>1e-12f) PI=PI_cum/vol_cum;

    pre+=vis*(1-vis_cum)*PI;
    vis*=vis_cum;

    (*pre_img_)(i,j)=pre;
    (*vis_img_)(i,j)=vis;

    vis_cum=1.0;
    PI_cum=0.0f;
    vol_cum=0.0f;
    return true;
  }

  inline bool redistribute(float vol, int index){return true;}

 private:
  boxm2_data<BOXM2_AUX> * aux_data_;
  vil_image_view<float> * input_img_;
  boxm2_data<BOXM2_GAMMA> * alpha_data_;
  boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
  vil_image_view<float> * pre_img_;
  vil_image_view<float> * vis_img_;
  float PI_cum;
  float vol_cum;
  float vis_cum;
};

class boxm2_cone_update_pass1_functor
{
 public:
  //: "default" constructor
  boxm2_cone_update_pass1_functor() = default;

  bool init_data(std::vector<boxm2_data_base*> & datas,
                 vil_image_view<float> * pre_img,
                 vil_image_view<float> * vis_img,
                 vil_image_view<float> * input_img)
  {
    aux_data_=new boxm2_data<BOXM2_AUX>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    alpha_data_=new boxm2_data<BOXM2_GAMMA>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    mog3_data_=new boxm2_data<BOXM2_MOG3_GREY>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());
    pre_img_=pre_img;
    vis_img_=vis_img;
    input_img_=input_img;
    PI_cum=0.0;
    vol_cum=0.0;
    vis_cum=1.0;
    return true;
  }

  inline bool step_cell(float ray_vol,int index,unsigned i,unsigned j)
  {
    boxm2_data<BOXM2_AUX>::datatype & aux=aux_data_->data()[index];
    aux[0]+=ray_vol;
    aux[1]+=ray_vol*(*input_img_)(i,j);

    float PI=boxm2_processor_type<BOXM2_MOG3_GREY>::type::prob_density(mog3_data_->data()[index],(*input_img_)(i,j));
    boxm2_data<BOXM2_GAMMA>::datatype gamma=alpha_data_->data()[index];
    float temp=std::exp(-ray_vol*gamma);
    PI_cum+=PI*ray_vol;
    vol_cum+=ray_vol;
    vis_cum*=temp;

    return true;
  }

  inline bool compute_ball_properties(unsigned i,unsigned j)
  {
    float vis=(*vis_img_)(i,j);
    float pre=(*pre_img_)(i,j);

    float PI=0.0;
    if (vol_cum>1e-12f) PI=PI_cum/vol_cum;

    pre+=vis*(1-vis_cum)*PI;
    vis*=vis_cum;

    (*pre_img_)(i,j)=pre;
    (*vis_img_)(i,j)=vis;

    vis_cum=1.0;
    PI_cum=0.0f;
    vol_cum=0.0f;
    return true;
  }

  inline bool redistribute(float vol, int index) {return true;}

 private:
  float PI_cum;
  float vol_cum;
  float vis_cum;
  float pre_cum;
  boxm2_data<BOXM2_AUX> * aux_data_;
  boxm2_data<BOXM2_GAMMA> * alpha_data_;
  boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
  vil_image_view<float> * pre_img_;
  vil_image_view<float> * vis_img_;
  vil_image_view<float> * input_img_;
};


class boxm2_cone_update_pass2_functor
{
 public:
  //: "default" constructor
  boxm2_cone_update_pass2_functor() = default;

  bool init_data(std::vector<boxm2_data_base*> & datas,
                 vil_image_view<float> * pre_img,
                 vil_image_view<float> * vis_img,
                 vil_image_view<float> * norm_img,
                 vil_image_view<float> * input_img)
  {
    aux_data_=new boxm2_data<BOXM2_AUX>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    alpha_data_=new boxm2_data<BOXM2_GAMMA>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    mog3_data_=new boxm2_data<BOXM2_MOG3_GREY>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());

    pre_img_=pre_img;
    vis_img_=vis_img;
    norm_img_=norm_img;
    input_img_=input_img;

    vis_cum=1.0;
    PI_cum=0.0;
    vol_cum=0.0;
    beta_=1;

    return true;
  }

  inline bool step_cell(float ray_vol,int index,unsigned i,unsigned j)
  {
    boxm2_data<BOXM2_AUX>::datatype & aux=aux_data_->data()[index];
    //if (aux[0]<1e-10f) return true;
    float PI=boxm2_processor_type<BOXM2_MOG3_GREY>::type::prob_density(mog3_data_->data()[index], (*input_img_)(i,j));
    float vis=(*vis_img_)(i,j);
    boxm2_data<BOXM2_GAMMA>::datatype gamma=alpha_data_->data()[index];
    aux[3]+=vis*ray_vol;
    float temp=std::exp(-ray_vol*gamma);
    PI_cum+=PI*ray_vol;
    vol_cum+=ray_vol;
    vis_cum*=temp;
    return true;
  }

  inline bool compute_ball_properties(unsigned i,unsigned j)
  {
    float vis=(*vis_img_)(i,j);
    float pre=(*pre_img_)(i,j);

    float PI=0.0;
    if (vol_cum>1e-12f) PI=PI_cum/vol_cum;

    beta_ = (pre+vis*PI)/(*norm_img_)(i,j);
    pre+=vis*(1-vis_cum)*PI;
    vis*=vis_cum;

    (*pre_img_)(i,j)=pre;
    (*vis_img_)(i,j)=vis;

    vis_cum=1.0;
    PI_cum=0.0f;
    vol_cum=0.0f;

    return true;
  }

  inline bool redistribute(float vol, int index)
  {
      boxm2_data<BOXM2_AUX>::datatype & aux=aux_data_->data()[index];
      aux[2]+=beta_*vol;
      return true;
  }

 private:
  float vis_cum;
  float pre_cum;

  float beta_;
  float PI_cum;
  float vol_cum;
  boxm2_data<BOXM2_AUX> * aux_data_;
  boxm2_data<BOXM2_GAMMA> * alpha_data_;
  boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
  vil_image_view<float> * input_img_;
  vil_image_view<float> * pre_img_;
  vil_image_view<float> * vis_img_;
  vil_image_view<float> * norm_img_;
};

class boxm2_cone_update_data_functor
{
 public:
  //: "default" constructor
  boxm2_cone_update_data_functor() = default;

  bool init_data(std::vector<boxm2_data_base*> & datas, float block_len, int max_levels)
  {
    aux_data_=new boxm2_data<BOXM2_AUX>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    alpha_data_=new boxm2_data<BOXM2_GAMMA>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    mog3_data_=new boxm2_data<BOXM2_MOG3_GREY>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());
    nobs_data_=new boxm2_data<BOXM2_NUM_OBS>(datas[3]->data_buffer(),datas[3]->buffer_length(),datas[3]->block_id());
    alpha_min_ = -std::log(1.f-0.0001f)/float(std::pow(block_len/max_levels,3));
    return true;
  }

  inline bool process_cell(int index)
  {
    boxm2_data<BOXM2_AUX>::datatype & aux=aux_data_->data()[index];

    if (aux[0]>1e-10f)
    {
      float beta    =aux[2]/aux[0];
      float vis     =aux[3]/aux[0];
      float mean_obs=aux[1]/aux[0];

      boxm2_data<BOXM2_GAMMA>::datatype     & alpha=alpha_data_->data()[index];
      boxm2_data<BOXM2_MOG3_GREY>::datatype & mog3 =mog3_data_->data()[index];
      boxm2_data<BOXM2_NUM_OBS>::datatype   & nobs =nobs_data_->data()[index];

      alpha=alpha*beta;

      vnl_vector_fixed<float,4> nobs_float;
      nobs_float[0]=(float)nobs[0];
      nobs_float[1]=(float)nobs[1];
      nobs_float[2]=(float)nobs[2];
      //: converting flot to short
      nobs_float[3]=((float)nobs[3])/100.0f;
      boxm2_processor_type<BOXM2_MOG3_GREY>::type::update_gauss_mixture_3(mog3,nobs_float, mean_obs,vis,0.09f, 0.03f);

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
  boxm2_data<BOXM2_GAMMA>     * alpha_data_;
  boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
  boxm2_data<BOXM2_NUM_OBS>   * nobs_data_;
  float alpha_min_;
};

#endif
