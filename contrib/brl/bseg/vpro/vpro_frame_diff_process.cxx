#include <vcl_iostream.h>
#include <brip/brip_vil1_float_ops.h>
#include <vpro/vpro_frame_diff_process.h>

vpro_frame_diff_process::vpro_frame_diff_process(vpro_frame_diff_params fdp):
  vpro_frame_diff_params(fdp)
{
  first_frame_ = true;
}

vpro_frame_diff_process::~vpro_frame_diff_process()
{
}

bool vpro_frame_diff_process::execute()
{
  if (!this->get_N_input_images()==1)
    {
      vcl_cout << "In vpro_frame_diff_process::execute() - not at exactly one"
               << " input image \n";
      return false;
    }
  vil1_image img = vpro_video_process::get_input_image(0);
  this->clear_input();

  vil1_memory_image_of<float> fimg = brip_vil1_float_ops::convert_to_float(img);

  //if first frame, just cache
  if (first_frame_)
    {
      img0_=fimg;
      first_frame_ = false;
      return false;
    }

  //compute difference
  vil1_memory_image_of<float> dif = brip_vil1_float_ops::difference(img0_,fimg);

    
  // push back buffer
    img0_=fimg;

  //convert output back to unsigned char
  output_image_ = brip_vil1_float_ops::convert_to_byte(dif, -range_, range_);
  return true;
}

bool vpro_frame_diff_process::finish()
{
  first_frame_ = true;
  return true;
}
