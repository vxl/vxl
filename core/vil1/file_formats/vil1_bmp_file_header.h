// This is core/vil1/file_formats/vil1_bmp_file_header.h
#ifndef vil1_bmp_file_header_h_
#define vil1_bmp_file_header_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm

#include <iosfwd>
#include <vcl_compiler.h>
class vil1_stream;

struct vil1_bmp_file_header
{
  char     magic[2];      // 2
  unsigned file_size;     // 4
  short    reserved1;     // 2
  short    reserved2;     // 2
  unsigned bitmap_offset; // 4

  vil1_bmp_file_header();
  void read(vil1_stream *);
  void write(vil1_stream *) const;
  void print(std::ostream &) const;
  bool signature_valid() const;
};

#endif // vil1_bmp_file_header_h_
