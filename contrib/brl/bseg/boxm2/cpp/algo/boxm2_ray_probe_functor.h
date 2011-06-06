#ifndef boxm2_ray_probe_functor_h
#define boxm2_ray_probe_functor_h
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <vil/vil_image_view.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>

class boxm2_ray_probe_functor
{
 public:
  //: "default" constructor
  boxm2_ray_probe_functor() {}

  bool init_data(vcl_vector<boxm2_data_base*> & datas,
                 vcl_vector<float> & seg_len,
                 vcl_vector<float> & alpha)
  {
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    seg_len_=&seg_len;
    alpha_  =&alpha;
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i, unsigned j)
  {
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    seg_len_->push_back(seg_len);
    alpha_->push_back(alpha);
    return true;
  }
 private:
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  vcl_vector<float> * seg_len_;
  vcl_vector<float> * alpha_;
};

class boxm2_ray_app_density_functor
{
 public:
  //: "default" constructor
  boxm2_ray_app_density_functor() {}

  bool init_data(vcl_vector<boxm2_data_base*> & datas,
                 vcl_vector<float> & app_density,
                 float intensity)
  {
    //alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    mog3_data_ =new boxm2_data<BOXM2_MOG3_GREY>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    app_density_=&app_density;
    intensity_  =intensity;
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i, unsigned j)
  {
    app_density_->push_back(boxm2_data_traits<BOXM2_MOG3_GREY>::processor::prob_density(mog3_data_->data()[index], intensity_));
    return true;
  }
 private:
  boxm2_data<BOXM2_MOG3_GREY> * mog3_data_;
  vcl_vector<float> * app_density_;
  float intensity_;
};

#endif
