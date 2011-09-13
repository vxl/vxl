#ifndef boxm2_cast_intensities_functor_h
#define boxm2_cast_intensities_functor_h
//:
// \file

#include <boxm2/cpp/algo/boxm2_cast_ray_function.h>
#include <vil/vil_image_view.h>
#include <vcl_iostream.h>

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

  inline bool step_cell(float seg_len,int index,unsigned i,unsigned j, float abs_depth=0.0f)
  {
    boxm2_data<BOXM2_AUX0>::datatype & alpha=alpha_data_->data()[index];
    alpha = (*input_img_)(i,j);
    return true;
  }
 private:
   boxm2_data<BOXM2_AUX0> * alpha_data_;
   vil_image_view<float> * input_img_;
};

class boxm2_data_print_functor
{
 public:
  //: "default" constructor
  boxm2_data_print_functor() {}

  bool init_data(boxm2_data_base* data, vcl_size_t data_size, vcl_string prefix)
  {
    buf_ = data;
    data_size_ = data_size;
    prefix_ = prefix;
    return true;
  }
  inline bool process_cell(int index)
  {
    int byte_ind = index*(int)data_size_;
    char * arr = buf_->cell_buffer(byte_ind, data_size_);
    return arr != 0;
  }

 private:
  boxm2_data_base *buf_;
  vcl_size_t data_size_;
  vcl_string prefix_;
};

#endif
