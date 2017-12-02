#ifndef boxm2_render_exp_depth_functor_h
#define boxm2_render_exp_depth_functor_h
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <vil/vil_image_view.h>

class boxm2_render_exp_depth_functor
{
 public:
  //: "default" constructor
  boxm2_render_exp_depth_functor() {}

  bool init_data(boxm2_data_base* data, vil_image_view<float> * expected, vil_image_view<float>* vis_img, vil_image_view<float> * len_img)
  {
    alpha_data_ = new boxm2_data<BOXM2_ALPHA>(data->data_buffer(),data->buffer_length(),data->block_id());
    expected_img_ = expected;
    vis_img_ = vis_img;
    len_img_ = len_img;
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i, unsigned j, float abs_depth=0.0f)
  {
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    float vis=(*vis_img_)(i,j);
    float exp_len=(*expected_img_)(i,j);
    float curr_p=(1-std::exp(-alpha*seg_len))*vis;
    float curr_len = (*len_img_)(i,j)+seg_len;
    exp_len+=curr_p*curr_len;
    (*expected_img_)(i,j)=exp_len;
    vis*=std::exp(-alpha*seg_len);
    (*vis_img_)(i,j)=vis;
    (*len_img_)(i,j)=curr_len;
    return true;
  }
 private:
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  vil_image_view<float> *expected_img_;
  vil_image_view<float> *vis_img_;
  vil_image_view<float> *len_img_;
};

#endif
