#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vil1/vil1_image.h>
#include <vidl_vil1/vidl_vil1_movie.h>
#include <vidl_vil1/vidl_vil1_clip.h>
#include <vidl_vil1/vidl_vil1_io.h>
#include <brip/brip_vil1_float_ops.h>
#include <vpro/vpro_half_res_process.h>



vpro_half_res_process::vpro_half_res_process(vcl_string const& video_file)
{
  video_file_ = video_file;
}

vpro_half_res_process::~vpro_half_res_process()
{
}

bool vpro_half_res_process::execute()
{
  if (this->get_N_input_images()!=1)
    {
      vcl_cout << "In vpro_half_res_process::execute() -"
               << " not exactly one input image \n";
    return false;
    }
  vil1_image img = vpro_video_process::get_input_image(0);
  this->clear_input();
  if (img.components()==1)
    {
      vil1_memory_image_of<float> fimg = 
        brip_vil1_float_ops::convert_to_float(img);
      vil1_memory_image_of<float> half = 
        brip_vil1_float_ops::half_resolution(fimg);
      vil1_memory_image_of<unsigned char> chalf = 
        brip_vil1_float_ops::convert_to_byte(half);
      out_frames_.push_back(chalf);
      return true;
    }
  if(img.components()==3)
    {
      vil1_memory_image_of<vil1_rgb<unsigned char> > cimg(img);
	  vil1_memory_image_of<vil1_rgb<unsigned char> > cimg_half = 
        brip_vil1_float_ops::half_resolution(cimg);
      out_frames_.push_back(cimg_half);
      return true;
    }
  return false;
}
bool vpro_half_res_process::finish()
{
  if (!out_frames_.size())
    return false;
  vidl_vil1_clip_sptr clip = new vidl_vil1_clip(out_frames_);
  vidl_vil1_movie_sptr mov= new vidl_vil1_movie();
  mov->add_clip(clip);
  if (!vidl_vil1_io::save(mov.ptr(), video_file_.c_str(), "AVI")){
    vcl_cout << "In vpro_half_res_process::finish()"
             << " - failed to save video" << vcl_endl;
    return false;
  }
  out_frames_.clear();
  return true;
 }

