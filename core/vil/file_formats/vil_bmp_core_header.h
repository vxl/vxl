// This is core/vil/file_formats/vil_bmp_core_header.h
#ifndef vil_bmp_core_header_h_
#define vil_bmp_core_header_h_
//:
// \file
// \author fsm
// \verbatim
//  Modifications
//   Nick Costen added Manchester IO 16/3/01
// \endverbatim

class vil_stream;
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

struct vil_bmp_core_header
{
  enum { disk_size = 4+4+4+2+2 }; // this is what is *on disk*.
  unsigned  header_size;  // 4
  int       width;        // 4, can be negative
  int       height;       // 4, can be negative
  short     planes;       // 2
  short     bitsperpixel; // 2

  vil_bmp_core_header();
  void read(vil_stream *);
  void write(vil_stream *) const;
  void print(std::ostream &) const;
};

#endif // vil_bmp_core_header_h_
