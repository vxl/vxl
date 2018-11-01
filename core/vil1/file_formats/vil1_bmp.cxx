// This is core/vil1/file_formats/vil1_bmp.cxx

#include <iostream>
#include <vector>
#include <cstring>
#include "vil1_bmp.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil1/vil1_stream.h>
#include <vil1/vil1_property.h>

#define where (std::cerr << __FILE__ ":" << __LINE__ << " : ")

//--------------------------------------------------------------------------------

char const* vil1_bmp_format_tag = "bmp";

vil1_image_impl* vil1_bmp_file_format::make_input_image(vil1_stream* is)
{
  // Attempt to read header
  vil1_bmp_file_header hdr;
  is->seek(0L);
  hdr.read(is);

  if ( hdr.signature_valid() )
    return new vil1_bmp_generic_image(is);
  else
    return nullptr;
}

vil1_image_impl* vil1_bmp_file_format::make_output_image(vil1_stream* is, int planes,
                                                         int width,
                                                         int height,
                                                         int components,
                                                         int bits_per_component,
                                                         vil1_component_format format)
{
  return new vil1_bmp_generic_image(is, planes, width, height, components, bits_per_component, format);
}

char const* vil1_bmp_file_format::tag() const
{
  return vil1_bmp_format_tag;
}

/////////////////////////////////////////////////////////////////////////////

char const* vil1_bmp_generic_image::file_format() const
{
  return vil1_bmp_format_tag;
}

vil1_bmp_generic_image::vil1_bmp_generic_image(vil1_stream* is)
  : is_(is)
  , bit_map_start(-1L)
  //, freds_colormap(0)
  //, local_color_map_(0)
{
  is_->ref();
  read_header();
}

bool vil1_bmp_generic_image::get_property(char const *tag, void *prop) const
{
  if (0==std::strcmp(tag, vil1_property_top_row_first))
    return prop ? (*(bool*)prop) = false, true : true;

  if (0==std::strcmp(tag, vil1_property_left_first))
    return prop ? (*(bool*)prop) = true : true;

  if (0==std::strcmp(tag, vil1_property_component_order_is_BGR))
    return prop ? (*(bool*)prop) = true : true;

  return false;
}

vil1_bmp_generic_image::vil1_bmp_generic_image(vil1_stream* is,
                                               int planes,
                                               int width,
                                               int height,
                                               int components,
                                               int bits_per_component,
                                               vil1_component_format /*format*/)
  : is_(is)
  , bit_map_start(-1L)
{
  is_->ref();
  assert(planes == 1); // FIXME

  // core_hdr.header_size is set up for us.
  core_hdr.width = width;
  core_hdr.height = height;
  core_hdr.planes = planes;
  // FIXME - we only support 8 and 24 bpp; add support for 1, 4, 16 and 32 bpp
  assert(bits_per_component == 8 && (components == 1 || components == 3));
  core_hdr.bitsperpixel = bits_per_component * components;

  write_header();
}

vil1_bmp_generic_image::~vil1_bmp_generic_image()
{
#if 0
  // we must get rid of the local_color_map_;
  if (local_color_map_){
    delete [] local_color_map_[0];
    delete [] local_color_map_[1];
    delete [] local_color_map_[2];
    delete local_color_map_;
  }

  if (freds_colormap) {
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

bool vil1_bmp_generic_image::read_header()
{
  // seek to beginning and read file header.
  is_->seek(0L);
  file_hdr.read(is_);
  if ( ! file_hdr.signature_valid() ) {
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
  // currently we only support 8 and 24 - FIXME
  if ( core_hdr.bitsperpixel != 8 && core_hdr.bitsperpixel != 24 ) {
    where << "BMP file has a non-supported pixel size of " << core_hdr.bitsperpixel << " bits\n";
    return false;
  }

  // determine whether or not there is an info header from
  // the size field.
  if (core_hdr.header_size == vil1_bmp_core_header::disk_size) {
    // no info header.
  }
  else if (core_hdr.header_size == vil1_bmp_core_header::disk_size + vil1_bmp_info_header::disk_size) {
    // probably an info header. read it now.
    info_hdr.read(is_);
#ifdef DEBUG
    info_hdr.print(std::cerr); // blather
#endif
    if (info_hdr.compression) {
      where << "cannot cope with compression at the moment\n";
      return false;
    }
  }
  else {
    // urgh!
    where << "dunno about header_size " << core_hdr.header_size << std::endl;
    return false;
  }

  // skip colormap info
  is_->seek(file_hdr.bitmap_offset); // === seek(is_->tell()+info_hdr.colormapsize);
#if 0
  // color map nonsense
  if (info_hdr.colormapsize ==0 && info_hdr.colorcount == 0) {
    // phew! no colour map.
  }
  else if (info_hdr.colormapsize == 256 && core_hdr.bitsperpixel == 8) {
    // In this case I know how to read the colormap because I have hexdumped an example.
    // But I ignore the color map in the get_section() routine because I don't care.
    // fsm
    typedef unsigned char uchar;
    freds_colormap = new uchar *[4];
    freds_colormap[0] = new uchar[256];
    freds_colormap[1] = new uchar[256];
    freds_colormap[2] = new uchar[256];
    freds_colormap[3] = new uchar[256];
    uchar bif[4];
    for (int i=0; i<256; ++i) {
      is_->read(bif, sizeof(bif));
      freds_colormap[0][i] = bif[0];
      freds_colormap[1][i] = bif[1];
      freds_colormap[2][i] = bif[2];
      freds_colormap[3][i] = bif[3];
    }
  }
  else {
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
  else {
  }

  if (ccount != 0) {
    unsigned cmap_size;
    if (header.biSize == sizeof(xBITMAPCOREHEADER))
      cmap_size = ccount*3;
    else
      cmap_size = ccount*4;

    std::vector<uchar> cmap(cmap_size, 0); // use vector<> to avoid coreleak
    if (is_->read(/* xxx */&cmap[0], 1024L) != 1024L) {
      std::cerr << "Error reading image palette\n";
      return false;
    }

    // SetColorNum(ccount);
    // int ncolors = get_color_num();
    int ncolors = ccount; // good guess
    if (ncolors != 0) {
      int **color_map = new int*[3];
      for (int i=0; i<3; ++i) {
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
  where << "bit_map_start = " << bit_map_start << std::endl; // blather
#endif
  assert(bit_map_start == (int)file_hdr.bitmap_offset); // I think they're supposed to be the same -- fsm.

  return true;
}

bool vil1_bmp_generic_image::write_header()
{
#ifdef DEBUG
  std::cerr << "Writing BMP header\n"
           << width() << 'x' << height() << '@'
           << components() << 'x' << bits_per_component() << '\n';
#endif

  int rowlen = width() * components() * bits_per_component() / 8;
  rowlen += (3-(rowlen-1)%4); // round up to multiple of 4
  int data_size = height() * rowlen;

  if (components() == 1)
    info_hdr.colorcount = info_hdr.colormapsize = 1<<bits_per_component();
  file_hdr.bitmap_offset = bit_map_start = 54L + 4 * info_hdr.colormapsize;
  file_hdr.file_size = bit_map_start+data_size;
  core_hdr.header_size = 40;
  core_hdr.width = width();
  core_hdr.height = height();
  core_hdr.bitsperpixel = components()*bits_per_component();
  info_hdr.bitmap_size = data_size;

  is_->seek(0L);
  file_hdr.write(is_);
  core_hdr.write(is_);
  info_hdr.write(is_);
  if (components() == 1) // Need to write a colourmap in this case
    for (int i=0; i<(1<<bits_per_component()); ++i)
      for (int j=0; j<4; ++j)
      {
        unsigned char c = i;
        is_->write(&c,1L);
      }

  return true;
}

//------------------------------------------------------------

bool vil1_bmp_generic_image::get_section(void* ib, int x0, int y0, int w, int h) const
{
  assert(ib != nullptr);
  char *bp = static_cast<char*>(ib);

  //
  unsigned bytes_per_pixel = core_hdr.bitsperpixel / 8;
  assert(core_hdr.bitsperpixel == 8 || core_hdr.bitsperpixel == 24);
  // FIXME - add support for 1, 4, 16 and 32 bpp

  // actual number of bytes per raster in file.
  unsigned have_bytes_per_raster = ((bytes_per_pixel * core_hdr.width + 3)>>2)<<2;

  // number of bytes we want per raster.
  unsigned long want_bytes_per_raster = w*bytes_per_pixel;

  // read each raster in turn. if the client wants the whole image, it may
  // be faster to read() it all in one chunk, so long as the number of bytes
  // per image raster is divisible by four (because the file rasters are
  // padded at the ends).
  for (int i=0; i<h; ++i) {
    is_->seek(bit_map_start + have_bytes_per_raster*(i+y0) + x0*bytes_per_pixel);
    is_->read(bp + want_bytes_per_raster*i, want_bytes_per_raster);
  }

  return true;
}


bool vil1_bmp_generic_image::put_section(void const *ib, int x0, int y0, int xs, int ys)
{
  assert(ib != nullptr);
  int bypp = (components() * bits_per_component() +7) / 8;
  int rowlen = width() * bypp;
  rowlen += (3-(rowlen-1)%4); // round up to a multiple of 4

  int skip_rows = height()-y0-ys;

  for (int y=0; y<ys; ++y,++skip_rows)
  {
    is_->seek(bit_map_start+skip_rows*rowlen+x0*bypp);
    is_->write(((char const*)ib)+y*xs*bypp, xs*bypp);
  }

  return true;
}
