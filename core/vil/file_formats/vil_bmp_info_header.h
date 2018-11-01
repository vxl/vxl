// This is core/vil/file_formats/vil_bmp_info_header.h
#ifndef vil_bmp_info_header_h_
#define vil_bmp_info_header_h_
//:
// \file
// \author fsm

class vil_stream;
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

struct vil_bmp_info_header
{
  enum { disk_size = 4+4+4+4+4+4 };  // this is what is *on disk*.
  unsigned compression; // 4
  unsigned bitmap_size; // 4
  unsigned horiz_res;   // 4
  unsigned verti_res;   // 4
  unsigned colormapsize;// 4
  unsigned colorcount;  // 4

  vil_bmp_info_header();
  void read(vil_stream *);
  void write(vil_stream *) const;
  void print(std::ostream &) const;
};

#endif // vil_bmp_info_header_h_
