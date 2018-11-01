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
  std::cerr << "mvl2_video_to_avi::set_codec() NYI\n";
  return false;
}

void mvl2_video_to_avi::set_quality(int /*qual*/)
{
  std::cerr << "mvl2_video_to_avi::set_quality() NYI\n";
}

bool mvl2_video_to_avi::open( int /*width*/, int /*height*/,
                              std::string /*format*/, std::string /*file_name*/)
{
  std::cerr << "mvl2_video_to_avi::open() NYI\n";
  return false;
}

void mvl2_video_to_avi::close()
{
  std::cerr << "mvl2_video_to_avi::close() NYI\n";
}

int mvl2_video_to_avi::get_width() const
{
  return width_;
}

int mvl2_video_to_avi::get_height() const
{
  return height_;
}

void mvl2_video_to_avi::set_frame_rate(double /*frame_rate*/)
{
  std::cerr << "mvl2_video_to_avi::set_frame_rate() NYI\n";
}

void mvl2_video_to_avi::write_frame(vil_image_view<vxl_byte>& /*image*/)
{
  std::cerr << "mvl2_video_to_avi::write_frame() NYI\n";
}

std::string mvl2_video_to_avi::is_a() const
{
  return std::string("mvl2_video_to_avi");
}

mvl2_video_writer* mvl2_video_to_avi::clone() const
{
  return new mvl2_video_to_avi(*this);
}
