#ifdef __GNUC__
#pragma implementation "vil_bmp.h"
#endif

#include "vil_bmp.h"

#include <vcl/vcl_cassert.h>
#include <vcl/vcl_iostream.h>

#include <vil/vil_buffer.h>
#include <vil/vil_stream.h>
#include <vil/vil_image.h>

#define where (cerr << __FILE__ ":" << __LINE__ << " : ")

//--------------------------------------------------------------------------------

char const* vil_bmp_format_tag = "bmp";

vil_image_impl* vil_bmp_file_format::make_input_image(vil_stream* is)
{
  // Attempt to read header
  vil_bmp_file_header hdr;
  is->seek(0);
  hdr.read(is);
  
  if ( hdr.signature_valid() )
    return new vil_bmp_generic_image(is);
  
  //cerr << "not a .bmp file" << endl;
  return 0;
}

vil_image_impl* vil_bmp_file_format::make_output_image(vil_stream* is, int planes,
						       int width,
						       int height,
						       int components,
						       int bits_per_component,
						       vil_component_format format)
{
  return new vil_bmp_generic_image(is, planes, width, height, components, bits_per_component, format);
}

char const* vil_bmp_file_format::tag() const
{
  return vil_bmp_format_tag;
}

/////////////////////////////////////////////////////////////////////////////
  
char const* vil_bmp_generic_image::file_format() const
{
  return vil_bmp_format_tag;
}

vil_bmp_generic_image::vil_bmp_generic_image(vil_stream* is)
  : is_(is)
  , bit_map_start(-1L)
  , freds_colormap(0)
  //, local_color_map_(0)
{
  read_header();
}

vil_bmp_generic_image::vil_bmp_generic_image(vil_stream* is,
					     int planes,
					     int width,
					     int height,
					     int components,
					     int bits_per_component,
					     vil_component_format format) 
  : is_(is)
  , bit_map_start(-1L)
  , freds_colormap(0)
  //, local_color_map_(0)
{
  is_->ref();
  assert(planes == 1); // FIXIT

  //file_hdr.magic;
  //file_hdr.file_size;
  //file_size.reserved1;
  //file_size.reserved2;
  //file_size.bitmap_offset;
  

  // core_hdr.header_size is set up for us.
  core_hdr.width = width;
  core_hdr.height = height;
  core_hdr.planes = 1;
  if (bits_per_component == 8 && components == 1) {
    core_hdr.bitsperpixel = 8;
  }
  else if (bits_per_component == 8 && components == 3) {
    core_hdr.bitsperpixel = 24;
  }
  else {
    // can't handle that.
    assert(false);
  }

  // don't worry about the info header for now.
}

vil_bmp_generic_image::~vil_bmp_generic_image()
{
#if 0
  // we must get rid of the local_color_map_;
  if(local_color_map_){
    delete [] local_color_map_[0];
    delete [] local_color_map_[1];
    delete [] local_color_map_[2];
    delete local_color_map_;
  }
#endif

  if (freds_colormap) {
    delete [] freds_colormap[0];
    delete [] freds_colormap[1];
    delete [] freds_colormap[2];
    delete [] freds_colormap[3];
    delete [] freds_colormap;
    freds_colormap = 0;
  }
  
  is_->unref();
}

bool vil_bmp_generic_image::read_header()
{
  // seek to beginning and read file header.
  is_->seek(0);
  file_hdr.read(is_);
  if( ! file_hdr.signature_valid() ) {
    where <<  "File is not a valid BMP file" << endl;
    return false;
  }
  file_hdr.print(cerr); // blather
  

  // read core header
  core_hdr.read(is_);
  core_hdr.print(cerr); // blather
  
  // determine whether or not there is an info header from
  // the size field.
  if (core_hdr.header_size == vil_bmp_core_header::disk_size) {
    // no info header.
  }
  else if (core_hdr.header_size == vil_bmp_core_header::disk_size + vil_bmp_info_header::disk_size) {
    // probably an info header. read it now.
    info_hdr.read(is_);
    info_hdr.print(cerr); // blather
    if (info_hdr.compression) {
      where << "cannot cope with compression at the moment" << endl;
      assert(false);
    }
  }
  else {
    // urgh!
    where << "dunno about header_size " << core_hdr.header_size << endl;
    return false;
  }

  // color map nonsense
  if (info_hdr.colormapsize ==0 && info_hdr.colorcount == 0) {
    // phew! no colour map.
  }
  else if (info_hdr.colormapsize == 256 && info_hdr.colorcount == 256 && core_hdr.bitsperpixel == 8) {
    // In this case I know how to read the colormap because I have hexdumped an example.
    // But I ignore the color map in the get_section() routine because I don't care.
    // -- fsm
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
    assert(false); // FIXIT
  }
  

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
    
    vil_buffer<uchar> cmap(cmap_size, 0); // use vil_buffer<> to avoid coreleak
    if (is_->read(cmap.data(),1024) != 1024) {
      cerr << "Error reading image palette" << endl;
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
  
  // SetOffset(file->Tell());
  start_of_data_ = is_->tell();
  
  // TODO not obvious where the magic number is read 
#endif


  // remember the position of the start of the bitmap data
  bit_map_start = is_->tell();
  where << "bit_map_start = " << bit_map_start << endl; // blather
  assert(bit_map_start == file_hdr.bitmap_offset); // I think they're supposed to be the same -- fsm.

  return true;
}

bool vil_bmp_generic_image::write_header()
{
  assert(false); // FIXIT
#if 0
  int rgb_length=0;

  int used_color=0;

  if (header.biClrUsed != 0)
      used_color = header.biClrUsed;
  else
      if (header.biBitCount != 24)
          used_color = 1 << header.biBitCount;

  if (false) {
    cerr << "Writing BMP header" << endl;
    cerr << header.biWidth << ' ' << header.biHeight << ' '
	 << header.biBitCount << endl;
  }

  if (header.biSize == sizeof(xBITMAPINFOHEADER))
      rgb_length = 4;
   else if (header.biSize == sizeof(xBITMAPCOREHEADER))
            rgb_length = 3;
        else
           {
             cerr << "Invalid header size"<<endl;
             return false;
           }

  fbmp.bfOffBits = 14+header.biSize+used_color*rgb_length;
  fbmp.bfSize    = fbmp.bfOffBits + get_area()*pixsize;
  

  // SetSizeX(header.biWidth);
  // SetSizeY(header.biHeight);
  // SetBitsPixel(header.biBitCount);

  if (is_->write(&fbmp.bfType, sizeof(fbmp.bfType)) != sizeof(fbmp.bfType))
  {
    cerr << "Error while writing BMP file header"<< endl;
    return false;
  }

  //if (is_->write(&fbmp.bfSize, sizeof(fbmp.bfSize)) != sizeof(fbmp.bfSize))
  //{
  //    cerr << "Error while writing BMP file header"<< endl;
  //    return false;
  //}
  write_int_macro(is_, fbmp.bfSize);

  //if (is_->write(&fbmp.bfReserved1, sizeof(fbmp.bfReserved1)) != sizeof(fbmp.bfReserved1))
  //{
  //    cerr << "Error while writing BMP file header"<< endl;
  //    return false;
  //}
  write_int_macro(is_, fbmp.bfReserved1);

  //if (is_->write(&fbmp.bfReserved2, sizeof(fbmp.bfReserved2)) != sizeof(fbmp.bfReserved2))
  //{
  //    cerr << "Error while writing BMP file header"<< endl;
  //    return false;
  //}
  write_int_macro(is_, fbmp.bfReserved2);

  //if (is_->write(&fbmp.bfOffBits, sizeof(fbmp.bfOffBits)) != sizeof(fbmp.bfOffBits))
  //{
  //    cerr << "Error while writing BMP file header"<< endl;
  //    return false;
  //}
  write_int_macro(is_, fbmp.bfOffBits);
  
  if (header.biSize == sizeof(xBITMAPINFOHEADER))
  {
     rgb_length = 4;
     if (is_->write(&header, sizeof(xBITMAPINFOHEADER)) != sizeof(xBITMAPINFOHEADER))
     {
         cerr << "Error while writing header"<< endl;
         return false;
     }

  }
  else if (header.biSize == sizeof(xBITMAPCOREHEADER))
       {
          rgb_length = 3;
          if (is_->write(&header, sizeof(xBITMAPINFOHEADER)) != sizeof(xBITMAPINFOHEADER))
          {
             cerr << "Error while writing header"<< endl;
             return false;
          }
       }

  if (used_color != 0 && local_color_map_)
  {
    // int** cm = get_color_map();
    int** cm = local_color_map_;
    
    for(int j=0; j<used_color; j++)
    {
      for(int i=0; i<3; i++)
        is_->write(&cm[2-i][j], 1);
      is_->write(0, 1);
    }
  }

  //  SetOffset(file->Tell());
#endif

  return true;
}

//------------------------------------------------------------

bool vil_bmp_generic_image::get_section(void* ib, int x0, int y0, int w, int h) const
{
  assert(ib);
  char *bp = static_cast<char*>(ib);

  //
  unsigned bytes_per_pixel;
  if (core_hdr.bitsperpixel == 8)
    bytes_per_pixel = 1;
  else if (core_hdr.bitsperpixel == 24)
    bytes_per_pixel = 3;
  else
    assert(false); // FIXIT
  
  // actual number of bytes per raster in file.
  unsigned have_bytes_per_raster = ((bytes_per_pixel * core_hdr.width + 3)>>2)<<2;

  // number of bytes we want per raster.
  unsigned want_bytes_per_raster = w*bytes_per_pixel;
 
  // read each raster in turn. if the client wants the whole image, it may
  // be faster to read() it all in one chunk, so long as the number of bytes
  // per image raster is divisible by four (because the file rasters are
  // padded at the ends).
  for (int i=0; i<h; ++i) {
    is_->seek(bit_map_start + have_bytes_per_raster*i + x0*bytes_per_pixel);
    is_->read(bp + want_bytes_per_raster*i, want_bytes_per_raster);
  }
  
  return true;
}


bool vil_bmp_generic_image::put_section(void const *ib, int x0, int y0, int xs, int ys)
{
  assert(ib);
  
  where << "put_section() does not work yet/any more" << endl;
  return false;
}
