#ifdef __GNUC__
#pragma implementation "vil_png.h"
#endif

#include "vil_png.h"

#include <assert.h>
#include <stdio.h> // for sprintf

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cstring.h>

#include <vil/vil_stream.h>
#include <vil/vil_generic_image.h>

#include <png.h>

// Constants
#define SIG_CHECK_SIZE 4

char const* vil_png_format_tag = "png";

// Functions
static bool problem(char const* msg)
{
  cerr << "[vil_png: PROBLEM " <<msg << "]";
  return false;
}

vil_generic_image* vil_png_file_format::make_input_image(vil_stream* is)
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

vil_generic_image* vil_png_file_format::make_output_image(vil_stream* is, vil_generic_image const* prototype)
{
  return new vil_png_generic_image(is, prototype);
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

static void pngtopnm_error_handler (png_structp png_ptr, png_const_charp msg)
{
  /* this function, aside from the extra step of retrieving the "error
   * pointer" (below) and the fact that it exists within the application
   * rather than within libpng, is essentially identical to libpng's
   * default error handler.  The second point is critical:  since both
   * setjmp() and longjmp() are called from the same code, they are
   * guaranteed to have compatible notions of how big a jmp_buf is,
   * regardless of whether _BSD_SOURCE or anything else has (or has not)
   * been defined. */

  fprintf(stderr, "pnmtopng:  fatal libpng error: %s\n", msg);
  fflush(stderr);

  vil_jmpbuf_wrapper  *jmpbuf_ptr = (vil_jmpbuf_wrapper*) png_get_error_ptr(png_ptr);
  if (jmpbuf_ptr == NULL) {         /* we are completely hosed now */
    fprintf(stderr,
      "pnmtopng:  EXTREMELY fatal error: jmpbuf unrecoverable; terminating.\n");
    fflush(stderr);
    exit(99);
  }

  longjmp(jmpbuf_ptr->jmpbuf, 1);
}

struct vil_png_structures {
  png_struct *png_ptr;
  png_info *info_ptr;
  png_byte **rows;
  int channels;

  vil_png_structures() {
    png_ptr = 0;
    info_ptr = 0;
    rows = 0;
    channels = 0;

    png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, &pngtopnm_jmpbuf_struct, pngtopnm_error_handler, NULL);
    if (!png_ptr) {
      problem("cannot allocate LIBPNG structure");
      return;
    }

    info_ptr = png_create_info_struct (png_ptr);
    if (!info_ptr) {
      problem("cannot allocate LIBPNG structures");
      return;
    }
  
    if (setjmp (pngtopnm_jmpbuf_struct.jmpbuf) != 0) {
      problem ("setjmp returns error condition");
      return;
    }
  }
  
  bool alloc_image() {
    rows = new png_byte* [info_ptr->height];
    if (rows == NULL)
      return problem("couldn't allocate space for image");

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
      return problem("couldn't allocate space for image");

    // Re-point rows.
    for (int y = 1 ; y < info_ptr->height ; ++y)
      rows[y] = rows[0] + y * linesize;
  }

  png_byte** get_rows() {
    if (!rows)
      if (alloc_image()) {
	png_read_image (png_ptr, rows);
	png_read_end (png_ptr, info_ptr);
      }
    
    return rows;
  }
  
  ~vil_png_structures() {
    if (rows) {
      delete [] rows[0];
      delete [] rows;
    }
    
    png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp)NULL);
  }
};


/////////////////////////////////////////////////////////////////////////////

vil_png_generic_image::vil_png_generic_image(vil_stream* is):
  is_(is),
  p(new vil_png_structures)
{
  read_header();
}

vil_png_generic_image::vil_png_generic_image(vil_stream* is, vil_generic_image const* prototype):
  is_(is),
  p(new vil_png_structures)
{
  width_ = prototype->width();
  height_ = prototype->height();
  components_ = prototype->components();
  bits_per_component_ = prototype->bits_per_component();

  write_header();
}

vil_png_generic_image::~vil_png_generic_image()
{
  delete p;
}

char const* vil_png_generic_image::file_format() const
{
  return vil_png_format_tag;
}

bool vil_png_generic_image::read_header()
{
  is_->seek(0);
  png_byte sig_buf [SIG_CHECK_SIZE];
  if (is_->read(sig_buf, SIG_CHECK_SIZE) != SIG_CHECK_SIZE) 
    return problem("Initial header fread");
  
  if (png_sig_cmp (sig_buf, (png_size_t) 0, (png_size_t) SIG_CHECK_SIZE) != 0)
    return problem("png_sig_cmp");

  png_set_read_fn(p->png_ptr, is_, user_read_data);
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
}

bool vil_png_generic_image::write_header()
{
}

bool vil_png_generic_image::do_get_section(void* buf, int x0, int y0, int xs, int ys) const
{
  png_byte** rows = p->get_rows();
  if (!rows) return 0;
  
  int bytes_per_pixel  = p->info_ptr->bit_depth*p->channels / 8;
  int bytes_per_row_dst = xs*bytes_per_pixel;
  if (xs == p->info_ptr->width) {
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

bool vil_png_generic_image::do_put_section(void const* buf, int x0, int y0, int xs, int ys)
{
  return problem("lazy awf");
}

vil_generic_image* vil_png_generic_image::get_plane(int plane) const 
{
  assert(plane == 0);
  return const_cast<vil_png_generic_image*>(this);
}

#if 0
//-----------------------------------------------------------------------------
void PNGImage::Print(FILE* os) const
{
  static char const* type_string = "";
  static char const* alpha_string = "";
  switch (p->info_ptr->color_type) {
  case PNG_COLOR_TYPE_GRAY:
    type_string = "gray";
    alpha_string = "";
    break;

  case PNG_COLOR_TYPE_GRAY_ALPHA:
    type_string = "gray";
    alpha_string = "+alpha";
    break;
    
  case PNG_COLOR_TYPE_PALETTE:
    type_string = "palette";
    alpha_string = "";
    break;
    
  case PNG_COLOR_TYPE_RGB:
    type_string = "truecolor";
    alpha_string = "";
    break;
    
  case PNG_COLOR_TYPE_RGB_ALPHA:
    type_string = "truecolor";
    alpha_string = "+alpha";
    break;
  }
  if (p->info_ptr->valid & PNG_INFO_tRNS) {
    alpha_string = "+transparency";
  }
  
  char gamma_string[1024];
  if (p->info_ptr->valid & PNG_INFO_gAMA) {
    sprintf (gamma_string, ", image gamma = %4.2f", p->info_ptr->gamma);
  } else {
    strcpy (gamma_string, "");
  }
  
  fprintf(os, "PNGImage: %ld x %ld image, %d channels of %d bit%s %s%s%s%s\n",
	  p->info_ptr->width, p->info_ptr->height,
	  p->channels,
	  p->info_ptr->bit_depth, p->info_ptr->bit_depth > 1 ? "s" : "",
	  type_string, alpha_string, gamma_string,
	  p->info_ptr->interlace_type ? ", Adam7 interlaced" : "");
}
#endif
