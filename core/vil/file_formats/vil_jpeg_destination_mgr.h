// This is mul/vil2/file_formats/vil2_jpeg_destination_mgr.h
#ifndef vil2_jpeg_destination_mgr_h_
#define vil2_jpeg_destination_mgr_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm
// \verbatim
//  Modifications
//     11 Oct 2002 Ian Scott - converted to vil22
//\endverbatim

#include <vil2/file_formats/vil2_jpeglib.h>
class vil2_stream;

//: this is the data source structure which allows JPEG to write to a vil2_stream.
struct vil2_jpeg_stream_destination_mgr
{
  struct jpeg_destination_mgr base;

  vil2_stream *stream;           /* target stream */
  JOCTET * buffer;              /* start of buffer */
};

void
vil2_jpeg_init_destination (j_compress_ptr cinfo);

jpeg_boolean
vil2_jpeg_empty_output_buffer (j_compress_ptr cinfo);

void
vil2_jpeg_term_destination (j_compress_ptr cinfo);

void
vil2_jpeg_stream_dst_set (j_compress_ptr cinfo, vil2_stream *vs);

void
vil2_jpeg_stream_dst_rewind(j_compress_ptr cinfo, vil2_stream *vs);

#endif // vil2_jpeg_destination_mgr_h_
