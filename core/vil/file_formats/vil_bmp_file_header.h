// This is core/vil/file_formats/vil_bmp_file_header.h
#ifndef vil_bmp_file_header_h_
#define vil_bmp_file_header_h_
//:
// \file
// \author fsm

#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class vil_stream;

struct vil_bmp_file_header
{
  char     magic[2];      // 2
  unsigned file_size;     // 4
  short    reserved1;     // 2
  short    reserved2;     // 2
  unsigned bitmap_offset; // 4

  vil_bmp_file_header();
  void read(vil_stream *);
  void write(vil_stream *) const;
  void print(std::ostream &) const;
  bool signature_valid() const;
};

#endif // vil_bmp_file_header_h_
