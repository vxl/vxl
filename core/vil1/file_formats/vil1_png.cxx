#ifdef __GNUC__
#pragma implementation
#endif

#include "vil_png.h"

#include <vcl_cassert.h>
#include <vcl_cstring.h>
#include <vcl_iostream.h>

#include <vil/vil_stream.h>
#include <vil/vil_image_impl.h>
#include <vil/vil_image.h>

#include <png.h>
#include <vcl_cstdlib.h> // for exit()

// http://www.mirror.ac.uk/sites/ftp.cdrom.com/pub/png/libpng.html

// Constants
#define SIG_CHECK_SIZE 4

char const* vil_png_format_tag = "png";

// Functions
static bool problem(char const* msg)
{
  vcl_cerr << "[vil_png: PROBLEM " <<msg << "]";
  return false;
}

vil_image_impl* vil_png_file_format::make_input_image(vil_stream* is)
{
  // Attempt to read header
  png_byte sig_buf [SIG_CHECK_SIZE];
  if (is->read(sig_buf, SIG_CHECK_SIZE) != SIG_CHECK_SIZE) {
    problem("Initial header fread");
    return 0;
  }

  if (png_sig_cmp (sig_buf, (png_size_t) 0, (png_size_t) SIG_CHECK_SIZE) != 0)
    return 0;

  return new vil_png_generic_image(is);
}

vil_image_impl* vil_png_file_format::make_output_image(vil_stream* is, int planes,
                                               int width,
                                               int height,
                                               int components,
                                               int bits_per_component,
                                               vil_component_format format)
{
  return new vil_png_generic_image(is, planes, width, height, components, bits_per_component, format);
}

char const* vil_png_file_format::tag() const
{
  return vil_png_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  vil_stream* f = (vil_stream*)png_get_io_ptr(png_ptr);
  f->read(data, length);
}

void user_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  vil_stream* f = (vil_stream*)png_get_io_ptr(png_ptr);
  f->write(data, length);
}

void user_flush_data(png_structp png_ptr)
{
  // IOFile* f = (IOFile*)png_get_io_ptr(png_ptr);
  // urk.  how to flush?
}

struct vil_jmpbuf_wrapper {
  jmp_buf jmpbuf;
} jmpbuf_wrapper;
static vil_jmpbuf_wrapper pngtopnm_jmpbuf_struct;
static bool jmpbuf_ok = false;

// Must be  a macro -- setjmp needs its stack frame to live
#define png_setjmp_on(ACTION) do {\
 jmpbuf_ok = true;\
 if (setjmp (pngtopnm_jmpbuf_struct.jmpbuf) != 0) {\
    problem("png_setjmp_on");\
    ACTION;\
 }\
} while (0);
#define png_setjmp_off() (jmpbuf_ok = false)

// this function, aside from the extra step of retrieving the "error
// pointer" (below) and the fact that it exists within the application
// rather than within libpng, is essentially identical to libpng's
// default error handler.  The second point is critical:  since both
// setjmp() and longjmp() are called from the same code, they are
// guaranteed to have compatible notions of how big a jmp_buf is,
// regardless of whether _BSD_SOURCE or anything else has (or has not)
// been defined.
//
static void pngtopnm_error_handler (png_structp png_ptr, png_const_charp msg)
{
  fprintf(stderr, "vil_png:  fatal libpng error: %s\n", msg);
  fflush(stderr);

  if (!jmpbuf_ok) {
    // Someone called the error handler when the setjmp was wrong
    vcl_cerr << "vil_png: jmpbuf is pretty far from ok.  returning\n";
    // abort();
    return;
  }

  vil_jmpbuf_wrapper  *jmpbuf_ptr = (vil_jmpbuf_wrapper*) png_get_error_ptr(png_ptr);
  if (jmpbuf_ptr == NULL) {         // we are completely hosed now
    fprintf(stderr,
      "pnmtopng:  EXTREMELY fatal error: jmpbuf unrecoverable; terminating.\n");
    fflush(stderr);
    exit(99);
  }

  longjmp(jmpbuf_ptr->jmpbuf, 1);
}

struct vil_png_structures {
  bool reading_;
  png_struct *png_ptr;
  png_info *info_ptr;
  png_byte **rows;
  int channels;
  bool ok;

  vil_png_structures(bool reading) {
    reading_ = reading;
    png_ptr = 0;
    info_ptr = 0;
    rows = 0;
    channels = 0;
    ok = false;

    png_setjmp_on(return);

    if (reading)
      png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, &pngtopnm_jmpbuf_struct, pngtopnm_error_handler, NULL);
    else
      png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, &pngtopnm_jmpbuf_struct, pngtopnm_error_handler, NULL);

    if (!png_ptr) {
      problem("cannot allocate LIBPNG structure");
      return;
    }

    info_ptr = png_create_info_struct (png_ptr);
    if (!info_ptr) {
      problem("cannot allocate LIBPNG structures");
      return;
    }

    ok = true;

    // Now jmpbuf is broken, hope noone calls png....
    png_setjmp_off();
  }

  bool alloc_image() {
    rows = new png_byte* [info_ptr->height];
    if (rows == 0)
      return (ok = problem("couldn't allocate space for image"));

    int linesize;
    if (info_ptr->bit_depth == 16)
      linesize = 2 * info_ptr->width;
    else
      linesize = info_ptr->width;

    if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
      linesize *= 2;
    else
      if (info_ptr->color_type == PNG_COLOR_TYPE_RGB)
        linesize *= 3;
      else
        if (info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
          linesize *= 4;

    // Alloc the whole thing at once
    rows[0] = new png_byte[linesize * info_ptr->height];
    if (!rows[0])
      return (ok = problem("couldn't allocate space for image"));

    // Re-point rows.
    for (unsigned int y = 1 ; y < info_ptr->height ; ++y)
      rows[y] = rows[0] + y * linesize;

    return true;
  }

  png_byte** get_rows() {
    if (reading_) {
      if (!rows) {
        if (alloc_image()) {
          png_setjmp_on(return 0);
          png_read_image (png_ptr, rows);
          png_read_end (png_ptr, info_ptr);
          png_setjmp_off();
        }
      }
    } else {
      assert(rows);
    }

    return rows;
  }

  ~vil_png_structures() {
    png_setjmp_on(goto del);
    if (reading_) {
      // Reading -- just delete
      png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp)NULL);

    } else {
      // Writing -- save the rows
      png_write_image(png_ptr, rows);
      png_write_end(png_ptr, info_ptr);

      png_destroy_write_struct (&png_ptr, &info_ptr);
    }
    png_setjmp_off();

  del:
    if (rows) {
      delete [] rows[0];
      delete [] rows;
    }
  }
};


/////////////////////////////////////////////////////////////////////////////

vil_png_generic_image::vil_png_generic_image(vil_stream* is):
  vs_(is),
  p(new vil_png_structures(true))
{
  vs_->ref();
  read_header();
}

vil_png_generic_image::vil_png_generic_image(vil_stream* is, int planes,
                                               int width,
                                               int height,
                                               int components,
                                               int bits_per_component,
                                               vil_component_format format):
  vs_(is),
  width_(width),
  height_(height),
  components_(components),
  bits_per_component_(bits_per_component),
  p(new vil_png_structures(false))
{
  vs_->ref();
  write_header();
}

vil_png_generic_image::~vil_png_generic_image()
{
  delete p;
  vs_->unref();
}

char const* vil_png_generic_image::file_format() const
{
  return vil_png_format_tag;
}

bool vil_png_generic_image::read_header()
{
  if (!p->ok)
    return false;

  png_setjmp_on(return false);

  vs_->seek(0);
  png_byte sig_buf [SIG_CHECK_SIZE];
  if (vs_->read(sig_buf, SIG_CHECK_SIZE) != SIG_CHECK_SIZE) {
    png_setjmp_off();
    return problem("Initial header fread");
  }

  if (png_sig_cmp (sig_buf, (png_size_t) 0, (png_size_t) SIG_CHECK_SIZE) != 0) {
    png_setjmp_off();
    return problem("png_sig_cmp");
  }

  png_set_read_fn(p->png_ptr, vs_, user_read_data);
  png_set_sig_bytes (p->png_ptr, SIG_CHECK_SIZE);
  png_read_info (p->png_ptr, p->info_ptr);

  if (p->info_ptr->bit_depth < 8)
    png_set_packing (p->png_ptr);

  // int maxval;
  // if (p->info_ptr->color_type == PNG_COLOR_TYPE_PALETTE) {
  //   maxval = 255;
  // } else {
  //   maxval = (1l << p->info_ptr->bit_depth) - 1;
  // }

  p->channels = png_get_channels(p->png_ptr, p->info_ptr);

  this->width_ = p->info_ptr->width;
  this->height_ = p->info_ptr->height;
  this->components_ = p->channels;
  this->bits_per_component_ = p->info_ptr->bit_depth;

  if (p->info_ptr->valid & PNG_INFO_sBIT) problem("LAZY AWF! PNG_INFO_sBIT");

  // if (p->info_ptr->valid & PNG_INFO_bKGD) problem("LAZY AWF! PNG_INFO_bKGD");
  png_setjmp_off();
  return true;
}

bool vil_png_generic_image::write_header()
{
  if (!p->ok)
    return false;

  png_setjmp_on( return false );

  vs_->seek(0);

  png_set_write_fn(p->png_ptr, vs_, user_write_data, user_flush_data);

  int color_type;
  if (components_ == 3)
    color_type = PNG_COLOR_TYPE_RGB;
  else
    color_type = PNG_COLOR_TYPE_GRAY;

  png_set_IHDR(p->png_ptr, p->info_ptr, width_, height_, bits_per_component_, color_type,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  png_write_info(p->png_ptr, p->info_ptr);

  // Make memory image
  p->channels = components_;
  p->alloc_image();

  png_setjmp_off();

  return true;
}

bool vil_png_generic_image::get_section(void* buf, int x0, int y0, int xs, int ys) const
{
  if (!p->ok)
    return false;

  // PNG lib wants everything in memory -- the first get_rows reads the whole image.
  png_byte** rows = p->get_rows();
  if (!rows) return 0;

  int bytes_per_pixel = p->info_ptr->bit_depth * p->channels / 8;
  int bytes_per_row_dst = xs*bytes_per_pixel;
  if ((unsigned int)xs == p->info_ptr->width) {
    assert(x0 == 0);
    memcpy(buf, rows[y0], ys * bytes_per_row_dst);
  }
  else {
    png_byte* dst = (png_byte*)buf;
    for(int y = 0; y < ys; ++y, dst += bytes_per_row_dst)
      memcpy(dst, &rows[y0+y][x0], xs);
  }

  return true;
}

bool vil_png_generic_image::put_section(void const* buf, int x0, int y0, int xs, int ys)
{
  if (!p->ok)
    return false;

  // PNG lib wants everything in memory -- the writing isn't done till this image is deleted.

  png_byte** rows = p->get_rows();
  if (!rows) return false;

  int bytes_per_pixel  = p->info_ptr->bit_depth*p->channels / 8;
  int bytes_per_row_dst = xs*bytes_per_pixel;
  if ((unsigned int)xs == p->info_ptr->width) {
    assert(x0 == 0);
    memcpy(rows[y0], buf, ys * bytes_per_row_dst);
  }
  else {
    png_byte* dst = (png_byte*)buf;
    for(int y = 0; y < ys; ++y, dst += bytes_per_row_dst)
      memcpy(&rows[y0+y][x0], dst, xs);
  }

  return true;
}

vil_image vil_png_generic_image::get_plane(int plane) const
{
  assert(plane == 0);
  return const_cast<vil_png_generic_image*>(this);
}
