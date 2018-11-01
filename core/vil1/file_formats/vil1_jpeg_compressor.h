// This is core/vil1/file_formats/vil1_jpeg_compressor.h
#ifndef vil1_jpeg_compressor_h_
#define vil1_jpeg_compressor_h_
//:
// \file
// \author fsm

#include <vil1/vil1_jpeglib.h>
class vil1_stream;

class vil1_jpeg_compressor
{
 public:
  struct jpeg_error_mgr         jerr;
  struct jpeg_compress_struct   jobj;
  vil1_stream *stream;

  vil1_jpeg_compressor(vil1_stream *s);

  // NB. does not delete the stream
  ~vil1_jpeg_compressor();

  bool write_scanline(unsigned line, JSAMPLE const *);

 private:
  bool ready;
};

#endif // vil1_jpeg_compressor_h_
