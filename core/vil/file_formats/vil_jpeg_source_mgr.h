// This is mul/vil2/file_formats/vil2_jpeg_source_mgr.h
#ifndef vil2_jpeg_source_mgr_h_
#define vil2_jpeg_source_mgr_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk
// \verbatim
//  Modifications
//     11 Oct 2002 Ian Scott - converted to vil2
//\endverbatim

#include <vil2/file_formats/vil2_jpeglib.h>
class vil2_stream;

//: this is the data source structure which allows JPEG to read from a vil2_stream.
struct vil2_jpeg_stream_source_mgr
{
  struct jpeg_source_mgr base;

  vil2_stream *stream;           /* source stream */
  JOCTET * buffer;              /* start of buffer */
  jpeg_boolean start_of_file;   /* have we gotten any data yet? */
};

void
vil2_jpeg_init_source (j_decompress_ptr cinfo);

jpeg_boolean
vil2_jpeg_fill_input_buffer (j_decompress_ptr cinfo);

void
vil2_jpeg_skip_input_data (j_decompress_ptr cinfo, long num_bytes);

void
vil2_jpeg_term_source (j_decompress_ptr cinfo);

void
vil2_jpeg_stream_src_set (j_decompress_ptr cinfo, vil2_stream *vs);

void
vil2_jpeg_stream_src_rewind(j_decompress_ptr cinfo, vil2_stream *vs);

#endif // vil2_jpeg_source_mgr_h_
