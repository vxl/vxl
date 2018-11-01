// This is core/vil/file_formats/vil_jpeg.cxx
//:
// \file
// \author fsm
// \date   17 Feb 2000
// \verbatim
//  Modifications
//     11 Oct 2002 Ian Scott - converted to vil
//     30 Mar 2007 Peter Vanroose - replaced deprecated vil_new_image_view_j_i_plane()
//\endverbatim

#include <iostream>
#include <cstring>
#include "vil_jpeg.h"
#include "vil_jpeg_source_mgr.h"
#include "vil_jpeg_decompressor.h"
#include "vil_jpeg_destination_mgr.h"
#include "vil_jpeg_compressor.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // vxl_byte

#include <vil/vil_stream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_exception.h>

//: the file probe, as a C function.
bool vil_jpeg_file_probe(vil_stream *vs)
{
  char magic[2];
  vs->seek(0L);
  vil_streampos n = vs->read(magic, sizeof(magic));

  if (n != sizeof(magic)) {
    std::cerr << __FILE__ << " : vil_stream::read() failed\n";
    return false;
  }

  // 0xFF followed by 0xD8
  return (magic[0] == char(0xFF)) && (magic[1] == char(0xD8));
}

// static data
static char const jpeg_string[] = "jpeg";

//--------------------------------------------------------------------------------
// class vil_jpeg_file_format

char const* vil_jpeg_file_format::tag() const
{
  return jpeg_string;
}

//:
vil_image_resource_sptr  vil_jpeg_file_format::make_input_image(vil_stream *vs)
{
  return vil_jpeg_file_probe(vs) ? new vil_jpeg_image(vs) : nullptr;
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
    std::cout<<"ERROR! vil_jpeg_file_format::make_output_image()\n"
            <<"Pixel format should be byte, but is "<<format<<" instead.\n";
    return nullptr;
  }
  return new vil_jpeg_image(vs, nx, ny, nplanes, format);
}

//--------------------------------------------------------------------------------

// class vil_jpeg_image

vil_jpeg_image::vil_jpeg_image(vil_stream *s)
  : jc(nullptr)
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

void vil_jpeg_image::set_quality(int quality)
{
  if( jc )
    jc->set_quality(quality);
}

vil_jpeg_image::vil_jpeg_image(vil_stream *s,
                               unsigned nx,
                               unsigned ny,
                               unsigned nplanes,
                               enum vil_pixel_format format)
  : jc(new vil_jpeg_compressor(s))
  , jd(nullptr)
  , stream(s)
{
  if (format != VIL_PIXEL_FORMAT_BYTE)
    std::cerr << "Sorry -- pixel format " << format << " not yet supported\n";
  assert(format == VIL_PIXEL_FORMAT_BYTE); // FIXME.

  stream->ref();

  // use same number of components as prototype, obviously.
  jc->jobj.input_components = nplanes;

  // store size
  jc->jobj.image_width = nx;
  jc->jobj.image_height = ny;
#ifdef DEBUG
  std::cerr << "w h = " << nx << ' ' << ny << '\n';
#endif
}

vil_jpeg_image::~vil_jpeg_image()
{
  // FIXME: I suspect there's a core leak here because jpeg_destroy() does not
  // free the vil_jpeg_stream_source_mgr allocated in vil_jpeg_stream_xxx_set()
  if (jd)
    delete jd;
  jd = nullptr;
  if (jc)
    delete jc;
  jc = nullptr;
  stream->unref();
  stream = nullptr;
}

//--------------------------------------------------------------------------------

//: decompressing from the vil_stream to a section buffer.
vil_image_view_base_sptr vil_jpeg_image::get_copy_view(unsigned x0,
                                                       unsigned nx,
                                                       unsigned y0,
                                                       unsigned ny) const
{
  if (!jd) {
    std::cerr << "attempted get_copy_view() failed -- no jpeg decompressor\n";
    return nullptr;
  }
#ifdef DEBUG
  std::cerr << "get_copy_view " << ' ' << x0 << ' ' << nx << ' ' << y0 << ' ' << ny << '\n';
#endif

  // number of bytes per pixel
  unsigned bpp = jd->jobj.output_components;

  vil_memory_chunk_sptr chunk = new vil_memory_chunk(bpp * nx * ny, pixel_format());

  for (unsigned int i=0; i<ny; ++i) {
    JSAMPLE const *scanline = jd->read_scanline(y0+i);
    if (!scanline)
      return nullptr; // failed

    std::memcpy(reinterpret_cast<char*>(chunk->data()) + i*nx*bpp, &scanline[x0*bpp], nx*bpp);
  }

  return new vil_image_view<vxl_byte>(chunk, reinterpret_cast<vxl_byte *>(chunk->data()), nx, ny, bpp, bpp, bpp*nx, 1);
}

//--------------------------------------------------------------------------------

//: compressing a section onto the vil_stream.
bool vil_jpeg_image::put_view(const vil_image_view_base &view,
                              unsigned x0, unsigned y0)
{

  if (!view_fits(view, x0, y0))
  {
    vil_exception_warning(vil_exception_out_of_bounds("vil_jpeg_image::put_view"));
    return false;
  }

  if (!jc) {
    std::cerr << "attempted put_view() failed -- no jpeg compressor\n";
    return false;
  }

  if (view.pixel_format() != VIL_PIXEL_FORMAT_BYTE)
  {
    std::cerr << "vil_jpeg_image::put_view() failed -- can only deal with byte images\n";
    return false;
  }

  const auto& view2 =
    static_cast<const vil_image_view<vxl_byte>&>(view);

  // "compression makes no sense unless the section covers the whole image."
  // Relaxed slightly.. awf.
  // It will work if you send entire scan lines sequentially
  if (x0 != 0 || view2.ni() != jc->jobj.image_width) {
    std::cerr << __FILE__ << " : Can only compress complete scanlines\n";
    return false;
  }
  if (y0 != jc->jobj.next_scanline) {
    std::cerr << __FILE__ << " : Scanlines must be sent sequentially\n";
    return false;
  }

  // write each scanline
  if ((view2.planestep() == 1 || view2.nplanes() == 1) && view2.istep() == jc->jobj.input_components)
  {
    assert(view2.istep() > 0);
    assert(view2.istep() == jc->jobj.input_components); // bytes per pixel in the section
    for (unsigned int j=0; j<view2.nj(); ++j) {
      auto const *scanline = (JSAMPLE const*)
        &view2(0,j);
      if (!jc->write_scanline(y0+j, scanline))
        return false;
    }
  }
  else
  {
    vil_memory_chunk_sptr chunk = new vil_memory_chunk(view2.ni()*view2.nplanes(), vil_pixel_format_component_format(vil_pixel_format_of(vxl_byte())));
    vil_image_view<vxl_byte> line = vil_image_view<vxl_byte>(chunk, reinterpret_cast<vxl_byte*>(chunk->data()), view2.ni(), 1, view2.nplanes(), view2.nplanes(), view2.nplanes()*view2.ni(), 1);
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


unsigned vil_jpeg_image::ni() const
{
  if (jd) return jd->jobj.output_width;
  if (jc) return jc->jobj.image_width;
  return 0;
}

unsigned vil_jpeg_image::nj() const
{
  if (jd) return jd->jobj.output_height;
  if (jc) return jc->jobj.image_height;
  return 0;
}

unsigned vil_jpeg_image::nplanes() const
{
  if (jd) return jd->jobj.output_components;
  if (jc) return jc->jobj.input_components;
  return 0;
}


vil_pixel_format vil_jpeg_image::pixel_format() const
{
  return VIL_PIXEL_FORMAT_BYTE;
}


char const *vil_jpeg_image::file_format() const
{
  return jpeg_string;
}

//--------------------------------------------------------------------------------
