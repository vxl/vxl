// This is core/vil/file_formats/vil_png.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// http://www.libpng.org/pub/png/libpng.html

#include "vil_png.h"

#include <vcl_cassert.h>
#include <vcl_cstring.h>
#include <vcl_iostream.h>

#include <vil/vil_stream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_property.h>

#include <png.h>
#if (PNG_LIBPNG_VER_MAJOR == 0)
extern "You need a later libpng. You should rerun CMake, after setting VXL_FORCE_V3P_PNG to ON."
#endif
#include <vcl_cstdlib.h> // for vcl_exit()

#include <vxl_config.h>

// Constants
#define SIG_CHECK_SIZE 4

char const* vil_png_format_tag = "png";

// Functions
static bool problem(char const* msg)
{
  vcl_cerr << "[vil_png: PROBLEM " <<msg << ']';
  return false;
}

vil_image_resource_sptr vil_png_file_format::make_input_image(vil_stream* is)
{
  // Attempt to read header
  png_byte sig_buf [SIG_CHECK_SIZE];
  if (is->read(sig_buf, SIG_CHECK_SIZE) != SIG_CHECK_SIZE) {
    problem("Initial header fread");
    return 0;
  }

  if (png_sig_cmp (sig_buf, (png_size_t) 0, (png_size_t) SIG_CHECK_SIZE) != 0)
    return 0;

  return new vil_png_image(is);
}

vil_image_resource_sptr vil_png_file_format::make_output_image(vil_stream* vs,
                                                               unsigned nx,
                                                               unsigned ny,
                                                               unsigned nplanes,
                                                               enum vil_pixel_format format)
{
  if (format != VIL_PIXEL_FORMAT_BYTE &&
      format != VIL_PIXEL_FORMAT_UINT_16)
  // FIXME || format != VIL_PIXEL_FORMAT_BOOL

  {
    vcl_cout<<"ERROR! vil_png_file_format::make_output_image()\n"
            <<"Pixel format should be byte, but is "<<format<<" instead.\n";
    return 0;
  }

  return new vil_png_image(vs, nx, ny, nplanes, format);
}

char const* vil_png_file_format::tag() const
{
  return vil_png_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

static void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  vil_stream* f = static_cast<vil_stream*>(png_get_io_ptr(png_ptr));
  f->read(data, length);
}

static void user_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  vil_stream* f = static_cast<vil_stream*>(png_get_io_ptr(png_ptr));
  f->write(data, length);
}

static void user_flush_data(png_structp /*png_ptr*/)
{
  // IOFile* f = (IOFile*)png_get_io_ptr(png_ptr);
  // urk.  how to flush?
}

struct vil_jmpbuf_wrapper {
  jmp_buf jmpbuf;
};
static vil_jmpbuf_wrapper pngtopnm_jmpbuf_struct;
static bool jmpbuf_ok = false;

// Must be  a macro - setjmp needs its stack frame to live
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
  vcl_cerr << "vil_png:  fatal libpng error: " << msg << '\n';

  if (!jmpbuf_ok) {
    // Someone called the error handler when the setjmp was wrong
    vcl_cerr << "vil_png: jmpbuf is pretty far from ok.  returning\n";
    // vcl_abort();
    return;
  }

  vil_jmpbuf_wrapper  *jmpbuf_ptr = static_cast<vil_jmpbuf_wrapper*>(png_get_error_ptr(png_ptr));
  if (jmpbuf_ptr == NULL) {         // we are completely hosed now
    vcl_cerr << "pnmtopng:  EXTREMELY fatal error: jmpbuf unrecoverable; terminating.\n";
    vcl_exit(99);
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
      return ok = problem("couldn't allocate space for image");

    unsigned long linesize;
    if (png_get_bit_depth(png_ptr, info_ptr) == 16)
      linesize = 2 * info_ptr->width;
    else
      linesize = info_ptr->width;

    if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_GRAY_ALPHA)
      linesize *= 2;
    else
      if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
        linesize *= 3;
      else
        if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB_ALPHA)
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
      assert(rows != 0);
    }

    return rows;
  }

  ~vil_png_structures() {
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

vil_png_image::vil_png_image(vil_stream* is):
  vs_(is),
  p_(new vil_png_structures(true))
{
  vs_->ref();
  read_header();
}

bool vil_png_image::get_property(char const *key, void * value) const
{
  if (vcl_strcmp(vil_property_quantisation_depth, key)==0)
  {
    if (value)
      *static_cast<unsigned int*>(value) = bits_per_component_;
    return true;
  }

  return false;
}

vil_png_image::vil_png_image(vil_stream *s,
                             unsigned nx,
                             unsigned ny,
                             unsigned nplanes,
                             enum vil_pixel_format format):
  vs_(s),
  width_(nx),
  height_(ny),
  components_(nplanes),
  format_(format),
  p_(new vil_png_structures(false))
{
  if (format == VIL_PIXEL_FORMAT_BOOL) bits_per_component_ = 1;
  else bits_per_component_ = vil_pixel_format_sizeof_components(format) * 8;

  vs_->ref();
  write_header();
  assert(format == VIL_PIXEL_FORMAT_BYTE ||
         format == VIL_PIXEL_FORMAT_UINT_16);
  // FIXME || format == VIL_PIXEL_FORMAT_BOOL
}

vil_png_image::~vil_png_image()
{
  delete p_;
  vs_->unref();
}


char const* vil_png_image::file_format() const
{
  return vil_png_format_tag;
}

bool vil_png_image::read_header()
{
  if (!p_->ok)
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

  png_set_read_fn(p_->png_ptr, vs_, user_read_data);
  png_set_sig_bytes (p_->png_ptr, SIG_CHECK_SIZE);
  png_read_info (p_->png_ptr, p_->info_ptr);

  if (png_get_bit_depth(p_->png_ptr, p_->info_ptr) < 8)
    png_set_packing (p_->png_ptr);

#if 0
 int maxval;
 if (p->info_ptr->color_type == PNG_COLOR_TYPE_PALETTE) { maxval = 255; }
 else { maxval = (1l << p->info_ptr->bit_depth) - 1; }
#endif

  p_->channels = png_get_channels(p_->png_ptr, p_->info_ptr);

#if VXL_LITTLE_ENDIAN
  // PNG stores data MSB
  if ( png_get_bit_depth(p_->png_ptr, p_->info_ptr) > 8 )
    png_set_swap(p_->png_ptr);
#endif

  this->width_ = png_get_image_width(p_->png_ptr, p_->info_ptr);
  this->height_ = png_get_image_height(p_->png_ptr, p_->info_ptr);
  this->components_ = png_get_channels(p_->png_ptr, p_->info_ptr);
  this->bits_per_component_ = png_get_bit_depth(p_->png_ptr, p_->info_ptr);

  if (this->bits_per_component_ == 1) format_ = VIL_PIXEL_FORMAT_BOOL;
  else if (this->bits_per_component_ <=8) format_=VIL_PIXEL_FORMAT_BYTE;
  else format_ = VIL_PIXEL_FORMAT_UINT_16;

  if (png_get_valid(p_->png_ptr, p_->info_ptr, PNG_INFO_sBIT)) problem("LAZY AWF! PNG_INFO_sBIT");

  // if (p->info_ptr->valid & PNG_INFO_bKGD) problem("LAZY AWF! PNG_INFO_bKGD");
  png_setjmp_off();
  return true;
}

bool vil_png_image::write_header()
{
  if (!p_->ok)
    return false;

  png_setjmp_on( return false );

  vs_->seek(0L);

  png_set_write_fn(p_->png_ptr, vs_, user_write_data, user_flush_data);

  int color_type;
  if (components_ == 3)
    color_type = PNG_COLOR_TYPE_RGB;
  else
    color_type = PNG_COLOR_TYPE_GRAY;

  png_set_IHDR(p_->png_ptr, p_->info_ptr, width_, height_, bits_per_component_, color_type,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  png_write_info(p_->png_ptr, p_->info_ptr);

#if VXL_LITTLE_ENDIAN
  // PNG stores data MSB
  if ( bits_per_component_ > 8 )
    png_set_swap(p_->png_ptr);
#endif

  // Make memory image
  p_->channels = components_;
  p_->alloc_image();

  png_setjmp_off();

  return true;
}

vil_image_view_base_sptr vil_png_image::get_copy_view(unsigned x0,
                                                      unsigned nx,
                                                      unsigned y0,
                                                      unsigned ny) const
{
  if (!p_->ok)
    return 0;

  // PNG lib wants everything in memory - the first get_rows reads the whole image.
  png_byte** rows = p_->get_rows();
  if (!rows) return 0;

  int bit_depth = png_get_bit_depth(p_->png_ptr,p_->info_ptr);
  int bytes_per_pixel = bit_depth * p_->channels / 8;
  int bytes_per_row_dst = nx*nplanes() * vil_pixel_format_sizeof_components(format_);

  vil_memory_chunk_sptr chunk = new vil_memory_chunk(ny*bytes_per_row_dst, format_);

  if ((bit_depth==16) &&
      (nx == png_get_image_width(p_->png_ptr, p_->info_ptr)))
  {
    assert(x0 == 0);

    vcl_memcpy(reinterpret_cast<char*>(chunk->data()), rows[y0], ny * bytes_per_row_dst);
    return new vil_image_view<vxl_uint_16>(chunk, reinterpret_cast<vxl_uint_16*>(chunk->data()),
      nx, ny, nplanes(), nplanes(), nplanes()*nx, 1);
  }
  if ((bit_depth ==8) &&
      (nx == png_get_image_width(p_->png_ptr, p_->info_ptr)))
  {
    assert(x0 == 0);

    vcl_memcpy(reinterpret_cast<char*>(chunk->data()), rows[y0], ny * bytes_per_row_dst);
    return new vil_image_view<vxl_byte>(chunk, reinterpret_cast<vxl_byte*>(chunk->data()),
      nx, ny, nplanes(), nplanes(), nplanes()*nx, 1);
  }
  else if (bit_depth==16)
  {
    png_byte* dst = reinterpret_cast<png_byte*>(chunk->data());
    for (unsigned y = 0; y < ny; ++y, dst += bytes_per_row_dst)
      vcl_memcpy(dst, &rows[y0+y][x0*bytes_per_pixel], nx*bytes_per_pixel);
    return new vil_image_view<vxl_uint_16>(chunk, reinterpret_cast<vxl_uint_16*>(chunk->data()),
      nx, ny, nplanes(), nplanes(), nplanes()*nx, 1);
  }
  else if (bit_depth==8)
  {
    png_byte* dst = reinterpret_cast<png_byte*>(chunk->data());
    for (unsigned y = 0; y < ny; ++y, dst += bytes_per_row_dst)
      vcl_memcpy(dst, &rows[y0+y][x0*bytes_per_pixel], nx*bytes_per_pixel);
    return new vil_image_view<vxl_byte>(chunk, reinterpret_cast<vxl_byte*>(chunk->data()),
      nx, ny, nplanes(), nplanes(), nplanes()*nx, 1);
  }
  // FIXME Can't handle pixel depths of 1 2 or 4 pixels yet
  else return 0;
}


bool vil_png_image::put_view(const vil_image_view_base &view,
                             unsigned x0, unsigned y0)
{
  if (!view_fits(view, x0, y0))
  {
    vcl_cerr << "ERROR: " << __FILE__ << ":\n view does not fit\n";
    return false;
  }

  if (!p_->ok) return false;

  // PNG lib wants everything in memory - the writing isn't done till this image is deleted.

  png_byte** rows = p_->get_rows();
  if (!rows) return false;

  // int bytes_per_pixel = bit_depth  * p_->channels / 8;
  // int bytes_per_row_dst = view.ni()*bytes_per_pixel;
  if (bits_per_component_ == 8)
  {
    if (view.pixel_format() != VIL_PIXEL_FORMAT_BYTE) return false;
    const vil_image_view<vxl_byte> &view2 = static_cast<const vil_image_view<vxl_byte>&>(view);
    if (nplanes()==1)
    {
      for (unsigned y = 0; y < view.nj(); ++y)
        for (unsigned x=0; x < view.ni(); ++x)
          rows[y0+y][x0+x] = view2(x,y);
    }
    else
    {
      assert(nplanes() == 3);
      for (unsigned y = 0; y < view.nj(); ++y)
        for (unsigned x=0; x < view.ni(); ++x)
        {
          rows[y0+y][(x0+x)*3] = view2(x,y,0);
          rows[y0+y][(x0+x)*3+1] = view2(x,y,1);
          rows[y0+y][(x0+x)*3+2] = view2(x,y,2);
        }
    }
  }
  else if (bits_per_component_ == 16)
  {
    if (view.pixel_format() != VIL_PIXEL_FORMAT_UINT_16) return false;
    const vil_image_view<vxl_uint_16> &view2 = static_cast<const vil_image_view<vxl_uint_16>&>(view);
    if (nplanes()==1)
    {
      for (unsigned y = 0; y < view.nj(); ++y)
        for (unsigned x=0; x < view.ni(); ++x)
          *reinterpret_cast<vxl_uint_16*>(&rows[y0+y][(x0+x)*2]) = view2(x,y);
    }
    else
    {
      assert(nplanes() == 3);
      for (unsigned y = 0; y < view.nj(); ++y)
        for (unsigned x=0; x < view.ni(); ++x)
        {
          *reinterpret_cast<vxl_uint_16*>(&rows[y0+y][(x0+x)*6]) = view2(x,y,0);
          *reinterpret_cast<vxl_uint_16*>(&rows[y0+y][(x0+x)*6+2]) = view2(x,y,1);
          *reinterpret_cast<vxl_uint_16*>(&rows[y0+y][(x0+x)*6+4]) = view2(x,y,2);
        }
    }
  }
  // FIXME else if (bit_depth=1)
  else return false;

  return true;
}
