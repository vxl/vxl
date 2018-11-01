// This is core/vil/file_formats/vil_png.cxx
//:
// \file
// http://www.libpng.org/pub/png/libpng.html

#include <cstring>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include "vil_png.h"

#include <cassert>

#include <vil/vil_stream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_property.h>
#include <vil/vil_exception.h>

#include <png.h>
#if (PNG_LIBPNG_VER_MAJOR == 0)
extern "You need a later libpng. You should rerun CMake, after setting VXL_FORCE_V3P_PNG to ON."
#endif
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vxl_config.h>

// Constants
#define SIG_CHECK_SIZE 4

char const* vil_png_format_tag = "png";

// Functions
static bool problem(char const* msg)
{
  std::cerr << "[vil_png: PROBLEM " <<msg << ']';
  return false;
}

vil_image_resource_sptr vil_png_file_format::make_input_image(vil_stream* is)
{
  // Attempt to read header
  png_byte sig_buf [SIG_CHECK_SIZE];
  if (is->read(sig_buf, SIG_CHECK_SIZE) != SIG_CHECK_SIZE) {
    problem("Initial header fread");
    return nullptr;
  }

  if (png_sig_cmp (sig_buf, (png_size_t) 0, (png_size_t) SIG_CHECK_SIZE) != 0)
    return nullptr;

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
    std::cout<<"ERROR! vil_png_file_format::make_output_image()\n"
            <<"Pixel format should be byte, but is "<<format<<" instead.\n";
    return nullptr;
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
  auto* f = static_cast<vil_stream*>(png_get_io_ptr(png_ptr));
  f->read(data, length);
}

static void user_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  auto* f = static_cast<vil_stream*>(png_get_io_ptr(png_ptr));
  f->write(data, length);
}

static void user_flush_data(png_structp /*png_ptr*/)
{
  // IOFile* f = (IOFile*)png_get_io_ptr(png_ptr);
  // urk.  how to flush?
}

struct vil_jmpbuf_wrapper
{
  jmp_buf jmpbuf;
};

static vil_jmpbuf_wrapper pngtopnm_jmpbuf_struct;
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
  std::cerr << "vil_png:  fatal libpng error: " << msg << '\n';

  if (!jmpbuf_ok) {
    // Someone called the error handler when the setjmp was wrong
    std::cerr << "vil_png: jmpbuf is pretty far from ok.  returning\n";
    // std::abort();
    return;
  }

  auto  *jmpbuf_ptr = static_cast<vil_jmpbuf_wrapper*>(png_get_error_ptr(png_ptr));
  if (jmpbuf_ptr == nullptr) {         // we are completely hosed now
    std::cerr << "pnmtopng:  EXTREMELY fatal error: jmpbuf unrecoverable; terminating.\n";
    std::exit(99);
  }

  longjmp(jmpbuf_ptr->jmpbuf, 1);
}

struct vil_png_structures
{
  bool reading_;
  png_struct *png_ptr;
  png_info *info_ptr;
  png_byte **rows;
  int channels;
  bool ok;

  vil_png_structures(bool reading)
  {
    reading_ = reading;
    png_ptr = nullptr;
    info_ptr = nullptr;
    rows = nullptr;
    channels = 0;
    ok = false;

    png_setjmp_on(return);

    if (reading)
      png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, &pngtopnm_jmpbuf_struct, pngtopnm_error_handler, nullptr);
    else
      png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, &pngtopnm_jmpbuf_struct, pngtopnm_error_handler, nullptr);

    if (!png_ptr) {
      problem("cannot allocate LIBPNG structure");
      return;
    }

    info_ptr = png_create_info_struct (png_ptr);
    if (!info_ptr) {
      png_destroy_read_struct(&png_ptr, nullptr, nullptr);
      problem("cannot allocate LIBPNG structures");
      return;
    }

    ok = true;

    // Now jmpbuf is broken, hope noone calls png....
    png_setjmp_off();
  }

  bool alloc_image()
  {
    rows = new png_byte* [png_get_image_height(png_ptr, info_ptr)];
    if (rows == nullptr)
      return ok = problem("couldn't allocate space for image");

    unsigned long linesize;
    if (png_get_bit_depth(png_ptr, info_ptr) == 16)
      linesize = 2 * png_get_image_width(png_ptr, info_ptr);
    else
      linesize = png_get_image_width(png_ptr, info_ptr);

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

  png_byte** get_rows()
  {
    if (reading_) {
      if (!rows) {
        if (alloc_image()) {
          png_setjmp_on(return nullptr);
          png_read_image (png_ptr, rows);
          png_read_end (png_ptr, info_ptr);
          png_setjmp_off();
        }
      }
    }
    else {
      assert(rows != nullptr);
    }

    return rows;
  }

  ~vil_png_structures()
  {
    png_setjmp_on(goto del);
    if (reading_) {
      // Reading - just delete
      png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp)nullptr);
    }
    else {
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

vil_png_image::vil_png_image(vil_stream* is)
: vs_(is),
  p_(new vil_png_structures(true))
{
  vs_->ref();
  read_header();
}

bool vil_png_image::get_property(char const *key, void * value) const
{
  if (std::strcmp(vil_property_quantisation_depth, key)==0)
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
                             enum vil_pixel_format format)
: vs_(s),
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

  //
  // First read info
  png_set_read_fn(p_->png_ptr, vs_, user_read_data);
  png_set_sig_bytes (p_->png_ptr, SIG_CHECK_SIZE);
  png_read_info (p_->png_ptr, p_->info_ptr);


  png_byte const color_type = png_get_color_type(p_->png_ptr, p_->info_ptr);
  png_byte const bit_depth = png_get_bit_depth(p_->png_ptr, p_->info_ptr);   // valid values are 1, 2, 4, 8, or 16
  bool is_bool_image = false;

#if 1
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    assert( bit_depth <= 8 );    // valid bit depth 1, 2, 4, 8
    png_set_palette_to_rgb(p_->png_ptr);
  }
  if (color_type == PNG_COLOR_TYPE_GRAY) {
    if (bit_depth==1) { //treat 1-bit image as bool image
      is_bool_image = true;
      png_set_packing(p_->png_ptr);  // This code expands pixels per byte without changing the values of the pixels"
    }
    else if (bit_depth < 8)  // treat these images as 8-bit greyscale image
      png_set_expand_gray_1_2_4_to_8(p_->png_ptr);
  }
  if (png_get_valid(p_->png_ptr, p_->info_ptr, PNG_INFO_tRNS)) {
    int channels = png_get_channels(p_->png_ptr, p_->info_ptr);
    assert( channels == 1 || channels == 3 );
    png_set_tRNS_to_alpha(p_->png_ptr);
  }
#else
  // According to manual:
  // "This code expands ... per byte without changing the values of the pixels"
  // But this is not desired if it has palette
  if (png_get_bit_depth(p_->png_ptr, p_->info_ptr) < 8)
    png_set_packing (p_->png_ptr);
#endif

#if VXL_LITTLE_ENDIAN
  // PNG stores data MSB
  if ( png_get_bit_depth(p_->png_ptr, p_->info_ptr) > 8 )
    png_set_swap(p_->png_ptr);
#endif

  png_color_8p sig_bit;
  if (png_get_valid(p_->png_ptr, p_->info_ptr, PNG_INFO_sBIT) && png_get_sBIT(p_->png_ptr, p_->info_ptr, &sig_bit)) {
    png_set_shift(p_->png_ptr, sig_bit);
  }

  //
  //  Update the info after putting in all these transforms
  //  From this point on, the info reflects not the raw image,
  //  but the image after transform and to be read.
  png_read_update_info(p_->png_ptr, p_->info_ptr);

  this->width_ = png_get_image_width(p_->png_ptr, p_->info_ptr);
  this->height_ = png_get_image_height(p_->png_ptr, p_->info_ptr);
  this->components_ = p_->channels = png_get_channels(p_->png_ptr, p_->info_ptr);
  this->bits_per_component_ = png_get_bit_depth(p_->png_ptr, p_->info_ptr);

  // Set bits_per_component_ back to 1 for bool image
  if (is_bool_image)
    this->bits_per_component_ = 1;

  if (this->bits_per_component_ == 1)     format_ = VIL_PIXEL_FORMAT_BOOL;
  else if (this->bits_per_component_==8)  format_ = VIL_PIXEL_FORMAT_BYTE;
  else if (this->bits_per_component_==16) format_ = VIL_PIXEL_FORMAT_UINT_16;
  else return problem("Bad bit depth");

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
  if (components_ == 4)
    color_type = PNG_COLOR_TYPE_RGB_ALPHA;
  else if (components_ == 3)
    color_type = PNG_COLOR_TYPE_RGB;
  else if (components_ == 2)
    color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
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
    return nullptr;

  // PNG lib wants everything in memory - the first get_rows reads the whole image.
  png_byte** rows = p_->get_rows();
  if (!rows) return nullptr;

  int bit_depth = bits_per_component_;  // value can be 1, 8, or 16
  int bytes_per_pixel = (bit_depth * p_->channels + 7) / 8;
  int bytes_per_row_dst = nx*nplanes() * vil_pixel_format_sizeof_components(format_);

  vil_memory_chunk_sptr chunk = new vil_memory_chunk(ny*bytes_per_row_dst, format_);

  if (nx == png_get_image_width(p_->png_ptr, p_->info_ptr))
  {
    assert(x0 == 0);

    if (bit_depth==1)
    {
      assert(format_==VIL_PIXEL_FORMAT_BOOL);

      std::memcpy(reinterpret_cast<char*>(chunk->data()), rows[y0], ny * bytes_per_row_dst);
      return new vil_image_view<bool>(chunk, reinterpret_cast<bool*>(chunk->data()),
        nx, ny, nplanes(), nplanes(), nplanes()*nx, 1);
    }
    else if (bit_depth==16)
    {
      assert(format_==VIL_PIXEL_FORMAT_UINT_16);

      std::memcpy(reinterpret_cast<char*>(chunk->data()), rows[y0], ny * bytes_per_row_dst);
      return new vil_image_view<vxl_uint_16>(chunk, reinterpret_cast<vxl_uint_16*>(chunk->data()),
        nx, ny, nplanes(), nplanes(), nplanes()*nx, 1);
    }
    else if (bit_depth ==8)
    {
      std::memcpy(reinterpret_cast<char*>(chunk->data()), rows[y0], ny * bytes_per_row_dst);
      return new vil_image_view<vxl_byte>(chunk, reinterpret_cast<vxl_byte*>(chunk->data()),
        nx, ny, nplanes(), nplanes(), nplanes()*nx, 1);
    }
    else return nullptr;
  }
  else   // not whole row
  {
    if (bit_depth==1)
    {
      assert(format_==VIL_PIXEL_FORMAT_BOOL);

      auto* dst = reinterpret_cast<png_byte*>(chunk->data());
      for (unsigned y = 0; y < ny; ++y, dst += bytes_per_row_dst)
        std::memcpy(dst, &rows[y0+y][x0*bytes_per_pixel], nx*bytes_per_pixel);
      return new vil_image_view<bool>(chunk, reinterpret_cast<bool*>(chunk->data()),
        nx, ny, nplanes(), nplanes(), nplanes()*nx, 1);
    }
    else if (bit_depth==16)
    {
      assert(format_==VIL_PIXEL_FORMAT_UINT_16);

      auto* dst = reinterpret_cast<png_byte*>(chunk->data());
      for (unsigned y = 0; y < ny; ++y, dst += bytes_per_row_dst)
        std::memcpy(dst, &rows[y0+y][x0*bytes_per_pixel], nx*bytes_per_pixel);
      return new vil_image_view<vxl_uint_16>(chunk, reinterpret_cast<vxl_uint_16*>(chunk->data()),
        nx, ny, nplanes(), nplanes(), nplanes()*nx, 1);
    }
    else if (bit_depth==8)
    {
      auto* dst = reinterpret_cast<png_byte*>(chunk->data());
      for (unsigned y = 0; y < ny; ++y, dst += bytes_per_row_dst)
        std::memcpy(dst, &rows[y0+y][x0*bytes_per_pixel], nx*bytes_per_pixel);
      return new vil_image_view<vxl_byte>(chunk, reinterpret_cast<vxl_byte*>(chunk->data()),
        nx, ny, nplanes(), nplanes(), nplanes()*nx, 1);
    }
    else return nullptr;
  }
}

bool vil_png_image::put_view(const vil_image_view_base &view,
                             unsigned x0, unsigned y0)
{
  if (!view_fits(view, x0, y0))
  {
    vil_exception_warning(vil_exception_out_of_bounds("vil_png_image::put_view"));
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
    const auto &view2 = static_cast<const vil_image_view<vxl_byte>&>(view);
    if (nplanes()==1)
    {
      for (unsigned y = 0; y < view.nj(); ++y)
        for (unsigned x=0; x < view.ni(); ++x)
          rows[y0+y][x0+x] = view2(x,y);
    }
    else if (nplanes()==2)
    {
      for (unsigned y = 0; y < view.nj(); ++y)
        for (unsigned x=0; x < view.ni(); ++x)
        {
          rows[y0+y][(x0+x)*2] = view2(x,y,0);
          rows[y0+y][(x0+x)*2+1] = view2(x,y,1);
        }
    }
    else if (nplanes()==3)
    {
      for (unsigned y = 0; y < view.nj(); ++y)
        for (unsigned x=0; x < view.ni(); ++x)
        {
          rows[y0+y][(x0+x)*3] = view2(x,y,0);
          rows[y0+y][(x0+x)*3+1] = view2(x,y,1);
          rows[y0+y][(x0+x)*3+2] = view2(x,y,2);
        }
    }
    else
    {
      assert(nplanes() == 4);
      for (unsigned y = 0; y < view.nj(); ++y)
        for (unsigned x=0; x < view.ni(); ++x)
        {
          rows[y0+y][(x0+x)*4] = view2(x,y,0);
          rows[y0+y][(x0+x)*4+1] = view2(x,y,1);
          rows[y0+y][(x0+x)*4+2] = view2(x,y,2);
          rows[y0+y][(x0+x)*4+3] = view2(x,y,3);
        }
    }
  }
  else if (bits_per_component_ == 16)
  {
    if (view.pixel_format() != VIL_PIXEL_FORMAT_UINT_16) return false;
    const auto &view2 = static_cast<const vil_image_view<vxl_uint_16>&>(view);
    if (nplanes()==1)
    {
      for (unsigned y = 0; y < view.nj(); ++y)
        for (unsigned x=0; x < view.ni(); ++x)
          *reinterpret_cast<vxl_uint_16*>(&rows[y0+y][(x0+x)*2]) = view2(x,y);
    }
    else if (nplanes() == 2)
    {
      for (unsigned y = 0; y < view.nj(); ++y)
        for (unsigned x=0; x < view.ni(); ++x)
        {
          *reinterpret_cast<vxl_uint_16*>(&rows[y0+y][(x0+x)*4]) = view2(x,y,0);
          *reinterpret_cast<vxl_uint_16*>(&rows[y0+y][(x0+x)*4+2]) = view2(x,y,1);
        }
    }
    else if (nplanes() == 3)
    {
      for (unsigned y = 0; y < view.nj(); ++y)
        for (unsigned x=0; x < view.ni(); ++x)
        {
          *reinterpret_cast<vxl_uint_16*>(&rows[y0+y][(x0+x)*6]) = view2(x,y,0);
          *reinterpret_cast<vxl_uint_16*>(&rows[y0+y][(x0+x)*6+2]) = view2(x,y,1);
          *reinterpret_cast<vxl_uint_16*>(&rows[y0+y][(x0+x)*6+4]) = view2(x,y,2);
        }
    }
    else
    {
      assert(nplanes() == 4);
      for (unsigned y = 0; y < view.nj(); ++y)
        for (unsigned x=0; x < view.ni(); ++x)
        {
          *reinterpret_cast<vxl_uint_16*>(&rows[y0+y][(x0+x)*8]) = view2(x,y,0);
          *reinterpret_cast<vxl_uint_16*>(&rows[y0+y][(x0+x)*8+2]) = view2(x,y,1);
          *reinterpret_cast<vxl_uint_16*>(&rows[y0+y][(x0+x)*8+4]) = view2(x,y,2);
          *reinterpret_cast<vxl_uint_16*>(&rows[y0+y][(x0+x)*8+6]) = view2(x,y,3);
        }
    }
  }
  // FIXME else if (bit_depth=1)
  else return false;

  return true;
}
