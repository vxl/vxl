/*
  fsm@robots.ox.ac.uk
*/
// Created: 17 Feb 2000
#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_jpeg.h"

#include <vcl/vcl_cassert.h>
#include <vcl/vcl_cstdio.h>
#include <vcl/vcl_climits.h> // CHAR_BIT
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_vector.h>

#include <vil/vil_stream.h>
#include <vil/vil_image.h>

/*
 * In ANSI C, and indeed any rational implementation, size_t is also the
 * type returned by sizeof().  However, it seems there are some irrational
 * implementations out there, in which sizeof() returns an int even though
 * size_t is defined as long or unsigned long.  To ensure consistent results
 * we always use this SIZEOF() macro in place of using sizeof() directly.
 */

#define SIZEOF(object)	((size_t) sizeof(object))

//: the file probe, as a C function.
bool vil_jpeg_file_probe(vil_stream *vs) {
  char magic[2];
  vs->seek(0);
  int n = vs->read(magic, sizeof(magic));
  
  if (n != sizeof(magic)) {
    cerr << __FILE__ << " : vil_stream::read() failed" << endl;
    return false;
  }
  
  // 0xFF followed by 0xD8  
  return ( (magic[0] == char(0xFF)) && (magic[1] == char(0xD8)) );
}

// static data
static char const jpeg_string[] = "jpeg";

//--------------------------------------------------------------------------------
// class vil_jpeg_file_format

char const* vil_jpeg_file_format::tag() const {
  return jpeg_string;
}

//:
vil_image_impl *vil_jpeg_file_format::make_input_image(vil_stream *vs) {
  return vil_jpeg_file_probe(vs) ? new vil_jpeg_generic_image(vs) : 0;
}

vil_image_impl *vil_jpeg_file_format::make_output_image(vil_stream *vs,
							   int planes,
							   int width,
							   int height,
							   int components,
							   int bits_per_component,
							   vil_component_format format)
{
  return new vil_jpeg_generic_image(vs, planes, width, height, components, bits_per_component, format);
}

//--------------------------------------------------------------------------------

// Implement a jpeg_source_manager for vil_stream *.
// Adapted by fsm from the FILE * version in jdatasrc.c

#define vil_jpeg_INPUT_BUF_SIZE  4096	/* choose an efficiently fread'able size */

//: this is the data source structure which allows JPEG to read from a vil_stream.
struct vil_jpeg_stream_source_mgr {
  struct jpeg_source_mgr base;
  
  vil_stream *stream;           /* source stream */
  JOCTET * buffer;              /* start of buffer */
  jpeg_boolean start_of_file;   /* have we gotten any data yet? */
};
typedef vil_jpeg_stream_source_mgr *vil_jpeg_srcptr;


// * Initialize source --- called by jpeg_read_header
// * before any data is actually read.
static
void
vil_jpeg_init_source (j_decompress_ptr cinfo) {
  //cerr << "vil_jpeg_init_source()" << endl;

  vil_jpeg_srcptr src = ( vil_jpeg_srcptr )( cinfo->src );
  
  /* We reset the empty-input-file flag for each image,
   * but we don't clear the input buffer.
   * This is correct behavior for reading a series of images from one source.
   */
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
static
jpeg_boolean
vil_jpeg_fill_input_buffer (j_decompress_ptr cinfo) {
  vil_jpeg_srcptr src = ( vil_jpeg_srcptr )( cinfo->src );

  size_t nbytes = src->stream->read(src->buffer, vil_jpeg_INPUT_BUF_SIZE);

  if (nbytes <= 0) {
    if (src->start_of_file)	/* Treat empty input file as fatal error */
      ERREXIT(cinfo, JERR_INPUT_EMPTY);
    WARNMS(cinfo, JWRN_JPEG_EOF);
    /* Insert a fake EOI marker */
    src->buffer[0] = (JOCTET) 0xFF;
    src->buffer[1] = (JOCTET) JPEG_EOI;
    nbytes = 2;
  }

  src->base.next_input_byte = src->buffer;
  src->base.bytes_in_buffer = nbytes;
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
static
void
vil_jpeg_skip_input_data (j_decompress_ptr cinfo, long num_bytes) {
  vil_jpeg_srcptr src = ( vil_jpeg_srcptr )( cinfo->src );
  
  /* Just a dumb implementation for now.  Could use fseek() except
   * it doesn't work on pipes.  Not clear that being smart is worth
   * any trouble anyway --- large skips are infrequent.
   */
  if (num_bytes > 0) {
    while (num_bytes > (long) src->base.bytes_in_buffer) {
      num_bytes -= (long) src->base.bytes_in_buffer;
      vil_jpeg_fill_input_buffer(cinfo);
      /* note we assume that fill_input_buffer will never return FALSE,
       * so suspension need not be handled.
       */
    }
    src->base.next_input_byte += (size_t) num_bytes;
    src->base.bytes_in_buffer -= (size_t) num_bytes;
  }
}


//  * Terminate source --- called by jpeg_finish_decompress
//  * after all data has been read.  Often a no-op.
//  *
//  * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
//  * application must deal with any cleanup that should happen even
//  * for error exit.
static
void
vil_jpeg_term_source (j_decompress_ptr cinfo) {
  cerr << "vil_jpeg_term_source()" << endl;
  /* no work necessary here */
}

static
void
vil_jpeg_stream_src_set (j_decompress_ptr cinfo, vil_stream *vs) {
  /* The source object and input buffer are made permanent so that a series
   * of JPEG images can be read from the same file by calling vil_jpeg_stream_src
   * only before the first one.  (If we discarded the buffer at the end of
   * one image, we'd likely lose the start of the next one.)
   * This makes it unsafe to use this manager and a different source
   * manager serially with the same JPEG object.  Caveat programmer.
   */
  vil_jpeg_srcptr src = ( vil_jpeg_srcptr )( cinfo->src );

  assert(! src); // this function must be called only once on each cinfo.

  //cerr << "vil_jpeg_stream_src() : creating new data source" << endl;
  
  src = (vil_jpeg_srcptr) // allocate
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, 
				JPOOL_PERMANENT,
				SIZEOF(vil_jpeg_stream_source_mgr));
  // set pointer in cinfo
  cinfo->src = (struct jpeg_source_mgr *) src;
  
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
  src->base.resync_to_restart =     jpeg_resync_to_restart; /* use default method */
  src->base.term_source       = vil_jpeg_term_source;
}

static
void 
vil_jpeg_stream_src_rewind(j_decompress_ptr cinfo, vil_stream *vs) { 
  { // verify
    vil_jpeg_srcptr src = ( vil_jpeg_srcptr )( cinfo->src );
    assert(src);
    assert(src->stream == vs);
  }
  
  cinfo->src->bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
  cinfo->src->next_input_byte = 0; /* until buffer loaded */
  
  vs->seek(0);
}

//--------------------------------------------------------------------------------

// Implement a jpeg_destination_manager for vil_stream *.
// Adapted by fsm from the FILE * version in jdatadst.c

#define vil_jpeg_OUTPUT_BUF_SIZE  4096	/* choose an efficiently fwrite'able size */

//: this is the data source structure which allows JPEG to write to a vil_stream.
struct vil_jpeg_stream_destination_mgr {
  struct jpeg_destination_mgr base;
  
  vil_stream *stream;           /* target stream */
  JOCTET * buffer;              /* start of buffer */
};
typedef vil_jpeg_stream_destination_mgr *vil_jpeg_dstptr;


//  * Initialize destination --- called by jpeg_start_compress
//  * before any data is actually written.
static
void
vil_jpeg_init_destination (j_compress_ptr cinfo) {
  vil_jpeg_dstptr dest = (vil_jpeg_dstptr) cinfo->dest; // cast to derived class
  
  /* Allocate the output buffer --- it will be released when done with image */
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
static
jpeg_boolean
vil_jpeg_empty_output_buffer (j_compress_ptr cinfo) {
  vil_jpeg_dstptr dest = (vil_jpeg_dstptr) cinfo->dest; // cast to derived class
  
  if (dest->stream->write(dest->buffer, vil_jpeg_OUTPUT_BUF_SIZE) != (size_t) vil_jpeg_OUTPUT_BUF_SIZE)
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
static
void
vil_jpeg_term_destination (j_compress_ptr cinfo) {
  vil_jpeg_dstptr dest = (vil_jpeg_dstptr) cinfo->dest; // cast to derived class
  size_t datacount = vil_jpeg_OUTPUT_BUF_SIZE - dest->base.free_in_buffer;
  
  /* Write any data remaining in the buffer */
  if (datacount > 0) {
    if (dest->stream->write(dest->buffer, datacount) != datacount)
      ERREXIT(cinfo, JERR_FILE_WRITE);
  }
}


//  * Prepare for output to a vil_stream.
//  * The caller must have already opened the stream, and is responsible
//  * for closing it after finishing compression.
static
void
vil_jpeg_stream_dst_set (j_compress_ptr cinfo, vil_stream *vs) {
  /* The destination object is made permanent so that multiple JPEG images
   * can be written to the same file without re-executing jpeg_stdio_dest.
   * This makes it dangerous to use this manager and a different destination
   * manager serially with the same JPEG object, because their private object
   * sizes may be different.  Caveat programmer.
   */
  assert(! cinfo->dest); // call this routine only once.

  // allocate
  vil_jpeg_dstptr dest = (vil_jpeg_dstptr) 
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, 
				JPOOL_PERMANENT,
				SIZEOF(vil_jpeg_stream_destination_mgr));
  cinfo->dest = ( jpeg_destination_mgr *) dest;
  
  // fill in methods in base
  dest->base.init_destination    = vil_jpeg_init_destination;
  dest->base.empty_output_buffer = vil_jpeg_empty_output_buffer;
  dest->base.term_destination    = vil_jpeg_term_destination;

  dest->stream = vs;
}

static
void 
vil_jpeg_stream_dst_rewind(j_compress_ptr cinfo, vil_stream *vs) { 
  vil_jpeg_dstptr dst = ( vil_jpeg_dstptr )( cinfo->dest );
  { // verify
    assert(dst);
    assert(dst->stream == vs);
  }
  
  cinfo->dest->next_output_byte = dst->buffer;
  cinfo->dest->free_in_buffer = vil_jpeg_OUTPUT_BUF_SIZE;
  
  vs->seek(0);
}

//--------------------------------------------------------------------------------
// class vil_jpeg_generic_image

// using jpeg decompressor objects :
// 0. supply an error manager, eg with jpeg_std_err().
//    this *must* be done before initializing the object.
// 1. initialize with jpeg_create_decompress().
// 2. supply a data stream, eg with jpeg_std_source().
// 3. call jpeg_read_header() to start reading the data stream. this will read
//    to the start of the compressed data and store various tables and parameters.
//    if you just want the image parameters and not the data, it's ok to stop 
//    now, so long as you call jpeg_abort_decompress() or jpeg_destroy_decompress()
//    to release resources.
// 4. call jpeg_finish_decompress() if you read all the data. if you only read
//    some of the data, call jpeg_abort_decompress().
//
// 5. destruct the object with jpeg_destroy_decompress().

vil_jpeg_generic_image::vil_jpeg_generic_image(vil_stream *is_) 
  : is_reader(true) 
  , stream(is_)
{
  // setup the standard error handler in the jpeg library
  cinfo_d.err = jpeg_std_error(&jerr);
  
  // construct the decompression object :
  jpeg_create_decompress(&cinfo_d);

  // we need to read the header here, in order to get parameters such as size.
  
  // set the data source
  vil_jpeg_stream_src_set(&cinfo_d, stream);
  
  // rewind the stream
  vil_jpeg_stream_src_rewind(&cinfo_d, stream);
  
  // now we may read the header.
  jpeg_read_header(&cinfo_d, TRUE);
  
  // this seem to be necessary :
#if 1
  // bogus decompression to get image parameters.
  jpeg_start_decompress(&cinfo_d);
  
  // this aborts the decompression, but doesn't destroy the object.
  jpeg_abort_decompress(&cinfo_d);
#endif
}

vil_jpeg_generic_image::vil_jpeg_generic_image(vil_stream *is,
					       int planes,
					       int width,
					       int height,
					       int components,
					       int bits_per_component,
					       vil_component_format format)
  : is_reader(false)
  , stream(is)
{ 
  // setup the standard error handler in the jpeg library
  cinfo_c.err = jpeg_std_error(&jerr);

  // Zero just in case..
  cinfo_c.next_scanline = 0;

  // construct the compression object :
  jpeg_create_compress(&cinfo_c);

  // set the data destination
  vil_jpeg_stream_dst_set(&cinfo_c, stream);

  // rewind the stream
  vil_jpeg_stream_dst_rewind(&cinfo_c, stream);

  // warn
  if (planes != 1)
    cerr << __FILE__ " : prototype has != 1 planes. ignored" << endl;

  // use same number of components as prototype, obviously.
  cinfo_c.input_components = components;

  // store size
  cinfo_c.image_width = width;
  cinfo_c.image_height = height;
  //cerr << "w h = " << cinfo_c.image_width << ' ' << cinfo_c.image_height << endl;
  
  // remember this for later.
  proto_bits_per_component = bits_per_component;
}

vil_jpeg_generic_image::~vil_jpeg_generic_image() {
  //cerr << __FILE__ " : dtor" << endl;

  // FIXME: I suspect there's a core leak here because jpeg_destroy() does not
  // free the vil_jpeg_stream_source_mgr allocated in vil_jpeg_stream_*_set()
  if (is_reader) {
    // destroy the decompression object
    jpeg_destroy_decompress(&cinfo_d);
  }
  else {
    // destroy the compression object
    jpeg_destroy_compress(&cinfo_c);
  }

 // destroy the stream, as we're supposed to do.  
  delete stream;
  stream = 0;
}



//: decompressing from the vil_stream to a section buffer.
bool vil_jpeg_generic_image::decompress_section(void *buf, int x0, int y0, int w, int h) {
  assert( is_reader);
  //cerr << "decompress_section " << buf << endl;

  // rewind stream
  vil_jpeg_stream_src_rewind(&cinfo_d, stream);
  
  // read header
  jpeg_read_header(&cinfo_d, TRUE);

  // start decompression
  jpeg_start_decompress(&cinfo_d);
  
  // make a sample array of height 1.
  // NB1 'row_size' is the size of the rows in the image.
  // NB2 a JSAMPARRAY is just an array of pointers to rasters of JSAMPLEs.
  // NB3 a JSAMPLE is an 8 or 12 bit unsigned integers.
  unsigned row_size = cinfo_d.output_width * cinfo_d.output_components;
  JSAMPARRAY buffer = (*cinfo_d.mem->alloc_sarray) ((j_common_ptr) &cinfo_d, 
						  JPOOL_IMAGE, 
						  row_size, // columns
						  1);       // rows
  
  // number of bytes per pixel
  unsigned bpp = cinfo_d.output_components;
  
  // skip the first y0 rows of the image :
  for (int i=0; i<y0; ++i)
    if (jpeg_read_scanlines(&cinfo_d, buffer, 1) != 1)
      goto return_false;
  
  // read each of the next h rows and copy the relevant w columns out :
  for (int i=0; i<h; ++i)
    if (jpeg_read_scanlines(&cinfo_d, buffer, 1) != 1)
      goto return_false;
    else
      memcpy(static_cast<char*>(buf) + i*w*bpp, &buffer[0][x0*bpp], w*bpp);
  
  goto return_true;

  //----------------------------------------
#ifndef VCL_SGI_CC
  assert(false); // don't fall through to here.
#endif

  // hack to make sure the scanline that we created is freed.
  // we also abort the decompression.
  // essentially, this amounts to calling destructors.
  
 return_true:
  jpeg_abort_decompress(&cinfo_d);
  (*cinfo_d.mem->free_pool) ((j_common_ptr) &cinfo_d, JPOOL_IMAGE);
  return true;
 return_false:
  jpeg_abort_decompress(&cinfo_d);
  (*cinfo_d.mem->free_pool) ((j_common_ptr) &cinfo_d, JPOOL_IMAGE);
  return false;  
}

//: compressing a section onto the vil_stream.
bool vil_jpeg_generic_image::compress_section(void const *buf, int x0, int y0, int w, int h) {
  assert(!is_reader);

  // "compression makes no sense unless the section covers the whole image."
  // Relaxed slightly.. awf.
  // It will work if you send entire scan lines sequentially

  if (x0 != 0 || w != cinfo_c.image_width) {
    cerr << __FILE__ << " : Can only compress complete scanlines, sent sequentially " << endl;
    return false;
  }

  int next_scanline = cinfo_c.next_scanline;
  
  if (y0 != next_scanline) {
    cerr << __FILE__ << " : Can only compress complete scanlines, sent sequentially " << endl;
    return false;
  }

  if (next_scanline == 0) {
    // set colorspace of input image. FIXME.
    switch (cinfo_c.input_components) {
    case 1:
      cinfo_c.in_color_space = JCS_GRAYSCALE;
      break;
    case 3:
      cinfo_c.in_color_space = JCS_RGB;
      break;
    default:
      cerr << __FILE__ " : urgh!" << endl;
      return false;
    }

    jpeg_set_defaults(&cinfo_c);
    
    // start compression
    bool write_all_tables = true;
    jpeg_start_compress (&cinfo_c, write_all_tables);
  }
  
  // make an array of raster pointers :
  vcl_vector<JSAMPLE *> rasters(h); //*rasters = neXw JSAMPLE*[h];

  // bytes per pixel in the section
  assert(proto_bits_per_component == CHAR_BIT); // FIXME.
  unsigned bpp = cinfo_c.input_components;
  
  // fill it in :
  for (int y=0; y<h; ++y)
    rasters[y] = (JSAMPLE*) ((char const*)buf + y*w*bpp);
  
  // write it
  jpeg_write_scanlines(&cinfo_c, rasters.begin(), h);
  
  // delete array again
  //deletXe [] rasters;

  if (y0 + h == cinfo_c.image_height) {
    // Finish when the last scanline was written
    jpeg_finish_compress(&cinfo_c);
  }


  return true;
}

//:
int vil_jpeg_generic_image::planes() const {
  return 1; 
}

//:
int vil_jpeg_generic_image::width() const { 
  if (is_reader)
    return cinfo_d.output_width;
  else
    return cinfo_c.image_width;
}

//:
int vil_jpeg_generic_image::height() const {
  if (is_reader)
    return cinfo_d.output_height;
  else
    return cinfo_c.image_height;
}

//:
int vil_jpeg_generic_image::components() const {
  if (is_reader)
    return cinfo_d.output_components;
  else
    return cinfo_c.input_components;
}

//:  
int vil_jpeg_generic_image::bits_per_component() const {
  return CHAR_BIT;
}

//:
vil_component_format vil_jpeg_generic_image::component_format() const {
  return VIL_COMPONENT_FORMAT_UNSIGNED_INT; 
}

//: assume only one plane
vil_image vil_jpeg_generic_image::get_plane(int i) const {
  assert(i == 0);
  return const_cast<vil_jpeg_generic_image*>( this );
}

//:  
char const *vil_jpeg_generic_image::file_format() const {
  return jpeg_string;
}

//: gets section, by calling decompress_section.
// If you want a jpeg image which caches blocks of the image, put it into a block_cache.
bool vil_jpeg_generic_image::get_section(void *buf, int x0, int y0, int w, int h) const {
  if (!is_reader) {
    cerr << "attempted put_section() on input jpeg image" << endl;
    return false;
  }
  return const_cast<vil_jpeg_generic_image*>(this)->decompress_section(buf, x0, y0, w, h);
}

//: puts section, by calling compress_section.
bool vil_jpeg_generic_image::put_section(void const *buf, int x0, int y0, int w, int h) {
  if ( is_reader) {
    cerr << "attempted get_section() on output image" << endl;
    return false;
  }
  return const_cast<vil_jpeg_generic_image*>(this)->compress_section(buf, x0, y0, w, h);
}

//--------------------------------------------------------------------------------
