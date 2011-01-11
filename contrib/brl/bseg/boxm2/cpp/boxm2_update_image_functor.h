#ifndef boxm2_update_image_functor_h
#define boxm2_update_image_functor_h
//:
// \file
#include <boxm2/cpp/boxm2_cast_ray_function.h>
#include <boxm2/cpp/boxm2_mog3_grey_processor.h>

class boxm2_update_pass0_functor
{
 public:
  //: "default" constructor
  boxm2_update_pass0_functor() {}

  bool init_data(vcl_vector<boxm2_data_base*> & datas, vil_image_view<float> * input_img)
  {
      aux_data_=new boxm2_data<BOXM2_AUX>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
      input_img_=input_img;
      return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i,unsigned j)
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
class boxm2_update_pass1_functor
{
 public:
  //: "default" constructor
  boxm2_update_pass1_functor() {}

  bool init_data(vcl_vector<boxm2_data_base*> & datas, vil_image_view<float> * pre_img,vil_image_view<float> * vis_img)
  {
      aux_data_=new boxm2_data<BOXM2_AUX>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
      alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
      mog3_data_=new boxm2_data<BOXM2_MOG3_GREY>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());
      pre_img_=pre_img;
      vis_img_=vis_img;
      return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i,unsigned j)
  {
    boxm2_data<BOXM2_AUX>::datatype aux=aux_data_->data()[index];
    if (aux[0]<1e-10f)return true;

    float mean_obs=aux[1]/aux[0];
    float PI=boxm2_data_traits<BOXM2_MOG3_GREY>::processor::prob_density(mog3_data_->data()[index], mean_obs);
    float vis=(*pre_img_)(i,j);
    float pre=(*vis_img_)(i,j);
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    float omega=1-vcl_exp(-seg_len*alpha);
    pre+=vis*omega*PI;
    vis=vis*(1-omega);
    (*pre_img_)(i,j)=pre;
    (*vis_img_)(i,j)=vis;
    return true;
  }
 private:
  boxm2_data<BOXM2_AUX> * aux_data_;
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
  vil_image_view<float> * pre_img_;
  vil_image_view<float> * vis_img_;
};


class boxm2_update_pass2_functor
{
 public:
  //: "default" constructor
  boxm2_update_pass2_functor() {}

  bool init_data(vcl_vector<boxm2_data_base*> & datas, 
                 vil_image_view<float> * pre_img,vil_image_view<float> * vis_img,
                 vil_image_view<float> * norm_img)
  {
      aux_data_=new boxm2_data<BOXM2_AUX>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
      alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
      mog3_data_=new boxm2_data<BOXM2_MOG3_GREY>(datas[2]->data_buffer(),datas[2]->buffer_length(),datas[2]->block_id());
      pre_img_=pre_img;
      vis_img_=vis_img;
      norm_img_=norm_img;
      return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i,unsigned j)
  {
      boxm2_data<BOXM2_AUX>::datatype aux=aux_data_->data()[index];
      if (aux[0]<1e-10f)return true;
      float mean_obs=aux[1]/aux[0];
      float PI=boxm2_data_traits<BOXM2_MOG3_GREY>::processor::prob_density(mog3_data_->data()[index], mean_obs);
      float vis=(*vis_img_)(i,j);
      float pre=(*pre_img_)(i,j);
      boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
      float omega=(1-vcl_exp(-seg_len*alpha));
      aux[2]+=(pre+vis*PI)/(*norm_img_)(i,j)*seg_len;
      aux[3]+=vis*seg_len;
      vcl_cout<<" beta : "<<aux[2]<<","<<seg_len;
      pre+=vis*omega*PI;
      vis=vis*(1-omega);
      (*vis_img_)(i,j)=vis;
      (*pre_img_)(i,j)=pre;
      return true;
  }
 private:
  boxm2_data<BOXM2_AUX> * aux_data_;
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
  vil_image_view<float> * pre_img_;
  vil_image_view<float> * vis_img_;
  vil_image_view<float> * norm_img_;
};




#endif
