#include <vcl_iostream.h>
#include <brip/brip_float_ops.h>
#include <vvid/vvid_frame_diff_process.h>

vvid_frame_diff_process::vvid_frame_diff_process()
{
  first_frame_ = true;
}

vvid_frame_diff_process::~vvid_frame_diff_process()
{
}

bool vvid_frame_diff_process::execute()
{
  if (!this->get_N_inputs()==1)
    {
      vcl_cout << "In vvid_frame_diff_process::execute() - not at exactly one"
               << " input image \n";
      return false;
    }
  vil_image img = vvid_video_process::get_input_image(0);
  vil_memory_image_of<unsigned char> temp(img);
  vil_memory_image_of<float> fimg = brip_float_ops::convert_to_float(temp);
  this->clear_input();

  //if first frame, just cache
  if(first_frame_)
    {
      img0_=fimg;
      first_frame_ = false;
      return false;
    }

  //compute difference
  vil_memory_image_of<float> dif = brip_float_ops::difference(img0_,fimg);
    
  // push back buffer 
  img0_=fimg;

  //convert output back to unsigned char
  output_image_ = brip_float_ops::convert_to_byte(dif, -50, 50);
  return true;
}

bool vvid_frame_diff_process::finish()
{
  first_frame_ = true;
  return true;
}
