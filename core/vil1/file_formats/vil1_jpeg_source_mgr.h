// This is core/vil1/file_formats/vil1_jpeg_source_mgr.h
#ifndef vil1_jpeg_source_mgr_h_
#define vil1_jpeg_source_mgr_h_
//:
// \file
// \author fsm

#include <vil1/vil1_jpeglib.h>
class vil1_stream;

//: this is the data source structure which allows JPEG to read from a vil1_stream.
struct vil1_jpeg_stream_source_mgr
{
  struct jpeg_source_mgr base;

  vil1_stream *stream;           /* source stream */
  JOCTET * buffer;              /* start of buffer */
  jpeg_boolean start_of_file;   /* have we gotten any data yet? */
};

void
vil1_jpeg_init_source (j_decompress_ptr cinfo);

jpeg_boolean
vil1_jpeg_fill_input_buffer (j_decompress_ptr cinfo);

void
vil1_jpeg_skip_input_data (j_decompress_ptr cinfo, long num_bytes);

void
vil1_jpeg_term_source (j_decompress_ptr cinfo);

void
vil1_jpeg_stream_src_set (j_decompress_ptr cinfo, vil1_stream *vs);

void
vil1_jpeg_stream_src_rewind(j_decompress_ptr cinfo, vil1_stream *vs);

#endif // vil1_jpeg_source_mgr_h_
