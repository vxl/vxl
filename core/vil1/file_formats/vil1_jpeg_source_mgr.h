// This is vxl/vil/file_formats/vil_jpeg_source_mgr.h
#ifndef vil_jpeg_source_mgr_h_
#define vil_jpeg_source_mgr_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include <vil/vil_jpeglib.h>
class vil_stream;

//: this is the data source structure which allows JPEG to read from a vil_stream.
struct vil_jpeg_stream_source_mgr
{
  struct jpeg_source_mgr base;

  vil_stream *stream;           /* source stream */
  JOCTET * buffer;              /* start of buffer */
  jpeg_boolean start_of_file;   /* have we gotten any data yet? */
};

void
vil_jpeg_init_source (j_decompress_ptr cinfo);

jpeg_boolean
vil_jpeg_fill_input_buffer (j_decompress_ptr cinfo);

void
vil_jpeg_skip_input_data (j_decompress_ptr cinfo, long num_bytes);

void
vil_jpeg_term_source (j_decompress_ptr cinfo);

void
vil_jpeg_stream_src_set (j_decompress_ptr cinfo, vil_stream *vs);

void
vil_jpeg_stream_src_rewind(j_decompress_ptr cinfo, vil_stream *vs);

#endif // vil_jpeg_source_mgr_h_
