#ifndef vil_jpeg_compressor_h_
#define vil_jpeg_compressor_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_jpeg_compressor
// .INCLUDE vil/file_formats/vil_jpeg_compressor.h
// .FILE file_formats/vil_jpeg_compressor.cxx
// @author fsm@robots.ox.ac.uk

#include <vil/vil_jpeglib.h>
class vil_stream;

class vil_jpeg_compressor {
public:
  struct jpeg_error_mgr         jerr;
  struct jpeg_compress_struct   jobj;
  vil_stream *stream;

  vil_jpeg_compressor(vil_stream *s);

  // NB. does not delete the stream
  ~vil_jpeg_compressor();

  bool write_scanline(unsigned line, JSAMPLE const *);

private:
  bool ready;
};

#endif // vil_jpeg_compressor_h_
