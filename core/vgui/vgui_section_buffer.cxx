// This is oxl/vgui/vgui_section_buffer.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm
// \brief  See vgui_section_buffer.h for a description of this file.
//
// \verbatim
// Modifications
//  16-AUG-2000  Marko Bacic, Oxford RRG -- Added support for multiple textures
//               Many cheap graphics cards do not support texture
//               maps bigger than 256x256. To support images greater then 256x256
//               it was necessary to add support for multiple textures. Hence an
//               image is rendered by tiling together several textures containing
//               different parts of it.
//
//  the_same_day fsm
//               Imposed my rigid ways on Marko's changes.
//               Fixes for SolarisGL.
// \endverbatim

#include "vgui_section_buffer.h"

#include <vcl_cassert.h>
#include <vcl_cstring.h> // memmove()
#include <vcl_iostream.h>
#include <vcl_vector.h>

#include <vil/vil_image.h>
#include <vil/vil_image_as.h>
#include <vil/vil_pixel.h>

#include <vgui/vgui_macro.h>
#include <vgui/vgui_pixel.h>
#include <vgui/vgui_cache_wizard.h>
#include <vgui/vgui_section_render.h>
#include <vgui/internals/vgui_accelerate.h>

static bool debug = false;

//: return the smallest power of two, 2^k, such that n <= 2^k
static unsigned next_power_of_two(unsigned n) {
  unsigned k=1;
  while (k < n)
    k *= 2;
  return k;
}

//------------------------------------------------------------------------------

//: constructor. determine size of memory block to allocate. initialize the_rasters.
vgui_section_buffer::vgui_section_buffer(int x_, int y_,
                                         unsigned w_, unsigned h_,
                                         GLenum format_,
                                         GLenum type_,
                                         bool alloc_as_texture)
  : format(format_), type(type_)
  , x(x_), y(y_)
  , w(w_), h(h_)
  //
  , the_pixels(0)
  , is_texture(alloc_as_texture)
  , the_rasters(0)
  , section_ok(true) // 'false' breaks vgui_section_buffer_of<T>
  //
  , tList(0)
{
  assert(x_>=0 && y_>=0);
  assert(w>0 && h>0);

  texture_size = 256;
  if (alloc_as_texture) {
    allocw = next_power_of_two(w);
    alloch = next_power_of_two(h);
  }
  else {
    allocw = w;
    alloch = h;
  }

  // It doesn't seem to make any sense to specify only one of the 'format' and
  // 'type' parameters. Until we decide if it makes sense, it's not allowed.
  if      (format == GL_NONE && type == GL_NONE)
    vgui_accelerate::instance()->vgui_choose_cache_format(&format, &type);
  else if (format != GL_NONE && type != GL_NONE)
    { } // ok
  else
    assert(false);

  // NB: GLbyte, GLshort and GLint are *guaranteed* to be 1,2 and 4 bytes (8 bits
  // each), even if the native short, int are not 2 and 4 bytes.
  unsigned components = num_components();
  the_rasters = new void * [alloch];
  switch (type) {
#define fsm_alloc_buffer(GLtype) \
{ GLtype *ptr = new GLtype[components * allocw * alloch]; the_pixels = ptr; \
  for (unsigned int i=0; i<h; ++i) \
    the_rasters[i] = ptr + i*allocw*components; /* alignment ? */ \
}

  case GL_UNSIGNED_BYTE:
  case GL_BYTE:
    fsm_alloc_buffer(GLbyte);
    break;

#ifdef GL_UNSIGNED_SHORT_5_6_5
  case GL_UNSIGNED_SHORT_5_6_5:
#endif
#ifdef GL_UNSIGNED_SHORT_5_5_5_1
  case GL_UNSIGNED_SHORT_5_5_5_1:
#endif
  case GL_UNSIGNED_SHORT:
  case GL_SHORT:
    fsm_alloc_buffer(GLshort);
    break;

  case GL_UNSIGNED_INT:
  case GL_INT:
    fsm_alloc_buffer(GLint);
    break;

  case GL_FLOAT:
    fsm_alloc_buffer(GLfloat);
    break;

  case GL_BITMAP: // what to do here?
  default:
    assert(0); // :(
    break;
  }
#undef fsm_alloc_buffer
}

//: destructor. casts pixel pointer and calls operator delete [] on it
vgui_section_buffer::~vgui_section_buffer() {
  assert(the_rasters!=0);
  delete [] the_rasters; the_rasters=0;

  assert(the_pixels!=0);
  switch (type) {
#define fsm_dealloc_buffer(GLtype) { delete [] static_cast<GLtype*>(the_pixels); }
  case GL_UNSIGNED_BYTE:
  case GL_BYTE:
    fsm_dealloc_buffer(GLbyte);
    break;

#ifdef GL_UNSIGNED_SHORT_5_6_5
  case GL_UNSIGNED_SHORT_5_6_5:
#endif
#ifdef GL_UNSIGNED_SHORT_5_5_5_1
  case GL_UNSIGNED_SHORT_5_5_5_1:
#endif
  case GL_UNSIGNED_SHORT:
  case GL_SHORT:
    fsm_dealloc_buffer(GLshort);
    break;

  case GL_UNSIGNED_INT:
  case GL_INT:
    fsm_dealloc_buffer(GLint);
    break;

  case GL_FLOAT:
    fsm_dealloc_buffer(GLfloat);
    break;

  case GL_BITMAP:
  default:
    assert(0); // :(
    break;
  }
#undef fsm_dealloc_buffer
  the_pixels = 0;

  if (tList) {
    glDeleteTextures(countw*counth, tList);
    delete [] tList;
    tList = 0;
  }
}

unsigned vgui_section_buffer::num_components() const {
  switch (format) {
    // not sure about these three yet :
    //case GL_COLOR_INDEX:
    //case GL_STENCIL_INDEX:
    //case GL_DEPTH_COMPONENT:
  case GL_RED:
  case GL_GREEN:
  case GL_BLUE:
  case GL_ALPHA:
  case GL_LUMINANCE:
  case GL_LUMINANCE_ALPHA:
#ifdef GL_UNSIGNED_SHORT_5_6_5
  case GL_UNSIGNED_SHORT_5_6_5: // urgh!
#endif
#ifdef GL_UNSIGNED_SHORT_5_5_5_1
  case GL_UNSIGNED_SHORT_5_5_5_1: // urgh!
#endif
    return 1;

  case GL_RGB:
  case GL_BGR:
    return 3;

  case GL_RGBA:
#ifdef GL_BGRA
  case GL_BGRA:
#endif
#if defined(GL_EXT_abgr) || defined(GL_ABGR_EXT)
  case GL_ABGR_EXT:
#endif
    return 4;
  default:
    assert(false); //  :(
    return 0;
  }
}
//------------------------------------------------------------------------------
//
// pixel format conversions
// [The funky do-while construct is used to allow a semicolon after each macro call.]

// 'pix' is the pixel type supplied by the image.
// 'wh'  is a string describing the pixel type.
// uses: 'section_ok', 'image', 'data', 'x', 'y', 'w', 'h', 'allocw', 'alloch'
#define fsm_macro_begin(pix, wh) \
pix *data = new pix[ w*h ]; /* note: this buffer is w-by-h, not allocw-by-alloch */ \
section_ok = image.get_section( data, x,y, w,h ); \
char const *what = wh; \
if (false) do { } while (false)

// 'fmt' is the GLenum format passed to glDrawPixels()
// 'typ' is the GLenum type passed to glDrawPixels()
// 'sto' is the pixel type to store the section as. it must agree with 'fmt' and 'typ'.
// uses: 'what', 'data', 'the_pixels', 'w', 'h'
#define fsm_macro_magic(fmt, typ, sto) \
else if (format==(fmt) && type==(typ)) do { \
  if (debug) \
    vcl_cerr << __FILE__ ": converting " << what << " image to " #fmt "," #typ " format\n"; \
  if (!the_pixels) \
    the_pixels = new sto[allocw*alloch]; \
  if (w != allocw) /* have to convert each raster separately in this case */ \
    for (unsigned i=0; i<h; ++i) \
      vgui_pixel_convert_span(data + w*i, static_cast<sto *>(the_rasters[i]), w); \
  else /* otherwise, it's more efficient to do it all in one go */ \
    vgui_pixel_convert_span(data, static_cast<sto*>(the_pixels), w*h); \
} while (false)

// you *must* call this -- to deallocate the temp buffer.
#define fsm_macro_end \
else { /* not really necessary */ } \
delete [] data; \
assert(section_ok)

void vgui_section_buffer::apply(vil_image const& image_in) {
  // FIXME: the calls to fsm_macro_magic() are identical for each image pixel type.
  // They could be coalesced to reduce code maintenance.
  vil_image image = image_in;
  vil_pixel_format_t pixel_format = vil_pixel_format(image);

  // Convert non-handled formats to ones we can handle.
  // e.g. uint32 -> float
  if (pixel_format == VIL_UINT32 || pixel_format == VIL_UINT16) {
    image  = vil_image_as_float(image_in);
    pixel_format = vil_pixel_format(image);
  }
  if (pixel_format == VIL_RGB_UINT16) {
    image  = vil_image_as_rgb_float(image_in);
    pixel_format = vil_pixel_format(image);
  }

  // 8bit greyscale
  if (pixel_format == VIL_BYTE) {
    fsm_macro_begin(GLubyte, "8 bit greyscale");
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_BYTE,        vgui_pixel_rgb888);
    fsm_macro_magic(GL_BGR,      GL_UNSIGNED_BYTE,        vgui_pixel_bgr888);
    fsm_macro_magic(GL_RGBA,     GL_UNSIGNED_BYTE,        vgui_pixel_rgba8888);
#if defined(GL_UNSIGNED_SHORT_5_6_5)
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_SHORT_5_6_5, vgui_pixel_rgb565);
#endif
#if defined(GL_UNSIGNED_SHORT_5_5_5_1)
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_SHORT_5_5_5_1, vgui_pixel_bgra5551);
#endif
#if defined(GL_BGRA)
    fsm_macro_magic(GL_BGRA,     GL_UNSIGNED_BYTE,        vgui_pixel_bgra8888);
#endif
#if defined(GL_EXT_abgr) || defined(GL_ABGR_EXT)
    fsm_macro_magic(GL_ABGR_EXT, GL_UNSIGNED_BYTE,        vgui_pixel_abgr8888);
#endif
    fsm_macro_end;
  }

  // 24bit rgb
  else if (pixel_format == VIL_RGB_BYTE) {
    fsm_macro_begin(vgui_pixel_rgb888, "24 bit RGB");
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_BYTE,        vgui_pixel_rgb888);
    fsm_macro_magic(GL_BGR,      GL_UNSIGNED_BYTE,        vgui_pixel_bgr888);
    fsm_macro_magic(GL_RGBA,     GL_UNSIGNED_BYTE,        vgui_pixel_rgba8888);
#if defined(GL_UNSIGNED_SHORT_5_6_5)
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_SHORT_5_6_5, vgui_pixel_rgb565);
#endif
#if defined(GL_UNSIGNED_SHORT_5_5_5_1)
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_SHORT_5_5_5_1, vgui_pixel_bgra5551);
#endif
#if defined(GL_BGRA)
    fsm_macro_magic(GL_BGRA,     GL_UNSIGNED_BYTE,        vgui_pixel_bgra8888);
#endif
#if defined(GL_EXT_abgr) || defined(GL_ABGR_EXT)
    fsm_macro_magic(GL_ABGR_EXT, GL_UNSIGNED_BYTE,        vgui_pixel_abgr8888);
#endif
    fsm_macro_end;
  }

  // float rgb
  else if (pixel_format == VIL_RGB_FLOAT) {
    fsm_macro_begin(vgui_pixel_rgbfloat, "float RGB");
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_BYTE,        vgui_pixel_rgb888);
    fsm_macro_magic(GL_BGR,      GL_UNSIGNED_BYTE,        vgui_pixel_bgr888);
    fsm_macro_magic(GL_RGBA,     GL_UNSIGNED_BYTE,        vgui_pixel_rgba8888);
#if defined(GL_UNSIGNED_SHORT_5_6_5)
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_SHORT_5_6_5, vgui_pixel_rgb565);
#endif
#if defined(GL_UNSIGNED_SHORT_5_5_5_1)
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_SHORT_5_5_5_1, vgui_pixel_bgra5551);
#endif
#if defined(GL_BGRA)
    fsm_macro_magic(GL_BGRA,     GL_UNSIGNED_BYTE,        vgui_pixel_bgra8888);
#endif
#if defined(GL_EXT_abgr) || defined(GL_ABGR_EXT)
    fsm_macro_magic(GL_ABGR_EXT, GL_UNSIGNED_BYTE,        vgui_pixel_abgr8888);
#endif
    fsm_macro_end;
  }

  // 32bit rgba
  else if (pixel_format == VIL_RGBA_BYTE) {
    fsm_macro_begin(vgui_pixel_rgba8888, "32 bit RGBA");
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_BYTE,        vgui_pixel_rgb888);
    fsm_macro_magic(GL_BGR,      GL_UNSIGNED_BYTE,        vgui_pixel_bgr888);
    fsm_macro_magic(GL_RGBA,     GL_UNSIGNED_BYTE,        vgui_pixel_rgba8888);
#if defined(GL_UNSIGNED_SHORT_5_6_5)
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_SHORT_5_6_5, vgui_pixel_rgb565);
#endif
#if defined(GL_UNSIGNED_SHORT_5_5_5_1)
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_SHORT_5_5_5_1, vgui_pixel_bgra5551);
#endif
#if defined(GL_BGRA)
    fsm_macro_magic(GL_BGRA,     GL_UNSIGNED_BYTE,        vgui_pixel_bgra8888);
#endif
#if defined(GL_EXT_abgr) || defined(GL_ABGR_EXT)
    fsm_macro_magic(GL_ABGR_EXT, GL_UNSIGNED_BYTE,        vgui_pixel_abgr8888);
#endif
    fsm_macro_end;
  }

  // 32bit float
  else if (pixel_format == VIL_FLOAT) {
    fsm_macro_begin(float, "32 bit float");
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_BYTE,        vgui_pixel_rgb888);
    fsm_macro_magic(GL_BGR,      GL_UNSIGNED_BYTE,        vgui_pixel_bgr888);
    fsm_macro_magic(GL_RGBA,     GL_UNSIGNED_BYTE,        vgui_pixel_rgba8888);
#if defined(GL_UNSIGNED_SHORT_5_6_5)
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_SHORT_5_6_5, vgui_pixel_rgb565);
#endif
#if defined(GL_UNSIGNED_SHORT_5_5_5_1)
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_SHORT_5_5_5_1, vgui_pixel_bgra5551);
#endif
#if defined(GL_BGRA)
    fsm_macro_magic(GL_BGRA,     GL_UNSIGNED_BYTE,        vgui_pixel_bgra8888);
#endif
#if defined(GL_EXT_abgr) || defined(GL_ABGR_EXT)
    fsm_macro_magic(GL_ABGR_EXT, GL_UNSIGNED_BYTE,        vgui_pixel_abgr8888);
#endif
    fsm_macro_end;
  }

  // IEEE double
  else if (pixel_format == VIL_DOUBLE) {
    fsm_macro_begin(double, "64 bit double");
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_BYTE,        vgui_pixel_rgb888);
    fsm_macro_magic(GL_BGR,      GL_UNSIGNED_BYTE,        vgui_pixel_bgr888);
    fsm_macro_magic(GL_RGBA,     GL_UNSIGNED_BYTE,        vgui_pixel_rgba8888);
#if defined(GL_UNSIGNED_SHORT_5_6_5)
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_SHORT_5_6_5, vgui_pixel_rgb565);
#endif
#if defined(GL_UNSIGNED_SHORT_5_5_5_1)
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_SHORT_5_5_5_1, vgui_pixel_bgra5551);
#endif
#if defined(GL_BGRA)
    fsm_macro_magic(GL_BGRA,     GL_UNSIGNED_BYTE,        vgui_pixel_bgra8888);
#endif
#if defined(GL_EXT_abgr) || defined(GL_ABGR_EXT)
    fsm_macro_magic(GL_ABGR_EXT, GL_UNSIGNED_BYTE,        vgui_pixel_abgr8888);
#endif
    fsm_macro_end;
  }

  // dunno.
  else
    {
      vcl_cerr << "pixel_format == " << vil_print(pixel_format) << " which is unknown...\n";
      assert(false);
    }

  if (debug || !section_ok)
    vcl_cerr << (section_ok ? "section ok" : "section bad") << vcl_endl;

  if (is_texture)
    image_id_ = vgui_cache_wizard::Instance()->load_image(image);
}

//------------------------------------------------------------------------------

//: just draw the outline of the given region.
bool vgui_section_buffer::draw_as_rectangle(float x0, float y0,  float x1, float y1) const
{
  glColor3i(0, 1, 0); // is green good for everyone?
  glLineWidth(1);
  glBegin(GL_LINE_LOOP);
  glVertex2f(x0, y0);
  glVertex2f(x1, y0);
  glVertex2f(x1, y1);
  glVertex2f(x0, y1);
  glEnd();
  return true;
}

//: draw the given region using glDrawPixels(), possibly accelerated.
bool vgui_section_buffer::draw_as_image(float x0, float y0,  float x1, float y1) const
{
  if (!section_ok) {
    vgui_macro_warning << "bad section in draw_as_image()\n";
    return draw_as_rectangle(x0, y0, x1, y1);
  }

  // this doesn't actually work yet if x0 or y0 are non-zero.
  return vgui_section_render(the_pixels,
                             allocw, alloch,
                             x0,y0, x1, y1,
                             format, type /*, true*/);
}

// to make sure we reload the texture image each time a different vgui_section_buffer
// is called upon to render, we store in this variable a pointer to the last
// vgui_section_buffer which loaded its texture image.
static vgui_section_buffer const *last = 0;

bool vgui_section_buffer::texture_begin(bool force_load) const
{
  vgui_macro_report_errors;

  glEnable(GL_TEXTURE_2D);
  vgui_macro_report_errors;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  vgui_macro_report_errors;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  vgui_macro_report_errors;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  vgui_macro_report_errors;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  vgui_macro_report_errors;

  // decal
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  vgui_macro_report_errors;

  if (force_load || ::last != this) {
    // time to reload the textures
    vcl_cerr << "loading textures\n";

    // byte alignment :
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //vgui_macro_report_errors;

    // the loaded texture image must be a power of two.
    if (allocw != next_power_of_two(allocw))
      vgui_macro_warning << "allocw, " << allocw << ", is not a power of two\n";
    if (alloch != next_power_of_two(alloch))
      vgui_macro_warning << "alloch, " << alloch << ", is not a power of two\n";

    // specify the texture image.
    glTexImage2D(GL_TEXTURE_2D, // target
                 0,             // level
                 3,             // internalformat (use only RGB. ignore alpha channel)
                 allocw,        // NB: must be power of 2
                 alloch,        // NB: must be power of 2
                 0,             // border FIXME
                 format,
                 type,
                 the_pixels);
    vgui_macro_report_errors;

    // remember that it was this vgui_section_buffer which last loaded its texture.
    last = this;
  }

  // set coordinate s to clamp :
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  vgui_macro_report_errors;

  // set coordinate t to clamp :
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  vgui_macro_report_errors;

  // faster than linear ?
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  vgui_macro_report_errors;

  // fastest option ?
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  vgui_macro_report_errors;

  // decal
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  vgui_macro_report_errors;

  //
  glEnable(GL_TEXTURE_2D);
  glShadeModel(GL_FLAT);
  vgui_macro_report_errors;

  //glEnable(GL_DEPTH_TEST);
  //glDepthFunc(GL_LEQUAL);

  return true;
}

bool vgui_section_buffer::texture_end() const
{
  glDisable(GL_TEXTURE_2D);
  vgui_macro_report_errors;

  return true;
}

//: the section by texture mapping it onto a plane.
bool vgui_section_buffer::draw_as_texture(float x0, float y0,  float x1, float y1) const
{
  if (!section_ok) {
    vgui_macro_warning << "bad section in draw_as_texture()\n";
    return draw_as_rectangle(x0, y0, x1, y1);
  }

  if (!texture_begin())
    return false;

  float egx = float(w)/allocw;
  float egy = float(h)/alloch;
  glBegin(GL_QUADS);             // x    y    z
  glTexCoord2f(  0,  0); glVertex3f(x  , y  , 0);
  glTexCoord2f(egx,  0); glVertex3f(x+w, y  , 0);
  glTexCoord2f(egx,egy); glVertex3f(x+w, y+h, 0);
  glTexCoord2f(  0,egy); glVertex3f(x  , y+h, 0);
  glEnd();
  vgui_macro_report_errors;

  if (!texture_end())
    return false;

  return true;
}

// Loads all of the images as a series of textures. This means
// associating a texture name to each tile of the image to be
// rendered.
bool vgui_section_buffer::load_image_as_textures()
{
  vgui_macro_report_errors;

  glEnable(GL_TEXTURE_2D);
  vgui_macro_report_errors;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  vgui_macro_report_errors;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  vgui_macro_report_errors;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  vgui_macro_report_errors;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  vgui_macro_report_errors;

  // decal
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  vgui_macro_report_errors;

  vcl_cerr << "loading image as textures\n";

  // byte alignment :
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  //vgui_macro_report_errors;

  // the loaded texture image must be a power of two.
  if (allocw != next_power_of_two(allocw))
    vgui_macro_warning << "allocw, " << allocw << ", is not a power of two\n";
  if (alloch != next_power_of_two(alloch))
    vgui_macro_warning << "alloch, " << alloch << ", is not a power of two\n";

  // Inquire about maximum texture size
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texture_size);
  vcl_cerr << "Max texture size: " << texture_size << vcl_endl;
  if (texture_size>(int)allocw)
    texture_size = allocw;

  // release old texture names.
  if (tList) {
    glDeleteTextures(countw*counth, tList);
    delete [] tList;
    tList = 0;
  }

  // See how many texture_size X texture_size blocks is needed to cover
  // the whole image
  countw = (w + texture_size-1)/texture_size;
  counth = (h + texture_size-1)/texture_size;

  // Generate texture numbers. See OpenGL for details
  tList = new GLuint[countw*counth];
  glGenTextures(counth*countw, tList);

  // allocate a buffer for working with in this routine.
  vcl_vector<char> sub_image(texture_size*texture_size*4 + 1);
  char *orig_image = (char*)the_pixels;
  char *op_image = orig_image;

  // now do each tile in turn.
  for (int i = 0;i<counth;i++) {
    int resty; // number of rows to include in this tile.
    if ((i+1)*texture_size>(int)h)
      resty = h%texture_size;
    else
      resty = texture_size;

    op_image = orig_image;
    for (int j = 0;j<countw;j++) {
      if (debug)
        vcl_cerr << "Copying quadrant (" << i << "," << j << ")\n";

      int restx; // number of cols to include in this tile.
      if ((j+1)*texture_size>(int)w)
        restx = w%texture_size;
      else
        restx = texture_size;

      // copy into the sub_image buffer:
      for (int y = 0; y<resty; y++)
        vcl_memmove(/* xxx */&sub_image[0] + 4*y*texture_size,
                op_image         + 4*y*allocw, //4*texture_size*(countw-1)*y+4*restx*y,
                4*restx);
      op_image += 4*restx;

      // Load texture
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      vgui_macro_report_errors;

      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      vgui_macro_report_errors;

      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      vgui_macro_report_errors;

      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      vgui_macro_report_errors;

      glBindTexture(GL_TEXTURE_2D, tList[i*countw+j]);
      vgui_macro_report_errors;

      glTexImage2D(GL_TEXTURE_2D, // target
                   0,             // level
                   3,             // internalformat (use only RGB. ignore alpha channel)
                   texture_size,  // NB: must be power of 2
                   texture_size,  // NB: must be power of 2
                   0,             // border FIXME
                   format,
                   type,
                   /* xxx */&sub_image[0]);
      vgui_macro_report_errors;
    }
    orig_image += allocw*texture_size*4;
  }
  return true;
}

bool vgui_section_buffer::draw_image_as_textures() const
{
  static int last = -1;
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  vgui_macro_report_errors;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  vgui_macro_report_errors;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  vgui_macro_report_errors;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  vgui_macro_report_errors;

  // set coordinate s to clamp :
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  vgui_macro_report_errors;

  // set coordinate t to clamp :
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  vgui_macro_report_errors;

  // faster than linear ?
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  vgui_macro_report_errors;

  // fastest option ?
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  vgui_macro_report_errors;

  // decal
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  vgui_macro_report_errors;

  //
  glEnable(GL_TEXTURE_2D);
  glShadeModel(GL_FLAT);

  for (int i = 0;i<counth;i++) {
    for (int j = 0;j<countw;j++) {
      // Activate the texture provided its not already in memory
      if (last != (int)tList[i*countw+j]) {
        glBindTexture(GL_TEXTURE_2D, tList[i*countw+j]);
        last = tList[i*countw+j];
      }

      int x = j*texture_size;
      int y = i*texture_size;

      int tw,th;
      if ((j+1)*texture_size<(int)w)
        tw = texture_size;
      else
        tw = w-j*texture_size;
      if ((i+1)*texture_size<(int)h)
        th = texture_size;
      else
        th = h-i*texture_size;

      float egx = float(tw)/texture_size;
      float egy = float(th)/texture_size;

      glBegin(GL_QUADS);             // x     y     z
      glTexCoord2f(  0,  0); glVertex3f(x   , y   , 0);
      glTexCoord2f(egx,  0); glVertex3f(x+tw, y   , 0);
      glTexCoord2f(egx,egy); glVertex3f(x+tw, y+th, 0);
      glTexCoord2f(  0,egy); glVertex3f(x   , y+th, 0);
      glEnd();
      vgui_macro_report_errors;
    }
  }
  if (!texture_end())
    return false;

  return true;
}

bool vgui_section_buffer::draw_image_as_cached_textures(float x0, float y0,  float w, float h)
{
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  vgui_macro_report_errors;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  vgui_macro_report_errors;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  vgui_macro_report_errors;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  vgui_macro_report_errors;

  // set coordinate s to clamp :
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  vgui_macro_report_errors;

  // set coordinate t to clamp :
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  vgui_macro_report_errors;

  // faster than linear ?
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  vgui_macro_report_errors;

  // fastest option ?
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  vgui_macro_report_errors;

  // decal
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  vgui_macro_report_errors;

  //
  glEnable(GL_TEXTURE_2D);
  glShadeModel(GL_FLAT);
  texture_size = 256;
  vgui_cache_wizard::image_cache_quadrants quadrants;
  vgui_cache_wizard::dimension d,pos;
  vgui_cache_wizard::Instance()->get_section(image_id_,int(x0),int(y0),int(w),int(h),&quadrants,&pos,&d);
  vgui_cache_wizard::image_cache_quadrants::iterator el = quadrants.begin();
  for (int i = pos.second;i<=pos.second+d.second;i++) {
    for (int j = pos.first;j<=pos.first+d.first;j++) {
      // Activate the texture provided its not already in memory
      glBindTexture(GL_TEXTURE_2D, *el);

      int x = j*texture_size;
      int y = i*texture_size;

      int tw;
      if ((j+1)*texture_size<x0+w)
        tw = texture_size;
      else
        tw = int(x0+w-j*texture_size);

      int th;
      if ((i+1)*texture_size<y0+h)
        th = texture_size;
      else
        th = int(y0+h-i*texture_size);

      float egx = float(tw)/texture_size;
      float egy = float(th)/texture_size;

      glBegin(GL_QUADS);             // x     y     z
      glTexCoord2f(  0,  0); glVertex3f(x   , y   , 0);
      glTexCoord2f(egx,  0); glVertex3f(x+tw, y   , 0);
      glTexCoord2f(egx,egy); glVertex3f(x+tw, y+th, 0);
      glTexCoord2f(  0,egy); glVertex3f(x   , y+th, 0);
      glEnd();
      vgui_macro_report_errors;
      el++;
    }
  }
  if (!texture_end())
    return false;

  return true;
}

//------------------------------------------------------------------------------
