#ifdef __GNUC__
#pragma implementation
#endif

//:
// \file
// \brief A base class for writing to avi files
// \author Louise Butcher

#include <mvl2/mvl2_video_writer.h>

mvl2_video_writer::mvl2_video_writer()
{
}

mvl2_video_writer::~mvl2_video_writer()
{
if (is_opened()) close();
}

vcl_string mvl2_video_writer::is_a() const
{
  return vcl_string("mvl2_video_writer");
}
