#ifndef boxm2_image_density_functor_h
#define boxm2_image_density_functor_h
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <boxm2/cpp/algo/boxm2_gauss_grey_processor.h>
#include <vil/vil_image_view.h>

template <boxm2_data_type APM_TYPE>
class boxm2_image_density_functor
{
 public:
  //: "default" constructor
  boxm2_image_density_functor() = default;

  bool init_data(std::vector<boxm2_data_base*> & datas, vil_image_view<float> * img, vil_image_view<float> * density_img, vil_image_view<float>* vis_img)
  {
    alpha_data_=new boxm2_data<BOXM2_ALPHA>(datas[0]->data_buffer(),datas[0]->buffer_length(),datas[0]->block_id());
    mog3_data_=new boxm2_data<APM_TYPE>(datas[1]->data_buffer(),datas[1]->buffer_length(),datas[1]->block_id());
    img_=img;
    density_img_=density_img;
    vis_img_ = vis_img;
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i, unsigned j, float abs_depth = 0.0)
  {
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    float vis=(*vis_img_)(i,j);
    float color=(*img_)(i,j);
    float curr_p=(1-std::exp(-alpha*seg_len))*vis;
    float color_dens = boxm2_processor_type<APM_TYPE>::type::prob_density(mog3_data_->data()[index], color);
    (*density_img_)(i,j)+=color_dens*curr_p;
    vis*=std::exp(-alpha*seg_len);
    (*vis_img_)(i,j)=vis;
    return true;
  }
 private:
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  boxm2_data<APM_TYPE> * mog3_data_;
  vil_image_view<float> *density_img_;
  vil_image_view<float> *img_;
  vil_image_view<float> *vis_img_;
};



#endif
