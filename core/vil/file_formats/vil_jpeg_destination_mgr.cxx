// This is core/vil/file_formats/vil_jpeg_destination_mgr.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm
// \verbatim
//  Modifications
//     11 Oct 2002 Ian Scott - converted to vil
//\endverbatim

#include "vil_jpeg_destination_mgr.h"
#include <vcl_cassert.h>
#include <vcl_cstddef.h> // for vcl_size_t
#include <vil/vil_stream.h>

#define STATIC /*static*/

// In ANSI C, and indeed any rational implementation, vcl_size_t is also the
// type returned by sizeof().  However, it seems there are some irrational
// implementations out there, in which sizeof() returns an int even though
// vcl_size_t is defined as long or unsigned long.  To ensure consistent results
// we always use this SIZEOF() macro in place of using sizeof() directly.

#define SIZEOF(object) ((vcl_size_t) sizeof(object))

// Implement a jpeg_destination_manager for vil_stream *.
// Adapted by fsm from the FILE * version in jdatadst.c

#define vil_jpeg_OUTPUT_BUF_SIZE  4096 // choose an efficiently fwrite'able size
typedef vil_jpeg_stream_destination_mgr *vil_jpeg_dstptr;


//  * Initialize destination --- called by jpeg_start_compress
//  * before any data is actually written.
STATIC
void
vil_jpeg_init_destination (j_compress_ptr cinfo)
{
  vil_jpeg_dstptr dest = (vil_jpeg_dstptr) cinfo->dest; // cast to derived class

  // Allocate the output buffer --- it will be released when done with image
  dest->buffer = (JOCTET *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo,
                                JPOOL_IMAGE,
                                vil_jpeg_OUTPUT_BUF_SIZE * SIZEOF(JOCTET));

  dest->base.next_output_byte = dest->buffer;
  dest->base.free_in_buffer = vil_jpeg_OUTPUT_BUF_SIZE;
}


//  * Empty the output buffer --- called whenever buffer fills up.
//  *
//  * In typical applications, this should write the entire output buffer
//  * (ignoring the current state of next_output_byte & free_in_buffer),
//  * reset the pointer & count to the start of the buffer, and return TRUE
//  * indicating that the buffer has been dumped.
//  *
//  * In applications that need to be able to suspend compression due to output
//  * overrun, a FALSE return indicates that the buffer cannot be emptied now.
//  * In this situation, the compressor will return to its caller (possibly with
//  * an indication that it has not accepted all the supplied scanlines).  The
//  * application should resume compression after it has made more room in the
//  * output buffer.  Note that there are substantial restrictions on the use of
//  * suspension --- see the documentation.
//  *
//  * When suspending, the compressor will back up to a convenient restart point
//  * (typically the start of the current MCU). next_output_byte & free_in_buffer
//  * indicate where the restart point will be if the current call returns FALSE.
//  * Data beyond this point will be regenerated after resumption, so do not
//  * write it out when emptying the buffer externally.
jpeg_boolean
vil_jpeg_empty_output_buffer (j_compress_ptr cinfo)
{
  vil_jpeg_dstptr dest = (vil_jpeg_dstptr) cinfo->dest; // cast to derived class

  if (dest->stream->write(dest->buffer, vil_jpeg_OUTPUT_BUF_SIZE) != (vcl_size_t) vil_jpeg_OUTPUT_BUF_SIZE)
    ERREXIT(cinfo, JERR_FILE_WRITE);

  dest->base.next_output_byte = dest->buffer;
  dest->base.free_in_buffer = vil_jpeg_OUTPUT_BUF_SIZE;

  return TRUE;
}


//  * Terminate destination --- called by jpeg_finish_compress
//  * after all data has been written.  Usually needs to flush buffer.
//  *
//  * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
//  * application must deal with any cleanup that should happen even
//  * for error exit.
void
vil_jpeg_term_destination (j_compress_ptr cinfo)
{
  vil_jpeg_dstptr dest = (vil_jpeg_dstptr) cinfo->dest; // cast to derived class
  vcl_size_t datacount = vil_jpeg_OUTPUT_BUF_SIZE - dest->base.free_in_buffer;

  // Write any data remaining in the buffer
  if (datacount > 0) {
    if (dest->stream->write(dest->buffer, datacount) != (vil_streampos)datacount)
      ERREXIT(cinfo, JERR_FILE_WRITE);
  }
}


//  * Prepare for output to a vil_stream.
//  * The caller must have already opened the stream, and is responsible
//  * for closing it after finishing compression.
void
vil_jpeg_stream_dst_set (j_compress_ptr cinfo, vil_stream *vs)
{
  // The destination object is made permanent so that multiple JPEG images
  // can be written to the same file without re-executing jpeg_stdio_dest.
  // This makes it dangerous to use this manager and a different destination
  // manager serially with the same JPEG object, because their private object
  // sizes may be different.  Caveat programmer.
  //
  assert(! cinfo->dest); // call this routine only once.

  // allocate
  vil_jpeg_dstptr dest = (vil_jpeg_dstptr)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo,
                                JPOOL_PERMANENT,
                                SIZEOF(vil_jpeg_stream_destination_mgr));
  cinfo->dest = reinterpret_cast<jpeg_destination_mgr *>(dest);

  // fill in methods in base
  dest->base.init_destination    = vil_jpeg_init_destination;
  dest->base.empty_output_buffer = vil_jpeg_empty_output_buffer;
  dest->base.term_destination    = vil_jpeg_term_destination;

  dest->stream = vs;
}

void
vil_jpeg_stream_dst_rewind(j_compress_ptr cinfo, vil_stream *vs)
{
  vil_jpeg_dstptr dst = ( vil_jpeg_dstptr )( cinfo->dest );
  { // verify
    assert(dst != 0);
    assert(dst->stream == vs);
  }

  cinfo->dest->next_output_byte = dst->buffer;
  cinfo->dest->free_in_buffer = vil_jpeg_OUTPUT_BUF_SIZE;

  vs->seek(0L);
}
