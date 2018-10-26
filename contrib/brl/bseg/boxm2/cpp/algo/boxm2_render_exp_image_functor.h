#ifndef boxm2_render_exp_image_functor_h
#define boxm2_render_exp_image_functor_h
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <boxm2/cpp/algo/boxm2_gauss_grey_processor.h>
#include <vil/vil_image_view.h>

class boxm2_render_vis_image_functor
{
 public:
  // "default" constructor
  boxm2_render_vis_image_functor() = default;

  bool init_data(std::vector<boxm2_data_base*> & datas,  vil_image_view<float>* vis_img)
  {
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    vis_img_   =vis_img;
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i, unsigned j, float abs_depth )
  {
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    float vis=(*vis_img_)(i,j);
    vis*=std::exp(-alpha*seg_len);
    (*vis_img_)(i,j)=vis;
    return true;
  }

 private:
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  vil_image_view<float> *vis_img_;
};

template <boxm2_data_type APM_TYPE>
class boxm2_render_exp_image_functor
{
 public:
  // "default" constructor
  boxm2_render_exp_image_functor() = default;

  inline bool init_data(std::vector<boxm2_data_base*> & datas, vil_image_view<float> * expected, vil_image_view<float>* vis_img)
  {
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    mog3_data_=new boxm2_data<APM_TYPE>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    expected_img_=expected;
    vis_img_     =vis_img;
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i, unsigned j, float abs_depth )
  {
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    float vis=(*vis_img_)(i,j);
    float exp_int=(*expected_img_)(i,j);
    float curr_p=(1-std::exp(-alpha*seg_len))*vis;
    float exp_color = boxm2_processor_type<APM_TYPE>::type::expected_color(mog3_data_->data()[index]);
    exp_int += curr_p * exp_color;
    (*expected_img_)(i,j)=exp_int;
    vis*=std::exp(-alpha*seg_len);
    (*vis_img_)(i,j)=vis;
    return true;
  }

 private:
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  boxm2_data<APM_TYPE> * mog3_data_;
  vil_image_view<float> *expected_img_;
  vil_image_view<float> *vis_img_;
};

//: Functor class to normalize expected image
class normalize_intensity
{
 public:
  normalize_intensity() = default;

  void operator()(float mask, float &pix) const
  {
    pix+=mask*0.5f;
  }
};

#endif
