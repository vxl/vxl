// This is core/vil1/file_formats/vil1_png.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// http://www.mirror.ac.uk/sites/ftp.cdrom.com/pub/png/libpng.html

#include "vil1_png.h"

#include <vcl_cassert.h>
#include <vcl_cstring.h>
#include <vcl_iostream.h>

#include <vil1/vil1_stream.h>
#include <vil1/vil1_image_impl.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_property.h>

#include <png.h>
#if (PNG_LIBPNG_VER_MAJOR == 0)
extern "You need a later libpng. You should rerun CMake, after setting VXL_FORCE_V3P_PNG to ON."
#endif
#include <vcl_cstdlib.h> // for vcl_exit()

#include <vxl_config.h>

// Constants
#define SIG_CHECK_SIZE 4

char const* vil1_png_format_tag = "png";

// Functions
static bool problem(char const* msg)
{
  vcl_cerr << "[vil1_png: PROBLEM " <<msg << ']';
  return false;
}

vil1_image_impl* vil1_png_file_format::make_input_image(vil1_stream* is)
{
  // Attempt to read header
  png_byte sig_buf [SIG_CHECK_SIZE];
  if (is->read(sig_buf, SIG_CHECK_SIZE) != SIG_CHECK_SIZE) {
    problem("Initial header fread");
    return 0;
  }

  if (png_sig_cmp (sig_buf, (png_size_t) 0, (png_size_t) SIG_CHECK_SIZE) != 0)
    return 0;

  return new vil1_png_generic_image(is);
}

vil1_image_impl* vil1_png_file_format::make_output_image(vil1_stream* is, int planes,
                                                         int width,
                                                         int height,
                                                         int components,
                                                         int bits_per_component,
                                                         vil1_component_format format)
{
  return new vil1_png_generic_image(is, planes, width, height, components, bits_per_component, format);
}

char const* vil1_png_file_format::tag() const
{
  return vil1_png_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

static void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  vil1_stream* f = (vil1_stream*)png_get_io_ptr(png_ptr);
  f->read(data, length);
}

static void user_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  vil1_stream* f = (vil1_stream*)png_get_io_ptr(png_ptr);
  f->write(data, length);
}

static void user_flush_data(png_structp /*png_ptr*/)
{
  vcl_cerr << "vil1_png_generic_image::user_flush_data() NYI\n";
#if 0 // NYI
  IOFile* f = (IOFile*)png_get_io_ptr(png_ptr);
  // urk.  how to flush?
#endif
}

struct vil1_jmpbuf_wrapper
{
  jmp_buf jmpbuf;
};
static vil1_jmpbuf_wrapper pngtopnm_jmpbuf_struct;
static bool jmpbuf_ok = false;

// Must be  a macro - setjmp needs its stack frame to live
#define png_setjmp_on(ACTION) \
  do {\
    jmpbuf_ok = true;\
    if (setjmp (pngtopnm_jmpbuf_struct.jmpbuf) != 0) {\
      problem("png_setjmp_on");\
      ACTION;\
    }\
  } while (false);
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
  vcl_cerr << "vil1_png:  fatal libpng error: " << msg << '\n';

  if (!jmpbuf_ok) {
    // Someone called the error handler when the setjmp was wrong
    vcl_cerr << "vil1_png: jmpbuf is pretty far from ok.  returning\n";
    // vcl_abort();
    return;
  }

  vil1_jmpbuf_wrapper  *jmpbuf_ptr = (vil1_jmpbuf_wrapper*) png_get_error_ptr(png_ptr);
  if (jmpbuf_ptr == NULL) {         // we are completely hosed now
    vcl_cerr << "pnmtopng:  EXTREMELY fatal error: jmpbuf unrecoverable; terminating.\n";
    vcl_exit(99);
  }

  longjmp(jmpbuf_ptr->jmpbuf, 1);
}

struct vil1_png_structures
{
  bool reading_;
  png_struct *png_ptr;
  png_info *info_ptr;
  png_byte **rows;
  int channels;
  bool ok;

  vil1_png_structures(bool reading)
  {
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

  bool alloc_image()
  {
    rows = new png_byte* [info_ptr->height];
    if (rows == 0)
      return ok = problem("couldn't allocate space for image");

    unsigned long linesize;
    if (png_get_bit_depth( png_ptr, info_ptr ) == 16)
      linesize = 2 * info_ptr->width;
    else
      linesize = info_ptr->width;

    if (png_get_color_type( png_ptr, info_ptr ) == PNG_COLOR_TYPE_GRAY_ALPHA)
      linesize *= 2;
    else
      if (png_get_color_type( png_ptr, info_ptr ) == PNG_COLOR_TYPE_RGB)
        linesize *= 3;
      else
        if (png_get_color_type( png_ptr, info_ptr ) == PNG_COLOR_TYPE_RGB_ALPHA)
          linesize *= 4;

    unsigned int height = png_get_image_height(png_ptr,info_ptr);
    // Alloc the whole thing at once
    rows[0] = new png_byte[linesize * height];
    if (!rows[0])
      return ok = problem("couldn't allocate space for image");

    // Re-point rows.
    for (unsigned int y = 1; y < height; ++y)
      rows[y] = rows[0] + y * linesize;

    return true;
  }

  png_byte** get_rows()
  {
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
      assert(rows != 0);
    }

    return rows;
  }

  ~vil1_png_structures()
  {
    png_setjmp_on(goto del);
    if (reading_) {
      // Reading - just delete
      png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp)NULL);

    } else {
      // Writing - save the rows
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

vil1_png_generic_image::vil1_png_generic_image(vil1_stream* is)
: vs_(is),
  p(new vil1_png_structures(true))
{
  vs_->ref();
  read_header();
}

bool vil1_png_generic_image::get_property(char const *tag, void *prop) const
{
  if (0==vcl_strcmp(tag, vil1_property_top_row_first))
    return prop ? (*(bool*)prop) = true : true;

  if (0==vcl_strcmp(tag, vil1_property_left_first))
    return prop ? (*(bool*)prop) = true : true;

  return false;
}

vil1_png_generic_image::vil1_png_generic_image(vil1_stream* is, int /*planes*/,
                                               int width,
                                               int height,
                                               int components,
                                               int bits_per_component,
                                               vil1_component_format /*format*/)
: vs_(is),
  width_(width),
  height_(height),
  components_(components),
  bits_per_component_(bits_per_component),
  p(new vil1_png_structures(false))
{
  vs_->ref();
  write_header();
}

vil1_png_generic_image::~vil1_png_generic_image()
{
  delete p;
  vs_->unref();
}

char const* vil1_png_generic_image::file_format() const
{
  return vil1_png_format_tag;
}

bool vil1_png_generic_image::read_header()
{
  if (!p->ok)
    return false;

  png_setjmp_on(return false);

  vs_->seek(0L);
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

  if (png_get_bit_depth( p->png_ptr, p->info_ptr ) < 8)
    png_set_packing (p->png_ptr);

#if 0
 int maxval;
 if (p->info_ptr->color_type == PNG_COLOR_TYPE_PALETTE) { maxval = 255; }
 else { maxval = (1l << p->info_ptr->bit_depth) - 1; }
#endif

  p->channels = png_get_channels(p->png_ptr, p->info_ptr);

#if VXL_LITTLE_ENDIAN
  // PNG stores data MSB
  if ( png_get_bit_depth( p->png_ptr, p->info_ptr ) > 8 )
    png_set_swap( p->png_ptr );
#endif

  this->width_ = png_get_image_width( p->png_ptr, p->info_ptr );
  this->height_ = png_get_image_height( p->png_ptr, p->info_ptr );
  this->components_ = png_get_channels( p->png_ptr, p->info_ptr );
  this->bits_per_component_ = png_get_bit_depth( p->png_ptr, p->info_ptr );

  if (png_get_valid( p->png_ptr, p->info_ptr, PNG_INFO_sBIT )) problem("LAZY AWF! PNG_INFO_sBIT");

  // if (p->info_ptr->valid & PNG_INFO_bKGD) problem("LAZY AWF! PNG_INFO_bKGD");
  png_setjmp_off();
  return true;
}

bool vil1_png_generic_image::write_header()
{
  if (!p->ok)
    return false;

  png_setjmp_on( return false );

  vs_->seek(0L);

  png_set_write_fn(p->png_ptr, vs_, user_write_data, user_flush_data);

  int color_type;
  if (components_ == 3)
    color_type = PNG_COLOR_TYPE_RGB;
  else
    color_type = PNG_COLOR_TYPE_GRAY;

  png_set_IHDR(p->png_ptr, p->info_ptr, width_, height_, bits_per_component_, color_type,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  png_write_info(p->png_ptr, p->info_ptr);

#if VXL_LITTLE_ENDIAN
  // PNG stores data MSB
  if ( bits_per_component_ > 8 )
    png_set_swap( p->png_ptr );
#endif

  // Make memory image
  p->channels = components_;
  p->alloc_image();

  png_setjmp_off();

  return true;
}

bool vil1_png_generic_image::get_section(void* buf, int x0, int y0, int xs, int ys) const
{
  if (!p->ok)
    return false;

  // PNG lib wants everything in memory - the first get_rows reads the whole image.
  png_byte** rows = p->get_rows();
  if (!rows) return 0;

  int bytes_per_pixel = png_get_bit_depth(p->png_ptr,p->info_ptr) * p->channels / 8;
  int bytes_per_row_dst = xs*bytes_per_pixel;
  if ((unsigned int)xs == png_get_image_width(p->png_ptr, p->info_ptr)) {
    assert(x0 == 0);
    vcl_memcpy(buf, rows[y0], ys * bytes_per_row_dst);
  }
  else {
    png_byte* dst = (png_byte*)buf;
    for (int y = 0; y < ys; ++y, dst += bytes_per_row_dst)
      vcl_memcpy(dst, &rows[y0+y][x0*bytes_per_pixel], xs*bytes_per_pixel);
  }

  return true;
}

bool vil1_png_generic_image::put_section(void const* buf, int x0, int y0, int xs, int ys)
{
  if (!p->ok)
    return false;

  // PNG lib wants everything in memory - the writing isn't done till this image is deleted.

  png_byte** rows = p->get_rows();
  if (!rows) return false;

  int bytes_per_pixel = png_get_bit_depth(p->png_ptr,p->info_ptr) * p->channels / 8;
  int bytes_per_row_dst = xs*bytes_per_pixel;
  if ((unsigned int)xs == png_get_image_width(p->png_ptr, p->info_ptr)) {
    assert(x0 == 0);
    vcl_memcpy(rows[y0], buf, ys * bytes_per_row_dst);
  }
  else {
    const png_byte* dst = (const png_byte*)buf;
    for (int y = 0; y < ys; ++y, dst += bytes_per_row_dst)
      vcl_memcpy(&rows[y0+y][x0*bytes_per_pixel], dst, xs*bytes_per_pixel);
  }

  return true;
}

vil1_image vil1_png_generic_image::get_plane(unsigned int plane) const
{
  assert(plane == 0);
  return const_cast<vil1_png_generic_image*>(this);
}
