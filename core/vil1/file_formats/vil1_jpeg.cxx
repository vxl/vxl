// This is vxl/vil/file_formats/vil_jpeg.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk
// \date   17 Feb 2000

#include "vil_jpeg.h"
#include "vil_jpeg_source_mgr.h"
#include "vil_jpeg_decompressor.h"
#include "vil_jpeg_destination_mgr.h"
#include "vil_jpeg_compressor.h"

#include <vcl_cassert.h>
#include <vcl_climits.h> // CHAR_BIT
#include <vcl_iostream.h>
#include <vcl_cstring.h> // memcpy()

#include <vil/vil_stream.h>
#include <vil/vil_image.h>
#include <vil/vil_property.h>

//: the file probe, as a C function.
bool vil_jpeg_file_probe(vil_stream *vs) {
  char magic[2];
  vs->seek(0L);
  int n = vs->read(magic, sizeof(magic));

  if (n != sizeof(magic)) {
    vcl_cerr << __FILE__ << " : vil_stream::read() failed\n";
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
  if (format != VIL_COMPONENT_FORMAT_UNSIGNED_INT)
    return 0;
  return new vil_jpeg_generic_image(vs, planes, width, height, components, bits_per_component, format);
}

//--------------------------------------------------------------------------------

// class vil_jpeg_generic_image

vil_jpeg_generic_image::vil_jpeg_generic_image(vil_stream *s)
  : jc(0)
  , jd(new vil_jpeg_decompressor(s))
  , stream(s)
{
  stream->ref();
}

bool vil_jpeg_generic_image::get_property(char const *tag, void *prop) const
{
  if (0==vcl_strcmp(tag, vil_property_top_row_first))
    return prop ? (*(bool*)prop) = true : true;

  if (0==vcl_strcmp(tag, vil_property_left_first))
    return prop ? (*(bool*)prop) = true : true;

  return false;
}

vil_jpeg_generic_image::vil_jpeg_generic_image(vil_stream *s,
                                               int planes,
                                               int width,
                                               int height,
                                               int components,
                                               int bits_per_component,
                                               vil_component_format format)
  : jc(new vil_jpeg_compressor(s))
  , jd(0)
  , stream(s)
{
  stream->ref();
  // warn
  if (planes != 1)
    vcl_cerr << __FILE__ " : prototype has != 1 planes. ignored\n";

  // use same number of components as prototype, obviously.
  jc->jobj.input_components = components;

  // store size
  jc->jobj.image_width = width;
  jc->jobj.image_height = height;
  //vcl_cerr << "w h = " << jobj.image_width << ' ' << jobj.image_height << vcl_endl;

  assert(bits_per_component == CHAR_BIT); // FIXME.
  assert(format == VIL_COMPONENT_FORMAT_UNSIGNED_INT); // FIXME
}

vil_jpeg_generic_image::~vil_jpeg_generic_image() {
  // FIXME: I suspect there's a core leak here because jpeg_destroy() does not
  // free the vil_jpeg_stream_source_mgr allocated in vil_jpeg_stream_xxx_set()
  if (jd)
    delete jd;
  jd = 0;
  if (jc)
    delete jc;
  jc = 0;
  stream->unref();
  stream = 0;
}

//--------------------------------------------------------------------------------

//: decompressing from the vil_stream to a section buffer.
bool vil_jpeg_generic_image::get_section(void *buf, int x0, int y0, int w, int h) const {
  if (!jd) {
    vcl_cerr << "attempted put_section() failed -- no jpeg decompressor\n";
    return false;
  }
  //vcl_cerr << "get_section " << buf << ' ' << x0 << ' ' << y0 << ' ' << w << ' ' << h << vcl_endl;

  // number of bytes per pixel
  unsigned bpp = jd->jobj.output_components;

  //
  for (int i=0; i<h; ++i) {
    JSAMPLE const *scanline = jd->read_scanline(y0+i);
    if (!scanline)
      return false; // failed
    vcl_memcpy(static_cast<char*>(buf) + i*w*bpp, &scanline[x0*bpp], w*bpp);
  }

  return true;
}

//--------------------------------------------------------------------------------

//: compressing a section onto the vil_stream.
bool vil_jpeg_generic_image::put_section(void const *buf, int x0, int y0, int w, int h) {
  if (!jc) {
    vcl_cerr << "attempted get_section() failed -- no jpeg compressor\n";
    return false;
  }

  // "compression makes no sense unless the section covers the whole image."
  // Relaxed slightly.. awf.
  // It will work if you send entire scan lines sequentially
  if (x0 != 0 || (unsigned int)w != jc->jobj.image_width) {
    vcl_cerr << __FILE__ << " : Can only compress complete scanlines\n";
    return false;
  }
  if ((unsigned int)y0 != jc->jobj.next_scanline) {
    vcl_cerr << __FILE__ << " : Scanlines must be sent sequentially \n";
    return false;
  }

  // bytes per pixel in the section
  unsigned bpp = jc->jobj.input_components;

  // write each scanline
  for (int i=0; i<h; ++i) {
    JSAMPLE const *scanline = (JSAMPLE const*) ((char const*)buf + i*w*bpp);
    if (!jc->write_scanline(y0+i, scanline))
      return false;
  }

  return true;
}

//--------------------------------------------------------------------------------

//:
int vil_jpeg_generic_image::planes() const {
  return 1;
}

//:
int vil_jpeg_generic_image::width() const {
  if (jd) return jd->jobj.output_width;
  if (jc) return jc->jobj.image_width;
  return 0;
}

//:
int vil_jpeg_generic_image::height() const {
  if (jd) return jd->jobj.output_height;
  if (jc) return jc->jobj.image_height;
  return 0;
}

//:
int vil_jpeg_generic_image::components() const {
  if (jd) return jd->jobj.output_components;
  if (jc) return jc->jobj.input_components;
  return 0;
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

//--------------------------------------------------------------------------------
