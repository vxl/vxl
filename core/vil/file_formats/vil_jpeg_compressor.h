// This is core/vil/file_formats/vil_jpeg_compressor.h
#ifndef vil_jpeg_compressor_h_
#define vil_jpeg_compressor_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm
// \verbatim
//  Modifications
//     11 Oct 2002 Ian Scott - converted to vil
//\endverbatim

#include <vil/file_formats/vil_jpeglib.h>
class vil_stream;

class vil_jpeg_compressor
{
 public:
  struct jpeg_error_mgr         jerr;
  struct jpeg_compress_struct   jobj;
  vil_stream *stream;

  vil_jpeg_compressor(vil_stream *s);

  // NB. does not delete the stream
  ~vil_jpeg_compressor();

  bool write_scanline(unsigned line, JSAMPLE const *);

  void set_quality(int quality);
  int get_quality();

 private:
  bool ready;
  int quality;
};

#endif // vil_jpeg_compressor_h_
