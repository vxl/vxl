#include <vcl_iostream.h>
#include <vil/vil_memory_image_of.h>
#include <vidl/vidl_movie.h>
#include <vidl/vidl_clip.h>
#include <vidl/vidl_io.h>
#include <vvid/vvid_capture_process.h>

vvid_capture_process::vvid_capture_process(vcl_string& video_file)
{
  video_file_ = video_file;
}

vvid_capture_process::~vvid_capture_process()
{
}

bool vvid_capture_process::execute()
{
    if (this->get_N_inputs()!=1)
    {
      vcl_cout << "In vvid_capture_process::execute() - not exactly one"
               << " input image \n";
    return false;
    }
  vil_memory_image_of<unsigned char> img(vvid_video_process::get_input_image(0));
  frames_.push_back(img);
  return true;
}

bool vvid_capture_process::finish()
{
  vidl_clip_sptr clip = new vidl_clip(frames_);
  vidl_movie_sptr mov= new vidl_movie();
  mov->add_clip(clip);
  if(!vidl_io::save(mov.ptr(), video_file_.c_str(), "ImageList"))
    {
      vcl_cout << "In vvid_capture_process::finish() - failed to save video\n";
      return false;
    }
  frames_.clear();
  return true;
}
