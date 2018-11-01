// This is core/vil1/file_formats/vil1_bmp_info_header.h
#ifndef vil1_bmp_info_header_h_
#define vil1_bmp_info_header_h_
//:
// \file
// \author fsm

class vil1_stream;
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

struct vil1_bmp_info_header
{
  enum { disk_size = 4+4+4+4+4+4 };  // this is what is *on disk*.
  unsigned compression; // 4
  unsigned bitmap_size; // 4
  unsigned horiz_res;   // 4
  unsigned verti_res;   // 4
  unsigned colormapsize;// 4
  unsigned colorcount;  // 4

  vil1_bmp_info_header();
  void read(vil1_stream *);
  void write(vil1_stream *) const;
  void print(std::ostream &) const;
};

#endif // vil1_bmp_info_header_h_
