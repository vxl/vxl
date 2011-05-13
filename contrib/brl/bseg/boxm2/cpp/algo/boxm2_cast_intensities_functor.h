#ifndef boxm2_cast_intensities_functor_h
#define boxm2_cast_intensities_functor_h
//:
// \file
#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <vil/vil_image_view.h>

class boxm2_cast_intensities_functor
{
 public:
  //: "default" constructor
  boxm2_cast_intensities_functor() {}

  bool init_data(boxm2_data_base* data, vil_image_view<float> * input_img)
  {
    alpha_data_=new boxm2_data<BOXM2_AUX0>(data->data_buffer(),data->buffer_length(),data->block_id());
    input_img_=input_img;
    return true;
  }

  inline bool step_cell(float seg_len,int index,unsigned i,unsigned j)
  {
    boxm2_data<BOXM2_AUX0>::datatype & alpha=alpha_data_->data()[index];
    alpha = (*input_img_)(i,j);
    return true;
  }
 private:
   boxm2_data<BOXM2_AUX0> * alpha_data_;
   vil_image_view<float> * input_img_;
};

#endif
