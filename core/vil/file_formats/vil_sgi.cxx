// This is core/vil/file_formats/vil_sgi.cxx

#include <iostream>

#include <vil/vil_image_view.h>
#include <vil/vil_exception.h>
#include "vil_sgi.h"

#define where (std::cerr << __FILE__ " : " << __LINE__ << " : ")
#define SGI_HDR_SIZE 512

//--------------------------------------------------------------------------------

vil_image_resource_sptr vil_sgi_file_format::make_input_image(vil_stream* vs)
{
  // Attempt to read header
  vil_sgi_file_header hdr;
  vs->seek(0L);
  hdr.read(vs);

  if ( hdr.signature_valid() )
    return new vil_sgi_image(vs);
  else
    return nullptr;
}

vil_image_resource_sptr vil_sgi_file_format::make_output_image(vil_stream* vs,
                                                               unsigned nx,
                                                               unsigned ny,
                                                               unsigned nplanes,
                                                               vil_pixel_format format)
{
  return new vil_sgi_image(vs, nx, ny, nplanes, format);
}

char const* vil_sgi_format_tag = "sgi";

char const* vil_sgi_file_format::tag() const
{
  return vil_sgi_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

char const* vil_sgi_image::file_format() const
{
  return vil_sgi_format_tag;
}

vil_sgi_image::vil_sgi_image(vil_stream* is)
  : is_(is)
{
  is_->ref();
  if (!read_header())
    vil_exception_error(vil_exception_image_io("vil_sgi_image::read_header", "SGI", ""));
}

bool vil_sgi_image::get_property(char const *  /*tag*/, void *  /*value*/) const
{
  return true;
}

vil_sgi_image::vil_sgi_image(vil_stream* vs, unsigned nx, unsigned ny,
                             unsigned nplanes, vil_pixel_format format):
  is_(vs)
{
  if (format != VIL_PIXEL_FORMAT_BYTE)
  {
    vil_exception_warning(vil_exception_pixel_formats_incompatible(VIL_PIXEL_FORMAT_BYTE, format, "vil_sgi_image::vil_sgi_image"));
    //std::cerr << "Sorry -- pixel format " << format << " not yet supported\n";
    return;
  }
  if(nplanes != 1 && nplanes != 3 && nplanes != 4)
  {
    vil_exception_warning(vil_exception_unsupported_operation("vil_sgi_image::vil_sgi_image: invalid number of planes"));
    return;
  }

  assert(format == VIL_PIXEL_FORMAT_BYTE);
  assert(nplanes == 1 || nplanes == 3 || nplanes == 4);

  is_->ref();

  hdr.dimension = (nplanes == 1) ? 2 : 3; // dimension is 2 for one channel image, 3 otherwise
  hdr.xsize = nx;
  hdr.ysize = ny;
  hdr.zsize = nplanes;

  write_header();
}

vil_sgi_image::~vil_sgi_image()
{
  is_->unref();
}

unsigned vil_sgi_image::nplanes() const
{
  return hdr.zsize;
}

unsigned vil_sgi_image::ni() const
{
  return hdr.xsize;
}

unsigned vil_sgi_image::nj() const
{
  return hdr.ysize;
}

bool vil_sgi_image::read_header()
{
  // seek to beginning and read file header.
  is_->seek(0L);
  hdr.read(is_);
  if ( ! hdr.signature_valid() )
  {
    where <<  "File is not a valid SGI file\n";
    return false;
  }
#ifdef DEBUG
  hdr.print(std::cerr); // blather
#endif

  // SGI allows an arbitrary number of bits per pixel, but assume one byte per channel and 1, 3, or 4 channels.
  if ( hdr.bpc*hdr.zsize != 1 && hdr.bpc*hdr.zsize != 3 && hdr.bpc*hdr.zsize != 4 )
  {
    where << "SGI file has a non-supported pixel size of " << hdr.bpc*hdr.zsize*8 << " bits\n";
    return false;
  }

  // RLE should be added in - FIXME
  if ( hdr.storage == 1 )
  {
    where << "The RLE storage format is not yet supported for SGI images\n";
    return false;
  }

  // There are four colormap modes. Only mode 0 ("normal") is in widespread use. Mode 1 and 2 are obsolete, mode 3 is for SGI hardware.
  if ( hdr.colormap != 0 )
  {
    where << "Only colormap mode 0 is supported for SGI images\n";
    return false;
  }

  return true;
}

bool vil_sgi_image::write_header()
{
#ifdef DEBUG
  std::cerr << "Writing SGI header\n"
           << ni() << 'x' << nj() << '@'
           << nplanes() << 'x' <<
           vil_pixel_format_sizeof_components(pixel_format()) << '\n';
#endif

#ifdef DEBUG
  hdr.print(std::cerr); // blather
#endif
  is_->seek(0L);
  hdr.write(is_);

  return true;
}

//------------------------------------------------------------
vil_image_view_base_sptr vil_sgi_image::get_copy_view(
  unsigned x0, unsigned nx, unsigned y0, unsigned ny) const
{
  if (x0+nx > ni() || y0+ny > nj())
  {
    vil_exception_warning(vil_exception_out_of_bounds("vil_sgi_image::get_copy_view"));
    return nullptr;
  }

  // Number of bytes per pixel is equal to the number of channels
  vil_memory_chunk_sptr chunk = new vil_memory_chunk(nx*ny*nplanes(), VIL_PIXEL_FORMAT_BYTE);

  for(int i = 0; i < ny; i++)
  {
    is_->seek(SGI_HDR_SIZE+ni()*nplanes()*(nj()-1-y0-i)+x0*nplanes());
    is_->read(reinterpret_cast<vxl_byte *>(chunk->data()) + i*nx*nplanes(), nx*nplanes());
  }

  return new vil_image_view<vxl_byte>(chunk, reinterpret_cast<vxl_byte *>(chunk->data()), nx, ny, nplanes(), nplanes(), nx*nplanes(), 1);
}

bool vil_sgi_image::put_view(const vil_image_view_base& view,
                             unsigned x0, unsigned y0)
{
  if (!view_fits(view, x0, y0))
  {
    vil_exception_warning(vil_exception_out_of_bounds("vil_sgi_image::put_view"));
    return false;
  }

  if(view.pixel_format() != VIL_PIXEL_FORMAT_BYTE)
  {
    vil_exception_warning(vil_exception_pixel_formats_incompatible(VIL_PIXEL_FORMAT_BYTE, view.pixel_format(), "vil_sgi_image::put_view"));
    return false;
  }
  const auto & view2 = static_cast<const vil_image_view<vxl_byte> &>(view);

  for(int i = 0; i < view2.nj(); i++)
  {
    is_->seek(SGI_HDR_SIZE+ni()*nplanes()*(nj()-1-y0-i)+x0*nplanes());
    is_->write(&view2(0, i, view2.nplanes()-1), view2.ni()*view2.nplanes());
  }

  return true;
}
