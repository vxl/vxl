/*
  fsm@robots.ox.ac.uk
*/
// Created: 17 Feb 2000
#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_jpeg.h"
#include <assert.h>
#include <vcl/vcl_iostream.h>
#include <vil/vil_stream.h>
#include <vil/vil_stream_FILE_ptr.h>

// ?
extern "C" {
#undef boolean
#define boolean jpeg_boolean
#include <jpeglib.h>
}

// static data
static char const jpeg_string[] = "jpeg";

//--------------------------------------------------------------------------------
// class vil_jpeg_file_format

char const* vil_jpeg_file_format::tag() const {
  return jpeg_string;
}

//:
vil_generic_image *vil_jpeg_file_format::make_input_image(vil_stream *vs) {
  // read magic bytes.
  char magic[2];
  vs->seek(0);
  int n = vs->read(magic, sizeof(magic));
  if (n != sizeof(magic)) {
    cerr << __FILE__ << " : vil_stream::read() failed" << endl;
    return 0;
  }

  // compare.
  bool is_jpeg = ( (magic[0] == char(0xFF)) && (magic[1] == char(0xD8)) );
  if (!is_jpeg)
    return 0; // forget it.

  // new up a vil_jpeg_generic_image.
  vil_jpeg_generic_image *image = new vil_jpeg_generic_image(vs);
  if (!image) {
    cerr << __FILE__ << " : file has jpeg magic number, but cannot read" << endl;
    return 0;
  }

  // done.
  return image;
}

vil_generic_image *vil_jpeg_file_format::make_output_image(vil_stream */*vs*/, 
							   vil_generic_image const */*prototype*/)
{
  assert(false);
  return 0;
}

//--------------------------------------------------------------------------------
// class vil_jpeg_generic_image

// implementation class
struct vil_jpeg_generic_image::impl {
  impl(vil_stream *vs) : file_ptr(vs) { }
  
  vil_stream_FILE_ptr file_ptr;
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr         jerr;
};

vil_jpeg_generic_image::vil_jpeg_generic_image(vil_stream *is) 
  : pimpl(new impl(is))
{
  // setup the standard error handler in the jpeg library
  pimpl->cinfo.err = jpeg_std_error(&pimpl->jerr);
  
  // create the decompression object
  jpeg_create_decompress(&pimpl->cinfo);
  
  // read the header
  jpeg_stdio_src(&pimpl->cinfo, pimpl->file_ptr);
  jpeg_read_header(&pimpl->cinfo, TRUE);
  jpeg_start_decompress(&pimpl->cinfo);
  
  // Have to reset the decompression object
  jpeg_abort_decompress(&pimpl->cinfo);
}

// vil_jpeg_generic_image::vil_jpeg_generic_image(vil_stream* is, vil_generic_image const* prototype) 
//   : stream_ptr(is)
// { 
// }

vil_jpeg_generic_image::~vil_jpeg_generic_image() {
  delete pimpl;
  pimpl = 0;
}

//--------------------------------------------------------------------------------

//:
int vil_jpeg_generic_image::planes() const {
  return 1; 
}

//:
int vil_jpeg_generic_image::width() const { 
  return pimpl->cinfo.output_width;
}

//:
int vil_jpeg_generic_image::height() const {
  return pimpl->cinfo.output_height;
}

//:
int vil_jpeg_generic_image::components() const {
  return pimpl->cinfo.output_components;
}

//#include <limits.h> // for CHAR_BIT

//:  
int vil_jpeg_generic_image::bits_per_component() const {
  
  return 8; // CHAR_BIT
}

//:
vil_component_format vil_jpeg_generic_image::component_format() const {
  return VIL_COMPONENT_FORMAT_UNSIGNED_INT; 
}

//:  
const char *vil_jpeg_generic_image::file_format() const {
  return jpeg_string;
}

vil_generic_image *vil_jpeg_generic_image::get_plane(int i) const {
  assert(false);
  return 0;
}

//--------------------------------------------------------------------------------

//: gets section, by calling decompress_section.
// If you want a jpeg image which caches blocks of the image, put a non-caching
// image into a block_cacher.
bool vil_jpeg_generic_image::do_get_section(void *buf, int x0, int y0, int w, int h) const {
  return const_cast<vil_jpeg_generic_image*>(this)->decompress_section(buf, x0, y0, w, h);
}

//: not implemented.
bool vil_jpeg_generic_image::do_put_section(void const */*buf*/,
					    int /*x*/, int /*y*/, 
					    int /*w*/, int /*h*/) 
{
  cerr << __FILE__ ":" << __LINE__ << " : cannot put section to JPEG image" << endl;
  return false;
}

//--------------------------------------------------------------------------------

//: 
bool vil_jpeg_generic_image::decompress_section(void *buf, int x0, int y0, int w, int h) {
  // Reset file and reset the decompression 
  rewind(pimpl->file_ptr);
  jpeg_stdio_src(&pimpl->cinfo, pimpl->file_ptr);
  jpeg_read_header(&pimpl->cinfo, TRUE);
  jpeg_start_decompress(&pimpl->cinfo);

  // JSAMPLEs per row in output buffer
  unsigned row_stride = pimpl->cinfo.output_width * pimpl->cinfo.output_components;
  
  // make a one-row-high sample array. it will go away when done with image.
  JSAMPARRAY buffer = (*pimpl->cinfo.mem->alloc_sarray) ((j_common_ptr) &pimpl->cinfo, JPOOL_IMAGE, row_stride, 1);

  // skip the first y0 rows of the image :
  for (int i=0; i<y0; ++i) {
    if (jpeg_read_scanlines(&pimpl->cinfo, buffer, 1) != 1) {
      jpeg_abort_decompress( &pimpl->cinfo);
      return false;
    }
  }

  // bytes per pixel
  unsigned bpp = (planes()*components()*bits_per_component());
  assert(bpp == ((bpp/8)*8));
  bpp /= 8;
  
  // process the next h rows :
  for (int i=0; i<h; ++i) {
    // read scanline :
    if (jpeg_read_scanlines(&pimpl->cinfo, buffer, 1)!= 1) {
      jpeg_abort_decompress(&pimpl->cinfo);
      return false;
    }
    
    // copy the relevant w columns out :
    memcpy(static_cast<char*>(buf) + i*w*bpp, &buffer[0][x0*bpp], w*bpp);
  }
  
  // shut down the decompression object
  jpeg_abort_decompress(&pimpl->cinfo);

  // free the scanline that we created
  (*pimpl->cinfo.mem->free_pool) ((j_common_ptr) &pimpl->cinfo, JPOOL_IMAGE);

  return true;
}

//--------------------------------------------------------------------------------

#if 0
bool save_jpeg(ImageBuffer<RGBcell> & image, FILE* outfile)
{
  int sizex = image.GetSizeX();
  int sizey = image.GetSizeY();  
  
  struct jpeg_compress_struct pimpl->cinfo;
  struct jpeg_error_mgr pimpl->jerr;
  pimpl->cinfo.err = jpeg_std_error(&pimpl->jerr);
      
  jpeg_create_compress(&pimpl->cinfo);

  jpeg_stdio_dest(&pimpl->cinfo, outfile);
 
  pimpl->cinfo.image_width = sizex;      /* image width and height, in pixels */
  pimpl->cinfo.image_height = sizey;
  pimpl->cinfo.input_components = 3;     /* # of color components per pixel */
  pimpl->cinfo.in_color_space = JCS_RGB; /* colorspace of input image */

  jpeg_set_defaults(&pimpl->cinfo);
  /* Make optional parameter settings here */


  JSAMPLE** rows = new JSAMPLE*[sizey];
  bool flipy = true;

  if (!flipy)
    for(int y = 0; y < sizey; ++y)
      rows[y] = (JSAMPLE*)&image(0,y);
  else
    for(int y = 0; y < sizey; ++y)
      rows[sizey - y - 1] = (JSAMPLE*)&image(0,y);


  bool write_all_tables = true;
  jpeg_start_compress (&pimpl->cinfo, write_all_tables);
  jpeg_write_scanlines(&pimpl->cinfo, rows, sizey);
  jpeg_finish_compress(&pimpl->cinfo);
  jpeg_destroy_compress(&pimpl->cinfo);

  return true;
}


void save_jpeg(ImageBuffer<byte> & image, char const* filename)
{
  FILE* outfile = fopen(filename, "w");
  if (!outfile) {
    cerr << "save_jpeg: Can't write to \"" << filename << "\"\n";
    return;
  }
 
  int sizex = image.GetSizeX();
  int sizey = image.GetSizeY();
 
  struct jpeg_compress_struct pimpl->cinfo;
  struct jpeg_error_mgr pimpl->jerr;
  pimpl->cinfo.err = jpeg_std_error(&pimpl->jerr);
 
  jpeg_create_compress(&pimpl->cinfo);
 
  jpeg_stdio_dest(&pimpl->cinfo, outfile);
 
  pimpl->cinfo.image_width = sizex;      /* image width and height, in pixels */
  pimpl->cinfo.image_height = sizey;
  pimpl->cinfo.input_components = 3;     /* # of color components per pixel */
  pimpl->cinfo.in_color_space = JCS_RGB; /* colorspace of input image */
 
  jpeg_set_defaults(&pimpl->cinfo);
  /* Make optional parameter settings here */
 
  bool write_all_tables = true;
  jpeg_start_compress (&pimpl->cinfo, write_all_tables);
 
  JSAMPLE* row = new JSAMPLE[sizex * 3];
  JSAMPLE* rowptr[1] = { row };
  for(int y = 0; y < sizey; ++y) {
    for(int x = 0; x < sizex; ++x)
      row[x*3] = row[x*3+1] = row[x*3+2] = image(x,y);
    jpeg_write_scanlines(&pimpl->cinfo, rowptr, 1);
  }
  delete[] row;
 
  jpeg_finish_compress(&pimpl->cinfo);
  jpeg_destroy_compress(&pimpl->cinfo);
 
  fclose(outfile);
}
#endif
