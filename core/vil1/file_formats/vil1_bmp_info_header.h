#ifndef vil_bmp_info_header_h_
#define vil_bmp_info_header_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_bmp_info_header
// .INCLUDE vil/file_formats/vil_bmp_info_header.h
// .FILE file_formats/vil_bmp_info_header.cxx
// @author fsm@robots.ox.ac.uk

class vil_stream;
#include <vcl/vcl_iosfwd.h>

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
  void print(ostream &) const;
};

#endif // vil_bmp_info_header_h_
