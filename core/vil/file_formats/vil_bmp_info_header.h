// This is mul/vil2/file_formats/vil2_bmp_info_header.h
#ifndef vil2_bmp_info_header_h_
#define vil2_bmp_info_header_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

class vil2_stream;
#include <vcl_iosfwd.h>

struct vil2_bmp_info_header
{
  enum { disk_size = 4+4+4+4+4+4 };  // this is what is *on disk*.
  unsigned compression; // 4
  unsigned bitmap_size; // 4
  unsigned horiz_res;   // 4
  unsigned verti_res;   // 4
  unsigned colormapsize;// 4
  unsigned colorcount;  // 4

  vil2_bmp_info_header();
  void read(vil2_stream *);
  void write(vil2_stream *) const;
  void print(vcl_ostream &) const;
};

#endif // vil2_bmp_info_header_h_
