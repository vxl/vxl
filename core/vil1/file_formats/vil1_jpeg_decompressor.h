// This is core/vil1/file_formats/vil1_jpeg_decompressor.h
#ifndef vil1_jpeg_decompressor_h_
#define vil1_jpeg_decompressor_h_
//:
// \file
// \author fsm

#include <vil1/vil1_jpeglib.h>
class vil1_stream;

class vil1_jpeg_decompressor
{
 public:
  struct jpeg_error_mgr         jerr;
  struct jpeg_decompress_struct jobj;
  vil1_stream *stream;

  vil1_jpeg_decompressor(vil1_stream *s);

  //:
  // NB. does not delete the stream.
  ~vil1_jpeg_decompressor();

  //:
  // Do \e not delete the return value. Leave it alone.
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

#endif // vil1_jpeg_decompressor_h_
