// This is core/vil1/vil1_save.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil1_save.h"

#include <vcl_cstring.h>
#include <vcl_iostream.h>

#include <vil1/vil1_new.h>
#include <vil1/vil1_open.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_copy.h>
#include <vil1/vil1_property.h>
#include <vil1/vil1_flipud.h>
#include <vil1/vil1_flip_components.h>

#include <vil1/vil1_rgb.h>
#include <vil1/vil1_memory_image_of.h>

//: Send vil1_image to disk.
bool vil1_save(vil1_image i, char const* filename, char const* file_format)
{
  vil1_stream* os = vil1_open(filename, "w");
  if (!os || !os->ok()) {
    vcl_cerr << __FILE__ ": Invalid stream for \"" << filename << "\"\n";
    return false;
  }
  vil1_image out = vil1_new(os, i.width(), i.height(), i, file_format);
  if (!out) {
    vcl_cerr << __FILE__ ": (vil1_save) Cannot save to type [" << file_format << "]\n";
    return false;
  }
  bool top_first, bgr;
  if (out.get_property(vil1_property_top_row_first, &top_first) && !top_first)
    i = vil1_flipud(i);
  if (i.components() == 3 && out.get_property(vil1_property_component_order_is_BGR, &bgr) && bgr)
    i = vil1_flip_components(i);
  vil1_copy(i, out);
  return true;
}

//: Send vil1_image to disk; preserve byte order.
bool vil1_save_raw(vil1_image const& i, char const* filename, char const* file_format)
{
  vil1_stream* os = vil1_open(filename, "w");
  return vil1_save_raw(i, os, file_format);
}

//: Send vil1_image_impl to output stream.
// The possible file_formats are defined by the subclasses of vil1_file_format
// in vil1_file_format.cxx
bool vil1_save_raw(vil1_image const& i, vil1_stream* os, char const* file_format)
{
  vil1_image out = vil1_new(os, i.width(), i.height(), i, file_format);

  if (!out) {
    vcl_cerr << __FILE__ ": (vil1_save_raw) Cannot save to type [" << file_format << "]\n";
    return false;
  }

  vil1_copy(i, out);

  return true;
}

static
char const *guess_file_format(char const* filename)
{
  char const *file_format = "pnm"; // default file format

  // find last "."
  char const *dot = vcl_strrchr(filename, '.');
  if (!dot) {
    // filename doesn't end in ".anything"
    vcl_cerr << __FILE__ ": assuming pnm format for \'" << filename << "\'\n";
    file_format = "pnm";
  }
  else {
    // translate common extensions into known file formats.
    if (false) { }
#define macro(ext, fmt) else if (!vcl_strcmp(dot, "." #ext)) file_format = #fmt
    macro(bmp, bmp);
    macro(pbm, pnm);
    macro(pgm, pnm);
    macro(ppm, pnm);
    macro(pnm, pnm);
    macro(jpg, jpeg);
    macro(jpeg, jpeg);
    macro(tiff, tiff);
    macro(tif, tiff);
    macro(mit, mit);
    macro(gif, gif);
    macro(png, png);
#undef macro
    else
      vcl_cerr << __FILE__ ": assuming pnm format for \'" << filename << "\'\n";
  }

  return file_format;
}

//: save to file, deducing format from filename.
bool vil1_save(vil1_image const& i, char const* filename)
{
  return vil1_save(i, filename, guess_file_format(filename));
}

// What's the point of these *_template functions? Why not just put
// the function template declaration in the header file?

template<class T>
static inline
void vil1_save_rgb_template(T const* p, int w, int h, vcl_string const& fn)
{
  vil1_memory_image_of<vil1_rgb<unsigned char> > out(w,h);
  unsigned char* o = (unsigned char*)out.get_buffer();
  T const* p_end = p + w*h*3;
  while (p != p_end)
    // possible loss of data! (e.g. clipping)
    // agap: but it's okay because the input values are in 0..255
    *o++ = (unsigned char)(*p++);
  vil1_save(out, fn.c_str());
}

template<class T>
static inline
void vil1_save_gray_template(T const* p, int w, int h, vcl_string const& fn)
{
  vil1_memory_image_of<unsigned char> out(w,h);
  unsigned char* o = out.get_buffer();
  T const* p_end = p + w*h;
  while (p != p_end)
    // possible loss of data! (e.g. clipping)
    // agap: but it's okay because the input values are in 0..255
    *o++ = (unsigned char)(*p++);
  vil1_save(out, fn.c_str());
}

//: Save raw unsigned chars, deducing format from filename
void vil1_save_gray(unsigned char const* p, int w, int h, vcl_string const& fn)
{
  vil1_save_gray_template(p, w, h, fn);
}

//: Save raw floats as gray.
// No scaling is performed, so values would be 0..255.
// File format is deduced from filename.
void vil1_save_gray(float const* p, int w, int h, vcl_string const& fn)
{
  vil1_save_gray_template(p, w, h, fn);
}

//: Save raw doubles as gray.
// No scaling is performed, so values would be 0..255.
// File format is deduced from filename.
void vil1_save_gray(double const* p, int w, int h, vcl_string const& fn)
{
  vil1_save_gray_template(p, w, h, fn);
}

//: Save raw RGB, deducing format from filename
void vil1_save_rgb(unsigned char const* p, int w, int h, vcl_string const& fn)
{
  vil1_save_rgb_template(p, w, h, fn);
}

//: Save raw floats as RGB.  See vil1_save_gray.
void vil1_save_rgb(float const* p, int w, int h, vcl_string const& fn)
{
  vil1_save_rgb_template(p, w, h, fn);
}

//: Save raw doubles as RGB.  See vil1_save_gray.
void vil1_save_rgb(double const* p, int w, int h, vcl_string const& fn)
{
  vil1_save_rgb_template(p, w, h, fn);
}
