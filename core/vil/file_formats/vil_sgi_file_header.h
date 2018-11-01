// This is core/vil/file_formats/vil_sgi_file_header.h
#ifndef vil_sgi_file_header_h_
#define vil_sgi_file_header_h_
//:
// \file
// \author David Hughes

#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class vil_stream;

struct vil_sgi_file_header
{
  unsigned char magic[2]; // 2
  char     storage;       // 1
  char     bpc;           // 1
  short    dimension;     // 2
  short    xsize;         // 2
  short    ysize;         // 2
  short    zsize;         // 2
  unsigned pixmin;        // 4
  unsigned pixmax;        // 4
  char     dummy1[4];     // 4
  char     image_name[80];// 80
  unsigned colormap;      // 4
  char     dummy2[404];   // 404

  vil_sgi_file_header();
  void read(vil_stream *);
  void write(vil_stream *) const;
  void print(std::ostream &) const;
  bool signature_valid() const;
};

#endif // vil_sgi_file_header_h_
