// This is core/vil/file_formats/vil_bmp.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vil_bmp.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cstring.h>
#include <vil/vil_stream.h>
#include <vil/vil_property.h>
#include <vil/vil_memory_chunk.h>
#include <vil/vil_image_view.h>

#define where (vcl_cerr << __FILE__ ":" << __LINE__ << " : ")

//--------------------------------------------------------------------------------

vil_image_resource_sptr vil_bmp_file_format::make_input_image(vil_stream* vs)
{
  // Attempt to read header
  vil_bmp_file_header hdr;
  vs->seek(0L);
  hdr.read(vs);

  if ( hdr.signature_valid() )
    return new vil_bmp_image(vs);
  else
    return 0;
}

vil_image_resource_sptr vil_bmp_file_format::make_output_image(vil_stream* vs,
                                                               unsigned nx,
                                                               unsigned ny,
                                                               unsigned nplanes,
                                                               vil_pixel_format format)
{
  return new vil_bmp_image(vs, nx, ny, nplanes, format);
}

char const* vil_bmp_format_tag = "bmp";

char const* vil_bmp_file_format::tag() const
{
  return vil_bmp_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

char const* vil_bmp_image::file_format() const
{
  return vil_bmp_format_tag;
}

vil_bmp_image::vil_bmp_image(vil_stream* is)
  : is_(is)
  , bit_map_start(-1L)
  //, freds_colormap(0)
  //, local_color_map_(0)
{
  is_->ref();
  read_header();
}

bool vil_bmp_image::get_property(char const * tag, void * value) const
{
  if (vcl_strcmp(vil_property_quantisation_depth, tag)==0)
  {
    unsigned* depth =  static_cast<unsigned*>(value);
    *depth = core_hdr.bitsperpixel / nplanes();
    return true;
  }

  return false;
}

vil_bmp_image::vil_bmp_image(vil_stream* vs, unsigned nx, unsigned ny,
                             unsigned nplanes, vil_pixel_format format):
  is_(vs), bit_map_start(-1L)
{
  is_->ref();

  // FIXME - we only support 8 and 24 bpp; add support for 1, 4, 16 and 32 bpp
  assert(format == VIL_PIXEL_FORMAT_BYTE);
  assert(nplanes == 1 || nplanes == 3);

  // core_hdr.header_size is set up for us.
  core_hdr.width = nx;
  core_hdr.height = ny;
  core_hdr.planes = 1;

  core_hdr.bitsperpixel = 8 * nplanes;

  write_header();
}

vil_bmp_image::~vil_bmp_image()
{
#if 0
  // we must get rid of the local_color_map_;
  if (local_color_map_)
  {
    delete [] local_color_map_[0];
    delete [] local_color_map_[1];
    delete [] local_color_map_[2];
    delete local_color_map_;
  }

  if (freds_colormap)
  {
    delete [] freds_colormap[0];
    delete [] freds_colormap[1];
    delete [] freds_colormap[2];
    delete [] freds_colormap[3];
    delete [] freds_colormap;
    freds_colormap = 0;
  }
#endif

  is_->unref();
}

bool vil_bmp_image::read_header()
{
  // seek to beginning and read file header.
  is_->seek(0L);
  file_hdr.read(is_);
  if ( ! file_hdr.signature_valid() )
  {
    where <<  "File is not a valid BMP file\n";
    return false;
  }
#ifdef DEBUG
  file_hdr.print(vcl_cerr); // blather
#endif

  // read core header
  core_hdr.read(is_);
#ifdef DEBUG
  core_hdr.print(vcl_cerr); // blather
#endif
  // allowed values for bitsperpixel are 1 4 8 16 24 32;
  // currently we only support 8 and 24 - FIXME
  if ( core_hdr.bitsperpixel != 8 && core_hdr.bitsperpixel != 24 )
  {
    where << "BMP file has a non-supported pixel size of " << core_hdr.bitsperpixel << " bits\n";
    return false;
  }

  // determine whether or not there is an info header from
  // the size field.
  if (core_hdr.header_size == vil_bmp_core_header::disk_size)
  {
    // no info header.
  }
  else if (core_hdr.header_size == vil_bmp_core_header::disk_size + vil_bmp_info_header::disk_size)
  {
    // probably an info header. read it now.
    info_hdr.read(is_);
#ifdef DEBUG
    info_hdr.print(vcl_cerr); // blather
#endif
    if (info_hdr.compression)
    {
      where << "cannot cope with compression at the moment\n";
      return false;
    }
  }
  else
  {
    // urgh!
    where << "dunno about header_size " << core_hdr.header_size << '\n';
    return false;
  }

  // skip colormap info
  is_->seek(file_hdr.bitmap_offset); // === seek(is_->tell()+info_hdr.colormapsize);
#if 0
  // color map nonsense
  if (info_hdr.colormapsize ==0 && info_hdr.colorcount == 0)
  {
    // phew! no colour map.
  }
  else if (info_hdr.colormapsize == 256 && core_hdr.bitsperpixel == 8)
  {
    // In this case I know how to read the colormap because I have hexdumped an example.
    // But I ignore the color map in the get_view( Fix params) routine because I don't care.
    // fsm
    typedef unsigned char uchar;
    freds_colormap = new uchar *[4];
    freds_colormap[0] = new uchar[256];
    freds_colormap[1] = new uchar[256];
    freds_colormap[2] = new uchar[256];
    freds_colormap[3] = new uchar[256];
    uchar bif[4];
    for (int i=0; i<256; ++i)
    {
      is_->read(bif, sizeof(bif));
      freds_colormap[0][i] = bif[0];
      freds_colormap[1][i] = bif[1];
      freds_colormap[2][i] = bif[2];
      freds_colormap[3][i] = bif[3];
    }
  }
  else
  {
    // dunno about this.
    assert(false); // FIXME
  }
#endif

  // old colormap reading code. it's not clear whether or not it worked -- fsm.
#if 0
  // Determine the number of colors and set color map if necessary
  int ccount=0;

  if (header.biClrUsed != 0)
    ccount = header.biClrUsed;
  else if (header.biBitCount != 24)
    ccount = 1 << header.biBitCount;
  else
  {
  }

  if (ccount != 0)
  {
    unsigned cmap_size;
    if (header.biSize == sizeof(xBITMAPCOREHEADER))
      cmap_size = ccount*3;
    else
      cmap_size = ccount*4;

    vcl_vector<uchar> cmap(cmap_size, 0); // use vector<> to avoid coreleak
    if (is_->read(/* xxx */&cmap[0], 1024L) != 1024L)
    {
      vcl_cerr << "Error reading image palette\n";
      return false;
    }

    // SetColorNum(ccount);
    // int ncolors = get_color_num();
    int ncolors = ccount; // good guess
    if (ncolors != 0)
    {
      int **color_map = new int*[3];
      for (int i=0; i<3; ++i)
      {
        color_map[i] = new int[ncolors];
        for (int j=0; j<ncolors; j++)
          color_map[i][j] = (int) cmap[2-i+4*j];
      }

      // SetColorMap(color_map);  - TODO find out where to save a color map
      local_color_map_=color_map;
    }
  }

  // TODO not obvious where the magic number is read
#endif

  // remember the position of the start of the bitmap data
  bit_map_start = is_->tell();
#ifdef DEBUG
  where << "bit_map_start = " << bit_map_start << '\n'; // blather
#endif
  assert(bit_map_start == (int)file_hdr.bitmap_offset); // I think they're supposed to be the same -- fsm.

  return true;
}

bool vil_bmp_image::write_header()
{
#ifdef DEBUG
  vcl_cerr << "Writing BMP header\n"
           << ni() << 'x' << nj() << '@'
           << nplanes() << 'x' <<
           vil_pixel_format_sizeof_components(pixel_format()) << '\n';
#endif

  int rowlen = ni() * nplanes() *
    vil_pixel_format_sizeof_components(pixel_format());
  rowlen += (3-(rowlen+3)%4); // round up to a multiple of 4
  int data_size = nj() * rowlen;

  if (nplanes() == 1)
    info_hdr.colorcount = info_hdr.colormapsize = 1<<
      vil_pixel_format_sizeof_components(pixel_format()) * 8;
  file_hdr.bitmap_offset = bit_map_start = 54L + 4 * info_hdr.colormapsize;
  file_hdr.file_size = bit_map_start+data_size;
  core_hdr.header_size = 40;
  core_hdr.width = ni();
  core_hdr.height = nj();
  core_hdr.bitsperpixel = nplanes()*
    vil_pixel_format_sizeof_components(pixel_format()) * 8;
  info_hdr.bitmap_size = data_size;

#ifdef DEBUG
  file_hdr.print(vcl_cerr);
  core_hdr.print(vcl_cerr); // blather
  info_hdr.print(vcl_cerr);
#endif
  is_->seek(0L);
  file_hdr.write(is_);
  core_hdr.write(is_);
  info_hdr.write(is_);
  if (nplanes() == 1) // Need to write a colourmap in this case
    for (int i=0; i<(1<<vil_pixel_format_sizeof_components(pixel_format())*8); ++i)
      for (int j=0; j<4; ++j)
      {
        unsigned char c = i;
        is_->write(&c,1L);
      }

  return true;
}

//------------------------------------------------------------
vil_image_view_base_sptr vil_bmp_image::get_copy_view(
  unsigned x0, unsigned nx, unsigned y0, unsigned ny) const
{
  if (x0+nx > ni() || y0+ny > nj()) return 0;
  //
  unsigned bytes_per_pixel = core_hdr.bitsperpixel / 8;
  assert(core_hdr.bitsperpixel == 8 || core_hdr.bitsperpixel == 24);
  // FIXME - add support for 1, 4, 16 and 32 bpp

  // actual number of bytes per raster in file.
  unsigned have_bytes_per_raster = ((bytes_per_pixel * core_hdr.width + 3)/4)*4;

  // number of bytes we want per raster.
  unsigned long want_bytes_per_raster = nx*bytes_per_pixel;

  if (nx == ni()) want_bytes_per_raster =  have_bytes_per_raster;

  vil_memory_chunk_sptr buf = new vil_memory_chunk(want_bytes_per_raster*ny, VIL_PIXEL_FORMAT_BYTE);

  // read each raster in turn. if the client wants the whole image, it may
  // be faster to read() it all in one chunk, so long as the number of bytes
  // per image raster is divisible by four (because the file rasters are
  // padded at the ends).
  if (nx == ni())
  {
    is_->seek(bit_map_start + have_bytes_per_raster*y0);
    is_->read(reinterpret_cast<vxl_byte *>(buf->data()), want_bytes_per_raster *ny);
  }
  else
  {
    for (unsigned i=0; i<ny; ++i)
    {
      is_->seek(bit_map_start + have_bytes_per_raster*(i+y0) + x0*bytes_per_pixel);
      is_->read(reinterpret_cast<vxl_byte *>(buf->data()) + want_bytes_per_raster*i, want_bytes_per_raster);
    }
  }

  return new vil_image_view<vxl_byte>(
    buf,
    reinterpret_cast<vxl_byte *>(buf->data())+(ny-1)*have_bytes_per_raster + nplanes()-1,
    nx, ny, nplanes(),
    nplanes(), -(long)have_bytes_per_raster, -1);
}


bool vil_bmp_image::put_view(const vil_image_view_base& view,
                             unsigned x0, unsigned y0)
{
  if (!view_fits(view, x0, y0))
  {
    vcl_cerr << "ERROR: " << __FILE__ << ":\n view does not fit\n";
    return false;
  }

  assert (view.pixel_format() == VIL_PIXEL_FORMAT_BYTE); // FIXME
  const vil_image_view<vxl_byte> & view2 = static_cast<const vil_image_view<vxl_byte> &>(view);

  unsigned bypp = nplanes();
  unsigned rowlen = ni() * bypp;
  rowlen += (3-(rowlen+3)%4); // round up to a multiple of 4

  if ((view2.planestep() == -1||nplanes()==1)&&
      view2.istep()==(int)view2.nplanes())
  {
    for (unsigned y=0; y<view2.nj(); ++y)
    {
      is_->seek(bit_map_start+(y+y0)*rowlen+x0*bypp);
      is_->write(&view2(0,view2.nj()-y-1,view2.nplanes()-1), rowlen);
    }
  }
  else
  {
    assert(nplanes()==3);
    vxl_byte* buf = new vxl_byte[rowlen];
    for (unsigned j=0; j<view2.nj(); ++j)
    {
      vxl_byte* b = buf;
      for (unsigned i=0; i<view2.ni(); ++i)
      {
        *(b++) = view2(i, view2.nj()-j-1, 2);
        *(b++) = view2(i, view2.nj()-j-1, 1);
        *(b++) = view2(i, view2.nj()-j-1, 0);
      }
      is_->seek(bit_map_start+(j+y0)*rowlen+x0*bypp);
      is_->write(buf, rowlen);
    }
    delete [] buf;
  }
  return true;
}
