// This is oxl/osl/osl_canny_smooth.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm
// see osl_canny_smooth.txx for template functions.

#include "osl_canny_smooth.h"
#include <vcl_cstdlib.h> // vcl_abort()
#include <vcl_iostream.h>
#include <vxl_config.h>
#include <vil1/vil1_rgb.h>
#include <vil1/vil1_pixel.h>
#include <vil1/vil1_memory_image_of.h>

#if 0
#include <vil1/vil1_save.h>
void debug_save(float const * const *image_out, int w, int h, char const *file) {
  vil1_memory_image_of<unsigned char> debug(w, h);
  for (unsigned i=0; i<h; ++i)
    for (unsigned j=0; j<w; ++j)
      debug[i][j] = (unsigned char) image_out[i][j];
  vcl_cerr << "save to " << file << vcl_endl;
  vil1_save(debug, file, "png");
}
#endif


//"osl_canny_smooth.cxx", line 37: Error: Overloading ambiguity between
// "osl_canny_smooth_rothwell<unsigned char>(const vil1_memory_image_of<unsigned char>&, const float*, int, int, float**)"
// and
// "osl_canny_smooth_rothwell<unsigned char>(const vil1_memory_image_of<unsigned char>&, const float*, int, int, float**)".

void osl_canny_smooth_rothwell(vil1_image const &image_in,
                               float const *kernel_, int width_, int k_size_,
                               float * const *smooth_)
{
  if (false) { }
#define macro(pt, st) \
  else if (vil1_pixel_format(image_in) == pt) { \
    vil1_memory_image_of<st > sec(image_in); \
    osl_canny_smooth_rothwell(const_cast<st const *const*>(sec.row_array()), sec.height(), sec.width(), \
                              kernel_, width_, k_size_, smooth_); \
  }
  macro(VIL1_BYTE, vxl_byte)
  macro(VIL1_UINT16, unsigned short)
  macro(VIL1_RGB_BYTE, vil1_rgb<unsigned char>)
  macro(VIL1_FLOAT, float)
#undef macro
  else {
    vcl_cerr << "Image must be either UBYTE, SHORT, RGB or FLOAT.\n";
    vcl_abort();
  }
}

void osl_canny_smooth_rothwell_adaptive(vil1_image const &image_in,
                                        int x0, int y0, int image_size,
                                        float const *kernel_, int width_, int k_size_,
                                        float * const *dx, float * const *dy, float * const *grad)
{
  if (false) { }
#define macro(pt, st) \
  else if (vil1_pixel_format(image_in) == pt) { \
    vil1_memory_image_of<st > sec(image_in); \
    osl_canny_smooth_rothwell_adaptive(const_cast<st const * const *>(sec.row_array()), sec.height(), sec.width(), \
                                       x0, y0, image_size, kernel_, width_, k_size_, dx, dy, grad); \
  }
  macro(VIL1_BYTE, vxl_byte)
  macro(VIL1_UINT16, unsigned short)
  macro(VIL1_RGB_BYTE, vil1_rgb<unsigned char>)
  macro(VIL1_FLOAT, float)
#undef macro
  else {
    vcl_cerr << "Image must be either UBYTE, SHORT, RGB or FLOAT.\n";
    vcl_abort();
  }
}

void osl_canny_smooth(vil1_image const &image_in,
                      float const *kernel_, int width_, float const *sub_area_OX_,
                      float * const * image_out)
{
  if (false) { }
#define macro(pt, st) \
  else if (vil1_pixel_format(image_in) == pt) { \
    vil1_memory_image_of<st > sec(image_in); \
    osl_canny_smooth(const_cast<st const * const *>(sec.row_array()), sec.height(), sec.width(), \
                     kernel_, width_, sub_area_OX_, image_out); \
  }
  macro(VIL1_BYTE, vxl_byte)
  macro(VIL1_UINT16, unsigned short)
  macro(VIL1_RGB_BYTE, vil1_rgb<unsigned char>)
  macro(VIL1_FLOAT, float)
#undef macro
  else {
    vcl_cerr << "Image must be either UBYTE, SHORT, RGB or FLOAT.\n";
    vcl_abort();
  }
}
