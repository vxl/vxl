#include <vcl_iostream.h>
#include <brip/brip_vil1_float_ops.h>
#include <vpro/vpro_spatial_filter_process.h>
#include <vil1/vil1_memory_image_of.h>

bool vpro_spatial_filter_process::execute()
{
  if (this->get_N_input_images()!=1)
  {
    vcl_cout << "In vpro_spatial_filter_process::execute() -"
             << " not at exactly one input image\n";
    return false;
  }
  vil1_image img = vpro_video_process::get_input_image(0);
  this->clear_input();

  vil1_memory_image_of<float> fimg = brip_vil1_float_ops::convert_to_float(img);

  vil1_memory_image_of<float> out;
  brip_vil1_float_ops::spatial_frequency_filter(fimg, dir_fx_, dir_fy_, f0_,
                                           radius_, show_filtered_fft_,
                                           out);
  if (show_filtered_fft_)
    output_image_ = brip_vil1_float_ops::convert_to_byte(out, 0.0f, 0.05f);
  else
    output_image_ = brip_vil1_float_ops::convert_to_byte(out, 0.0f, 150.0f);
  return true;
}
