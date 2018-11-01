// This is core/vil1/file_formats/vil1_jpeg_destination_mgr.h
#ifndef vil1_jpeg_destination_mgr_h_
#define vil1_jpeg_destination_mgr_h_
//:
// \file
// \author fsm

#include <vil1/vil1_jpeglib.h>
class vil1_stream;

//: this is the data source structure which allows JPEG to write to a vil1_stream.
struct vil1_jpeg_stream_destination_mgr
{
  struct jpeg_destination_mgr base;

  vil1_stream *stream;           /* target stream */
  JOCTET * buffer;              /* start of buffer */
};

void
vil1_jpeg_init_destination (j_compress_ptr cinfo);

jpeg_boolean
vil1_jpeg_empty_output_buffer (j_compress_ptr cinfo);

void
vil1_jpeg_term_destination (j_compress_ptr cinfo);

void
vil1_jpeg_stream_dst_set (j_compress_ptr cinfo, vil1_stream *vs);

void
vil1_jpeg_stream_dst_rewind(j_compress_ptr cinfo, vil1_stream *vs);

#endif // vil1_jpeg_destination_mgr_h_
