// This is mul/vil2/file_formats/vil2_bmp_core_header.h
#ifndef vil2_bmp_core_header_h_
#define vil2_bmp_core_header_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk
// \verbatim
//  Modifications
//   Nick Costen added Manchester IO 16/3/01
// \endverbatim

class vil2_stream;
#include <vcl_iosfwd.h>

struct vil2_bmp_core_header
{
  enum { disk_size = 4+4+4+2+2 }; // this is what is *on disk*.
  unsigned  header_size;  // 4
  unsigned  width;        // 4
  unsigned  height;       // 4
  short     planes;       // 2
  short     bitsperpixel; // 2

  vil2_bmp_core_header();
  void read(vil2_stream *);
  void write(vil2_stream *) const;
  void print(vcl_ostream &) const;
};

#endif // vil2_bmp_core_header_h_
