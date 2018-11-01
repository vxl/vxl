// This is core/vil/file_formats/vil_bmp.cxx

#include <iostream>
#include <vector>
#include <cstring>
#include "vil_bmp.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_stream.h>
#include <vil/vil_property.h>
#include <vil/vil_memory_chunk.h>
#include <vil/vil_image_view.h>
#include <vil/vil_exception.h>

#define where (std::cerr << __FILE__ " : " << __LINE__ << " : ")

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
    return nullptr;
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
#if 0
  , freds_colormap(0)
  , local_color_map_(0)
#endif
{
  is_->ref();
  if (!read_header())
    vil_exception_error(vil_exception_image_io("vil_bmp_image::read_header", "BMP", ""));
}

bool vil_bmp_image::get_property(char const * tag, void * value) const
{
  if (std::strcmp(vil_property_quantisation_depth, tag)==0)
  {
    if (value)
      *static_cast<unsigned int*>(value) = core_hdr.bitsperpixel / nplanes();
    return true;
  }

  return false;
}

vil_bmp_image::vil_bmp_image(vil_stream* vs, unsigned nx, unsigned ny,
                             unsigned nplanes, vil_pixel_format format):
  is_(vs), bit_map_start(-1L)
{
  if (format != VIL_PIXEL_FORMAT_BYTE)
  {
    vil_exception_warning(vil_exception_pixel_formats_incompatible(VIL_PIXEL_FORMAT_BYTE, format, "vil_bmp_image::vil_bmp_image"));
    //std::cerr << "Sorry -- pixel format " << format << " not yet supported\n";
    return;
  }
  if(nplanes != 1 && nplanes != 3 && nplanes != 4)
  {
    vil_exception_warning(vil_exception_unsupported_operation("vil_bmp_image::vil_bmp_image: invalid number of planes"));
    //std::cerr << "Sorry -- pixel format " << format << " not yet supported\n";
    return;
  }

  // FIXME - we only support 8, 24 bpp, and 32bpp; add support for 1, 4, and 16 bpp
  assert(format == VIL_PIXEL_FORMAT_BYTE);
  assert(nplanes == 1 || nplanes == 3 || nplanes == 4);

  is_->ref();

  // core_hdr.header_size is set up for us.
  core_hdr.width = int(nx);
  core_hdr.height = -int(ny);  // use top-down approach
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

unsigned vil_bmp_image::nplanes() const
{
  return (core_hdr.bitsperpixel<24)?1:core_hdr.bitsperpixel/8;   // FIXME
}

unsigned vil_bmp_image::ni() const
{
  return (core_hdr.width>=0)?core_hdr.width:-core_hdr.width;  // width is signed integer
}

unsigned vil_bmp_image::nj() const
{
  return (core_hdr.height>=0)?core_hdr.height:-core_hdr.height; // height is signed integer
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
  file_hdr.print(std::cerr); // blather
#endif

  // read core header
  core_hdr.read(is_);
#ifdef DEBUG
  core_hdr.print(std::cerr); // blather
#endif
  // allowed values for bitsperpixel are 1 4 8 16 24 32;
  // currently we only support 8, 24, and 32 - FIXME
  if ( core_hdr.bitsperpixel != 8 && core_hdr.bitsperpixel != 24 && core_hdr.bitsperpixel != 32 )
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
    info_hdr.print(std::cerr); // blather
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

    std::vector<uchar> cmap(cmap_size, 0); // use vector<> to avoid coreleak
    if (is_->read(/* xxx */&cmap[0], 1024L) != 1024L)
    {
      std::cerr << "Error reading image palette\n";
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
  return bit_map_start == (int)file_hdr.bitmap_offset; // I think they're supposed to be the same -- fsm.
}

bool vil_bmp_image::write_header()
{
#ifdef DEBUG
  std::cerr << "Writing BMP header\n"
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
  file_hdr.bitmap_offset = 54L + 4 * info_hdr.colormapsize;
  bit_map_start = file_hdr.bitmap_offset;
  file_hdr.file_size = file_hdr.bitmap_offset+data_size;
  core_hdr.header_size = 40;
  // already set
  //core_hdr.width = ni();
  //core_hdr.height = nj();
  core_hdr.bitsperpixel = nplanes()*
    vil_pixel_format_sizeof_components(pixel_format()) * 8;
  info_hdr.bitmap_size = data_size;

#ifdef DEBUG
  file_hdr.print(std::cerr);
  core_hdr.print(std::cerr); // blather
  info_hdr.print(std::cerr);
#endif
  is_->seek(0L);
  file_hdr.write(is_);
  core_hdr.write(is_);
  info_hdr.write(is_);
  if (nplanes() == 1) // Need to write a colourmap in this case
    {
      unsigned int const n = 1<<vil_pixel_format_sizeof_components(pixel_format())*8;  // usually 256
      auto* map = new vxl_byte[n*4];
      vxl_byte* ptr = map;
      for (unsigned int i=0; i<n; ++i, ptr+=4)
      {
        for (unsigned int j=0; j<3; ++j)
        {
          auto c = (unsigned char)i;
          ptr[j] = c;
          //is_->write(&c,1L);
        }
        ptr[3] = 0; // unused byte
      }
      is_->write(map, n*4);
      delete [] map;
    }
  return true;
}

//------------------------------------------------------------
vil_image_view_base_sptr vil_bmp_image::get_copy_view(
  unsigned x0, unsigned nx, unsigned y0, unsigned ny) const
{
  if (x0+nx > ni() || y0+ny > nj())
  {
    vil_exception_warning(vil_exception_out_of_bounds("vil_bmp_image::get_copy_view"));
    return nullptr;
  }
  //
  unsigned const bytes_per_pixel = core_hdr.bitsperpixel / 8;
  assert(core_hdr.bitsperpixel == 8 || core_hdr.bitsperpixel == 24 || core_hdr.bitsperpixel == 32 );
  // FIXME - add support for 1, 4, and 16 bpp

  // actual number of bytes per raster in file.
  unsigned const have_bytes_per_raster = ((bytes_per_pixel * core_hdr.width + 3)/4)*4;

  // number of bytes we want per raster.
  unsigned long want_bytes_per_raster = nx*bytes_per_pixel;

  // The following line results in non-contiguous image view and waste a bit memory
  // if (nx == ni()) want_bytes_per_raster =  have_bytes_per_raster;

  vil_memory_chunk_sptr buf = new vil_memory_chunk(want_bytes_per_raster*ny, VIL_PIXEL_FORMAT_BYTE);

  std::ptrdiff_t top_left_y0_in_mem = 0;
  std::ptrdiff_t  ystep = want_bytes_per_raster;
  unsigned int rows_to_skip = y0;
  if( core_hdr.height > 0 )
  {
    // Bottom-up pass
    // BMP images are stored with a flipped y-axis w.r.t. conventional
    // pixel axes.
    //
    rows_to_skip = nj() - (y0+ny);
    top_left_y0_in_mem = (ny-1)*want_bytes_per_raster;
    ystep = -ystep;
  }
  else
  {
    // Top-bottom pass
    // already init.
    //top_left_y0_in_mem = 0;
    //y0 = y0;
  }

  // read each raster in turn. if the client wants the whole image, it may
  // be faster to read() it all in one chunk, so long as the number of bytes
  // per image raster is divisible by four (because the file rasters are
  // padded at the ends).
  vil_streampos bytes_read = 0;
  if (nx == ni() && want_bytes_per_raster == have_bytes_per_raster)
  {
    is_->seek(bit_map_start + have_bytes_per_raster*rows_to_skip);
    bytes_read = is_->read(reinterpret_cast<vxl_byte *>(buf->data()), want_bytes_per_raster *ny);
  }
  else
  {
    for (unsigned i=0; i<ny; ++i)
    {
      is_->seek(bit_map_start + have_bytes_per_raster*(i+rows_to_skip) + x0*bytes_per_pixel);
      bytes_read += is_->read(reinterpret_cast<vxl_byte *>(buf->data()) + want_bytes_per_raster*i, want_bytes_per_raster);
    }
  }
  if (bytes_read != vil_streampos(ny * want_bytes_per_raster))
  {
    vil_exception_warning(
      vil_exception_corrupt_image_file("vil_bmp_image::get_copy_view", "BMP", ""));
    return nullptr;
  }

  unsigned np = 1;
  std::ptrdiff_t plane_step = 1;
  std::ptrdiff_t top_left_plane0_in_mem = 0;
  if( core_hdr.bitsperpixel == 8 )
  {
    np = 1;
    plane_step = 1;
    top_left_plane0_in_mem = 0;
  }
  else if( core_hdr.bitsperpixel == 24 )
  {
    //return new vil_image_view<vxl_byte>(
    //  buf,
    //  reinterpret_cast<vxl_byte *>(buf->data())+(ny-1)*want_bytes_per_raster + nplanes()-1,
    //  nx, ny, nplanes(),
    //  nplanes(), -(long)want_bytes_per_raster, -1/*correspond to BB GG RR*/);
    np = 3;
    plane_step = -1; /*correspond to BB GG RR*/
    top_left_plane0_in_mem = 2;  // np-1
  }
  else if( core_hdr.bitsperpixel == 32 )
  {
    // re-organize channel ordering from BGRA to RGBA.
    // In other words,  swap B and R
    assert( (want_bytes_per_raster & 3) == 0 );  //  must be multiple of 4
    auto* data = reinterpret_cast<vxl_byte *>(buf->data());
    vxl_byte* const data_end = data+(want_bytes_per_raster*ny);
    for(; data!=data_end; data+=4)
    {
      // memory layout for pixel color values:
      // Form      BB GG RR AA BB GG RR AA ....
      // Change to RR GG BB AA RR GG BB AA ...
      std::swap(data[0], data[2]);
    }

    np = 4;
    plane_step = 1;
    top_left_plane0_in_mem = 0;
  }
  assert( np == nplanes() );

  return new vil_image_view<vxl_byte>(
    buf,
    reinterpret_cast<vxl_byte *>(buf->data())+top_left_plane0_in_mem+top_left_y0_in_mem,
    nx, ny, np,
    np, ystep, plane_step);
}


bool vil_bmp_image::put_view(const vil_image_view_base& view,
                             unsigned x0, unsigned y0)
{
  if (!view_fits(view, x0, y0))
  {
    vil_exception_warning(vil_exception_out_of_bounds("vil_bmp_image::put_view"));
    return false;
  }

  if(view.pixel_format() != VIL_PIXEL_FORMAT_BYTE)
  {
    vil_exception_warning(vil_exception_pixel_formats_incompatible(VIL_PIXEL_FORMAT_BYTE, view.pixel_format(), "vil_bmp_image::put_view"));
    return false;
  }
  const auto & view2 = static_cast<const vil_image_view<vxl_byte> &>(view);

  unsigned const bypp = nplanes();
  unsigned const rowlen = ni() * bypp;
  unsigned const padlen = (3-(rowlen+3)%4); // round row length up to a multiple of 4
  vxl_byte padding[3]={0, 0, 0};

  assert( core_hdr.height < 0 );  // we utilize only top-down scan

  if ((view2.planestep() == -1||nplanes()==1)&&
      view2.istep()==(int)view2.nplanes())
  {
    for (unsigned y=0; y<view2.nj(); ++y)
    {
      is_->seek(bit_map_start+(y+y0)*(rowlen+padlen)+x0*bypp);
      is_->write(&view2(0,y,view2.nplanes()-1), rowlen);
      if (padlen !=0) is_->write(padding, padlen);
    }
  }
  else if (nplanes()==3)
  {
    assert(nplanes()==3);
    auto* buf = new vxl_byte[rowlen+padlen];
    for (unsigned i=rowlen; i<rowlen+padlen; ++i) buf[i]=0;
    for (unsigned j=0; j<view2.nj(); ++j)
    {
      vxl_byte* b = buf;
      //unsigned int const negj = view2.nj()-j-1;
      for (unsigned i=0; i<view2.ni(); ++i)
      {
        *(b++) = view2(i, j, 2);  //B
        *(b++) = view2(i, j, 1);  //G
        *(b++) = view2(i, j, 0);  //R
      }
      is_->seek(bit_map_start+(j+y0)*(rowlen+padlen)+x0*bypp);
      is_->write(buf, rowlen+padlen);
    }
    delete [] buf;
  }
  else /*nplanes()==4*/
  {
    assert(nplanes()==4);
    auto* buf = new vxl_byte[rowlen+padlen];
    for (unsigned i=rowlen; i<rowlen+padlen; ++i) buf[i]=0;
    for (unsigned j=0; j<view2.nj(); ++j)
    {
      vxl_byte* b = buf;
      //unsigned int const negj = view2.nj()-j-1;
      for (unsigned i=0; i<view2.ni(); ++i)
      {
        *(b++) = view2(i, j, 2);  //B
        *(b++) = view2(i, j, 1);  //G
        *(b++) = view2(i, j, 0);  //R
        *(b++) = view2(i, j, 3);  //A
      }
      is_->seek(bit_map_start+(j+y0)*(rowlen+padlen)+x0*bypp);
      is_->write(buf, rowlen+padlen);
    }
    delete [] buf;
  }
  return true;
}
