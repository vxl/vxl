// This is mul/vil2/file_formats/vil2_jpeg_compressor.h
#ifndef vil2_jpeg_compressor_h_
#define vil2_jpeg_compressor_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk
// \verbatim
//  Modifications
//     11 Oct 2002 Ian Scott - converted to vil22
//\endverbatim

#include <vil2/file_formats/vil2_jpeglib.h>
class vil2_stream;

class vil2_jpeg_compressor
{
 public:
  struct jpeg_error_mgr         jerr;
  struct jpeg_compress_struct   jobj;
  vil2_stream *stream;

  vil2_jpeg_compressor(vil2_stream *s);

  // NB. does not delete the stream
  ~vil2_jpeg_compressor();

  bool write_scanline(unsigned line, JSAMPLE const *);

 private:
  bool ready;
};

#endif // vil2_jpeg_compressor_h_
