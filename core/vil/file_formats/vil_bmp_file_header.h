// This is mul/vil2/file_formats/vil2_bmp_file_header.h
#ifndef vil2_bmp_file_header_h_
#define vil2_bmp_file_header_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include <vcl_iosfwd.h>
class vil2_stream;

struct vil2_bmp_file_header
{
  char     magic[2];      // 2
  unsigned file_size;     // 4
  short    reserved1;     // 2
  short    reserved2;     // 2
  unsigned bitmap_offset; // 4

  vil2_bmp_file_header();
  void read(vil2_stream *);
  void write(vil2_stream *) const;
  void print(vcl_ostream &) const;
  bool signature_valid() const;
};

#endif // vil2_bmp_file_header_h_
