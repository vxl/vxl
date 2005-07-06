// This is core/vil/file_formats/vil_jpeg_source_mgr.cxx
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

#include "vil_jpeg_source_mgr.h"
#include <vcl_cassert.h>
#include <vcl_cstddef.h> // for vcl_size_t
#include <vcl_limits.h> //for vcl_numeric_limits
#include <vil/vil_stream.h>

#define STATIC /*static*/

// In ANSI C, and indeed any rational implementation, vcl_size_t is also the
// type returned by sizeof().  However, it seems there are some irrational
// implementations out there, in which sizeof() returns an int even though
// vcl_size_t is defined as long or unsigned long.  To ensure consistent results
// we always use this SIZEOF() macro in place of using sizeof() directly.
//
#define SIZEOF(object) ((vcl_size_t) sizeof(object))

// Implement a jpeg_source_manager for vil_stream *.
// Adapted by fsm from the FILE * version in jdatasrc.c

#define vil_jpeg_INPUT_BUF_SIZE  4096 // choose an efficiently fread'able size
typedef vil_jpeg_stream_source_mgr *vil_jpeg_srcptr;


// * Initialize source --- called by jpeg_read_header
// * before any data is actually read.
STATIC
void
vil_jpeg_init_source (j_decompress_ptr cinfo)
{
  //vcl_cerr << "vil_jpeg_init_source()\n";

  vil_jpeg_srcptr src = ( vil_jpeg_srcptr )( cinfo->src );

  // We reset the empty-input-file flag for each image,
  // but we don't clear the input buffer.
  // This is correct behavior for reading a series of images from one source.
  src->start_of_file = TRUE;
}

//  * Fill the input buffer --- called whenever buffer is emptied.
//  *
//  * In typical applications, this should read fresh data into the buffer
//  * (ignoring the current state of next_input_byte & bytes_in_buffer),
//  * reset the pointer & count to the start of the buffer, and return TRUE
//  * indicating that the buffer has been reloaded.  It is not necessary to
//  * fill the buffer entirely, only to obtain at least one more byte.
//  *
//  * There is no such thing as an EOF return.  If the end of the file has been
//  * reached, the routine has a choice of ERREXIT() or inserting fake data into
//  * the buffer.  In most cases, generating a warning message and inserting a
//  * fake EOI marker is the best course of action --- this will allow the
//  * decompressor to output however much of the image is there.  However,
//  * the resulting error message is misleading if the real problem is an empty
//  * input file, so we handle that case specially.
//  *
//  * In applications that need to be able to suspend compression due to input
//  * not being available yet, a FALSE return indicates that no more data can be
//  * obtained right now, but more may be forthcoming later.  In this situation,
//  * the decompressor will return to its caller (with an indication of the
//  * number of scanlines it has read, if any).  The application should resume
//  * decompression after it has loaded more data into the input buffer.  Note
//  * that there are substantial restrictions on the use of suspension --- see
//  * the documentation.
//  *
//  * When suspending, the decompressor will back up to a convenient restart point
//  * (typically the start of the current MCU). next_input_byte & bytes_in_buffer
//  * indicate where the restart point will be if the current call returns FALSE.
//  * Data beyond this point must be rescanned after resumption, so move it to
//  * the front of the buffer rather than discarding it.
STATIC
jpeg_boolean
vil_jpeg_fill_input_buffer (j_decompress_ptr cinfo)
{
  vil_jpeg_srcptr src = ( vil_jpeg_srcptr )( cinfo->src );

  vil_streampos nbytes = src->stream->read(src->buffer, vil_jpeg_INPUT_BUF_SIZE);

  if (nbytes <= 0) {
    if (src->start_of_file) // Treat empty input file as fatal error
      ERREXIT(cinfo, JERR_INPUT_EMPTY);
    WARNMS(cinfo, JWRN_JPEG_EOF);
    // Insert a fake EOI marker
    src->buffer[0] = (JOCTET) 0xFF;
    src->buffer[1] = (JOCTET) JPEG_EOI;
    nbytes = 2;
  }

  src->base.next_input_byte = src->buffer;
  //original JPEG (non-j2k) files aren't usually very big
  //so this assert should be no problem
  assert( nbytes < vcl_numeric_limits< size_t >::max() );
  src->base.bytes_in_buffer = (size_t)nbytes;
  src->start_of_file = FALSE;

  return TRUE;
}

//  * Skip data --- used to skip over a potentially large amount of
//  * uninteresting data (such as an APPn marker).
//  *
//  * Writers of suspendable-input applications must note that skip_input_data
//  * is not granted the right to give a suspension return.  If the skip extends
//  * beyond the data currently in the buffer, the buffer can be marked empty so
//  * that the next read will cause a fill_input_buffer call that can suspend.
//  * Arranging for additional bytes to be discarded before reloading the input
//  * buffer is the application writer's problem.
STATIC
void
vil_jpeg_skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
  vil_jpeg_srcptr src = ( vil_jpeg_srcptr )( cinfo->src );

  // Just a dumb implementation for now.  Could use fseek() except
  // it doesn't work on pipes.  Not clear that being smart is worth
  // any trouble anyway --- large skips are infrequent.
  //
  if (num_bytes > 0) {
    while (num_bytes > (long) src->base.bytes_in_buffer) {
      num_bytes -= (long) src->base.bytes_in_buffer;
      vil_jpeg_fill_input_buffer(cinfo);
      // note we assume that fill_input_buffer will never return FALSE,
      // so suspension need not be handled.
    }
    src->base.next_input_byte += (vcl_size_t) num_bytes;
    src->base.bytes_in_buffer -= (vcl_size_t) num_bytes;
  }
}


//  * Terminate source --- called by jpeg_finish_decompress
//  * after all data has been read.  Often a no-op.
//  *
//  * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
//  * application must deal with any cleanup that should happen even
//  * for error exit.
STATIC
void
vil_jpeg_term_source (j_decompress_ptr /*cinfo*/)
{
  // no work necessary here
}

STATIC
void
vil_jpeg_stream_src_set (j_decompress_ptr cinfo, vil_stream *vs)
{
  // The source object and input buffer are made permanent so that a series
  // of JPEG images can be read from the same file by calling vil_jpeg_stream_src
  // only before the first one.  (If we discarded the buffer at the end of
  // one image, we'd likely lose the start of the next one.)
  // This makes it unsafe to use this manager and a different source
  // manager serially with the same JPEG object.  Caveat programmer.
  vil_jpeg_srcptr src = ( vil_jpeg_srcptr )( cinfo->src );

  assert(! src); // this function must be called only once on each cinfo.

  //vcl_cerr << "vil_jpeg_stream_src() : creating new data source\n";

  src = (vil_jpeg_srcptr) // allocate
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo,
                                JPOOL_PERMANENT,
                                SIZEOF(vil_jpeg_stream_source_mgr));
  // set pointer in cinfo
  cinfo->src = reinterpret_cast<struct jpeg_source_mgr *>(src);

  // set fields in src :
  src->stream = vs;

  src->buffer = (JOCTET *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo,
                                JPOOL_PERMANENT,
                                vil_jpeg_INPUT_BUF_SIZE * SIZEOF(JOCTET));

  src->start_of_file = TRUE;

  // fill in methods in base class :
  src->base.init_source       = vil_jpeg_init_source;
  src->base.fill_input_buffer = vil_jpeg_fill_input_buffer;
  src->base.skip_input_data   = vil_jpeg_skip_input_data;
  src->base.resync_to_restart =     jpeg_resync_to_restart; // use default method
  src->base.term_source       = vil_jpeg_term_source;
}

STATIC
void
vil_jpeg_stream_src_rewind(j_decompress_ptr cinfo, vil_stream *vs)
{
  { // verify
    vil_jpeg_srcptr src = ( vil_jpeg_srcptr )( cinfo->src );
    assert(src != 0);
    assert(src->stream == vs);
  }

  cinfo->src->bytes_in_buffer = 0; // forces fill_input_buffer on first read
  cinfo->src->next_input_byte = 0; // until buffer loaded

  vs->seek(0L);
}

