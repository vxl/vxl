#include <vcl_iostream.h>
#include <vil1/vil1_memory_image_of.h>
#include <vidl/vidl_movie.h>
#include <vidl/vidl_clip.h>
#include <vidl/vidl_io.h>
#include <vpro/vpro_capture_process.h>

vpro_capture_process::vpro_capture_process(vcl_string const& video_file)
{
  video_file_ = video_file;
}

vpro_capture_process::~vpro_capture_process()
{
}

bool vpro_capture_process::execute()
{
  if (this->get_N_input_images()!=1)
    {
      vcl_cout << "In vpro_capture_process::execute() - not exactly one"
               << " input image \n";
    return false;
    }
    //JLM
  //vil_memory_image_of<unsigned char> img(vpro_video_process::get_input_image(0));
  vil1_image img = vpro_video_process::get_input_image(0);
  frames_.push_back(img);
  return true;
}

bool vpro_capture_process::finish()
{
  if(!frames_.size())
    return false;
  vidl_clip_sptr clip = new vidl_clip(frames_);
  vidl_movie_sptr mov= new vidl_movie();
  mov->add_clip(clip);
  if (!vidl_io::save(mov.ptr(), video_file_.c_str(), "ImageList"))
    {
      vcl_cout << "In vpro_capture_process::finish() - failed to save video\n";
      return false;
    }
  frames_.clear();
  vcl_cout << "Finished Saving Captured Video File\n";
  return true;
}
