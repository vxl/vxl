// This is core/vil/file_formats/vil_jpeg.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm
// \date   17 Feb 2000
// \verbatim
//  Modifications
//     11 Oct 2002 Ian Scott - converted to vil
//\endverbatim

#include "vil_jpeg.h"
#include "vil_jpeg_source_mgr.h"
#include "vil_jpeg_decompressor.h"
#include "vil_jpeg_destination_mgr.h"
#include "vil_jpeg_compressor.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_cstring.h> // memcpy()
#include <vxl_config.h> // vxl_byte

#include <vil/vil_stream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>

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
  return (magic[0] == char(0xFF)) && (magic[1] == char(0xD8));
}

// static data
static char const jpeg_string[] = "jpeg";

//--------------------------------------------------------------------------------
// class vil_jpeg_file_format

char const* vil_jpeg_file_format::tag() const {
  return jpeg_string;
}

//:
vil_image_resource_sptr  vil_jpeg_file_format::make_input_image(vil_stream *vs) {
  return vil_jpeg_file_probe(vs) ? new vil_jpeg_image(vs) : 0;
}

vil_image_resource_sptr
  vil_jpeg_file_format::make_output_image(vil_stream* vs,
                                          unsigned nx,
                                          unsigned ny,
                                          unsigned nplanes,
                                          enum vil_pixel_format format)
{
  if (format != VIL_PIXEL_FORMAT_BYTE)
  {
    vcl_cout<<"ERROR! vil_jpeg_file_format::make_output_image()\n"
            <<"Pixel format should be byte, but is "<<format<<" instead.\n";
    return 0;
  }
  return new vil_jpeg_image(vs, nx, ny, nplanes, format);
}

//--------------------------------------------------------------------------------

// class vil_jpeg_image

vil_jpeg_image::vil_jpeg_image(vil_stream *s)
  : jc(0)
  , jd(new vil_jpeg_decompressor(s))
  , stream(s)
{
  stream->ref();
}

bool vil_jpeg_image::get_property(char const * /*tag*/, void * /*prop*/) const
{
  // This is not an in-memory image type, nor is it read-only:
  return false;
}

vil_jpeg_image::vil_jpeg_image(vil_stream *s,
                               unsigned nx,
                               unsigned ny,
                               unsigned nplanes,
                               enum vil_pixel_format format)
  : jc(new vil_jpeg_compressor(s))
  , jd(0)
  , stream(s)
{
  stream->ref();

  // use same number of components as prototype, obviously.
  jc->jobj.input_components = nplanes;

  // store size
  jc->jobj.image_width = nx;
  jc->jobj.image_height = ny;
  //vcl_cerr << "w h = " << jobj.image_width << ' ' << jobj.image_height << vcl_endl;

  assert(format == VIL_PIXEL_FORMAT_BYTE); // FIXME.
}

vil_jpeg_image::~vil_jpeg_image() {
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
vil_image_view_base_sptr vil_jpeg_image::get_copy_view(unsigned x0,
                                                       unsigned nx,
                                                       unsigned y0,
                                                       unsigned ny) const {
  if (!jd) {
    vcl_cerr << "attempted get_copy_view() failed -- no jpeg decompressor\n";
    return 0;
  }
  //vcl_cerr << "get_copy_view " << ' ' << x0 << ' ' << nx << ' ' << y0 << ' ' << ny << vcl_endl;

  // number of bytes per pixel
  unsigned bpp = jd->jobj.output_components;

  vil_memory_chunk_sptr chunk = new vil_memory_chunk(bpp * nx * ny, pixel_format());

  for (unsigned int i=0; i<ny; ++i) {
    JSAMPLE const *scanline = jd->read_scanline(y0+i);
    if (!scanline)
      return 0; // failed

    vcl_memcpy(reinterpret_cast<char*>(chunk->data()) + i*nx*bpp, &scanline[x0*bpp], nx*bpp);
  }

  return new vil_image_view<vxl_byte>(chunk, reinterpret_cast<vxl_byte *>(chunk->data()), nx, ny, bpp, bpp, bpp*nx, 1);
}

//--------------------------------------------------------------------------------

//: compressing a section onto the vil_stream.
bool vil_jpeg_image::put_view(const vil_image_view_base &view,
                              unsigned x0, unsigned y0)
{
  if (!jc) {
    vcl_cerr << "attempted put_view() failed -- no jpeg compressor\n";
    return false;
  }

  if (view.pixel_format() != VIL_PIXEL_FORMAT_BYTE)
  {
    vcl_cerr << "vil_jpeg_image::put_view() failed -- can only deal with byte images\n";
    return false;
  }

  const vil_image_view<vxl_byte>& view2 =
    static_cast<const vil_image_view<vxl_byte>&>(view);

  // "compression makes no sense unless the section covers the whole image."
  // Relaxed slightly.. awf.
  // It will work if you send entire scan lines sequentially
  if (x0 != 0 || view2.ni() != jc->jobj.image_width) {
    vcl_cerr << __FILE__ << " : Can only compress complete scanlines\n";
    return false;
  }
  if (y0 != jc->jobj.next_scanline) {
    vcl_cerr << __FILE__ << " : Scanlines must be sent sequentially\n";
    return false;
  }

  // write each scanline
  if (view2.planestep() == 1 || view2.nplanes() == 1)
  {
    assert(view2.istep() > 0);
    assert(view2.istep() == jc->jobj.input_components); // bytes per pixel in the section
    for (unsigned int j=0; j<view2.nj(); ++j) {
      JSAMPLE const *scanline = (JSAMPLE const*)
        &view2(0,j);
      if (!jc->write_scanline(y0+j, scanline))
        return false;
    }
  }
  else
  {
    vil_image_view<vxl_byte> line = vil_new_image_view_j_i_plane(
      view2.ni(), 1, view2.nplanes(), vxl_byte());
    JSAMPLE *scanline = line.top_left_ptr();

    for (unsigned int j=0; j<view2.nj(); ++j)
    {
      // arrange data into componentwise form.
      for (unsigned i = 0; i < view2.ni(); ++i)
        for (unsigned p = 0; p < view2.nplanes(); ++p)
          line(i,0,p) = view2(i,j,p);
      if (!jc->write_scanline(y0+j, scanline))
        return false;
    }
  }

  return true;
}

//--------------------------------------------------------------------------------


unsigned vil_jpeg_image::ni() const {
  if (jd) return jd->jobj.output_width;
  if (jc) return jc->jobj.image_width;
  return 0;
}

unsigned vil_jpeg_image::nj() const {
  if (jd) return jd->jobj.output_height;
  if (jc) return jc->jobj.image_height;
  return 0;
}

unsigned vil_jpeg_image::nplanes() const {
  if (jd) return jd->jobj.output_components;
  if (jc) return jc->jobj.input_components;
  return 0;
}


vil_pixel_format vil_jpeg_image::pixel_format() const {
  return VIL_PIXEL_FORMAT_BYTE;
}


char const *vil_jpeg_image::file_format() const {
  return jpeg_string;
}

//--------------------------------------------------------------------------------
