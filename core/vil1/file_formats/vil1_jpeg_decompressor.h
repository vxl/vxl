// This is vxl/vil/file_formats/vil_jpeg_decompressor.h
#ifndef vil_jpeg_decompressor_h_
#define vil_jpeg_decompressor_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm

#include <vil/vil_jpeglib.h>
class vil_stream;

class vil_jpeg_decompressor
{
 public:
  struct jpeg_error_mgr         jerr;
  struct jpeg_decompress_struct jobj;
  vil_stream *stream;

  vil_jpeg_decompressor(vil_stream *s);

  // NB. does not delete the stream.
  ~vil_jpeg_decompressor();

  // Do *not* delete the return value. Leave it alone.
  // The return value is zero on failure.
  // It should cost nothing to read the same scanline twice in succession.
  JSAMPLE const *read_scanline(unsigned line);

 private:
  bool ready; // true if decompression has started but not finished.
  bool valid; // true if last scanline read was successful.

  // It's not worth the effort using JPEG to allocate the buffer using the
  // jobj.mem->alloc_sarray method, because it would have to be reallocated
  // after each call to jpeg_finish_decompress(). The symptom of not doing
  // so is a nasty heap corruption which only shows up later in unrelated 
  // code.
  JSAMPLE *biffer;   // pointer to scanline buffer.
};

#endif // vil_jpeg_decompressor_h_
