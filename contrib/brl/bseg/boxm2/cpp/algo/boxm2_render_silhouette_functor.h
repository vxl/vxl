#ifndef boxm2_render_silhouette_functor_h
#define boxm2_render_silhouette_functor_h
//:
// \file

#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <vil/vil_image_view.h>

class boxm2_render_silhouette_functor
{
 public:
  //: "default" constructor
  boxm2_render_silhouette_functor() = default;

  bool init_data(boxm2_data_base* alpha_data, vil_image_view<float> * silhouette, vil_image_view<float>* vis_img)
  {
    alpha_data_ = new boxm2_data<BOXM2_ALPHA>(alpha_data->data_buffer(),alpha_data->buffer_length(),alpha_data->block_id());
    silhouette_img_ = silhouette;
    vis_img_ = vis_img;
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i, unsigned j, float abs_depth=0.0f)
  {
    boxm2_data<BOXM2_ALPHA>::datatype alpha=alpha_data_->data()[index];
    float vis=(*vis_img_)(i,j);
    float curr_p=(1-std::exp(-alpha*seg_len))*vis;
    if(curr_p>0.0f)
      (*silhouette_img_)(i,j)=0.0f;
    vis*=std::exp(-alpha*seg_len);
    (*vis_img_)(i,j)=vis;
    return true;
  }
 private:
  boxm2_data<BOXM2_ALPHA> * alpha_data_;
  vil_image_view<float> *silhouette_img_;
  vil_image_view<float> *vis_img_;
};

#endif
