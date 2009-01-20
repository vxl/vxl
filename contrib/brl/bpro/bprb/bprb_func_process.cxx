#include "bprb_func_process.h"

//#include <vpgl/vpgl_camera.h>
#include <vil/vil_image_view_base.h>

template <class T>
T bprb_func_process::get_input(unsigned i)
{
#if 0
  vcl_cout << input_data_[i]->is_a() << vcl_endl
           << typeid(T).name() << vcl_endl;
  if (input_data_[i]->is_a() != typeid(T).name()){
    vcl_cout << "Input is of the wrong type" << vcl_endl;
    return T(0);
  }
#endif // 0

  brdb_value_t<T>* input = static_cast<brdb_value_t<T>* >(input_data_[i].ptr());
  T val = input->value();
  return val;
}

bool bprb_func_process::execute()
{
  return fpt_(*this);
}

template vcl_string bprb_func_process::get_input(unsigned);
template float bprb_func_process::get_input(unsigned);
template unsigned bprb_func_process::get_input(unsigned);
//template vpgl_camera_double_sptr bprb_func_process::get_input(unsigned);
template vil_image_view_base_sptr bprb_func_process::get_input(unsigned);
