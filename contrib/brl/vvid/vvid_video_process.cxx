#include <vcl_iostream.h>
#include <vvid/vvid_video_process.h>

vvid_video_process::vvid_video_process()
{
}

vvid_video_process::~vvid_video_process()
{
}

vil_image  vvid_video_process::get_input_image(int i)
{
  if (i<0||i>(int)_input_images.size())
    {
      vcl_cout << "In vvid_video_process::get_input_image(..) - index out of"
               << " bounds \n";
      return vil_image();
    }
  return _input_images[i];
}
