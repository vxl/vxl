#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief A class for writing videos
// \author Franck Bettinger

#include "mvl2_video_to_avi_windows.h"

mvl2_video_to_avi::mvl2_video_to_avi()
{
}
 
mvl2_video_to_avi::~mvl2_video_to_avi()
{
}

bool mvl2_video_to_avi::set_codec(char a, char b, char c, char d)
{
  return true;
}

void mvl2_video_to_avi::set_quality(int qual)
{
}
 
bool mvl2_video_to_avi::open( int width, int height, 
                              vcl_string format, vcl_string file_name)
{
  return true;
}
 
void mvl2_video_to_avi::close()
{
}
 
int mvl2_video_to_avi::get_width()
{
  return width_; 
}
 
int mvl2_video_to_avi::get_height()
{
  return height_; 
}
 
void mvl2_video_to_avi::set_frame_rate(double frame_rate)
{
}
 
void mvl2_video_to_avi::write_frame(vimt_image_2d_of<vxl_byte>& image)
{
}

vcl_string mvl2_video_to_avi::is_a() const 
{
  return vcl_string("mvl2_video_to_avi");
}
 
mvl2_video_writer* mvl2_video_to_avi::clone() const 
{
  return new mvl2_video_to_avi(*this);
}
