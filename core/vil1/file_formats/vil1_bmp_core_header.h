#ifndef vil_bmp_core_header_h_
#define vil_bmp_core_header_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_bmp_core_header
// .INCLUDE vil/file_formats/vil_bmp_core_header.h
// .FILE file_formats/vil_bmp_core_header.cxx
// @author fsm@robots.ox.ac.uk

class vil_stream;
#include <vcl_iosfwd.h>

struct vil_bmp_core_header
{
  enum { disk_size = 4+4+4+2+2 }; // this is what is *on disk*.
  unsigned  header_size;  // 4
  unsigned  width;        // 4
  unsigned  height;       // 4
  short     planes;       // 2
  short     bitsperpixel; // 2
  
  vil_bmp_core_header();
  void read(vil_stream *);
  void write(vil_stream *) const;
  void print(vcl_ostream &) const;
};

#endif // vil_bmp_core_header_h_
