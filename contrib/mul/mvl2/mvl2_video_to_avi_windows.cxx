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

bool mvl2_video_to_avi::set_codec(char /*a*/, char /*b*/, char /*c*/, char /*d*/)
{
  vcl_cerr << "mvl2_video_to_avi::set_codec() NYI\n";
  return false;
}

void mvl2_video_to_avi::set_quality(int /*qual*/)
{
  vcl_cerr << "mvl2_video_to_avi::set_quality() NYI\n";
}
 
bool mvl2_video_to_avi::open( int /*width*/, int /*height*/, 
                              vcl_string /*format*/, vcl_string /*file_name*/)
{
  vcl_cerr << "mvl2_video_to_avi::open() NYI\n";
  return false;
}
 
void mvl2_video_to_avi::close()
{
  vcl_cerr << "mvl2_video_to_avi::close() NYI\n";
}
 
int mvl2_video_to_avi::get_width()
{
  return width_; 
}
 
int mvl2_video_to_avi::get_height()
{
  return height_; 
}
 
void mvl2_video_to_avi::set_frame_rate(double /*frame_rate*/)
{
  vcl_cerr << "mvl2_video_to_avi::set_frame_rate() NYI\n";
}
 
void mvl2_video_to_avi::write_frame(vil_image_view<vxl_byte>& /*image*/)
{
  vcl_cerr << "mvl2_video_to_avi::write_frame() NYI\n";
}

vcl_string mvl2_video_to_avi::is_a() const 
{
  return vcl_string("mvl2_video_to_avi");
}
 
mvl2_video_writer* mvl2_video_to_avi::clone() const 
{
  return new mvl2_video_to_avi(*this);
}
