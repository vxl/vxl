// This is oxl/oxp/JPEG_Decompressor.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "JPEG_Decompressor.h"

#include <vcl_fstream.h>
#include <vil/vil_jpeglib.h>

struct JPEG_DecompressorPrivates {
  jpeg_decompress_struct jinfo;
  jpeg_error_mgr jerr;
  JSAMPARRAY buffer;
  FILE* fileid;
};

JPEG_Decompressor::JPEG_Decompressor(int fd)
{
  init(fd);
}

// cannot be sure vcl_ifstream has an fd() method.
#if defined(VCL_SGI_CC) || defined(VCL_SUNPRO_CC) || (defined(VCL_GCC) && !defined(GNU_LIBSTDCXX_V3))
JPEG_Decompressor::JPEG_Decompressor(vcl_ifstream& f)
{
  init(f.rdbuf()->fd());
}
#endif

void JPEG_Decompressor::init(int fd)
{
  pd = new JPEG_DecompressorPrivates;

  // setup the standard error handler in the jpeg library
  pd->jinfo.err = jpeg_std_error( &pd->jerr);

  // create the decompression object
  jpeg_create_decompress( &pd->jinfo);

  // read the header
  pd->fileid = fdopen(fd, "r");
  jpeg_stdio_src(&pd->jinfo, pd->fileid);
  jpeg_read_header( &pd->jinfo, TRUE);
  jpeg_start_decompress( &pd->jinfo);

  // JSAMPLEs per row in output buffer
  unsigned long row_stride = pd->jinfo.output_width * pd->jinfo.output_components;

  // Make a one-row-high sample array that will go away when done with image
  pd->buffer = (*pd->jinfo.mem->alloc_sarray) ((j_common_ptr) &pd->jinfo, JPOOL_IMAGE, row_stride, 1);
}

void JPEG_Decompressor::StartNextJPEG()
{
  // shut down the decompression object
  jpeg_finish_decompress( &pd->jinfo);

  // free the scanline that we created
  (*pd->jinfo.mem->free_pool) ((j_common_ptr) &pd->jinfo, JPOOL_IMAGE);

  jpeg_read_header( &pd->jinfo, TRUE);
  jpeg_start_decompress( &pd->jinfo);

  // JSAMPLEs per row in output buffer
  unsigned long row_stride = pd->jinfo.output_width * pd->jinfo.output_components;

  // Make a one-row-high sample array that will go away when done with image
  pd->buffer = (*pd->jinfo.mem->alloc_sarray) ((j_common_ptr) &pd->jinfo, JPOOL_IMAGE, row_stride, 1);
}

void* JPEG_Decompressor::GetNextScanLine()
{
  if (jpeg_read_scanlines(&pd->jinfo, pd->buffer, 1) != 1)
    return 0;
  return pd->buffer[0];
}

JPEG_Decompressor::~JPEG_Decompressor()
{
  // shut down the decompression object
  jpeg_abort_decompress( &pd->jinfo);

  // free the scanline that we created
  (*pd->jinfo.mem->free_pool) ((j_common_ptr) &pd->jinfo, JPOOL_IMAGE);

  delete pd;
}


int JPEG_Decompressor::height()
{
  return pd->jinfo.output_height;
}

int JPEG_Decompressor::width()
{
  return pd->jinfo.output_width;
}

int JPEG_Decompressor::GetBitsPixel()
{
  return pd->jinfo.output_components;
}

// extern "C" unsigned long jpeg_stdio_ftell(j_decompress_ptr cinfo);

// capes - implement this method here, rather than messing with the
// libjpeg sources.
unsigned long jpeg_stdio_ftell(j_decompress_ptr cinfo)
{
  typedef struct {
    struct jpeg_source_mgr pub;
    FILE * infile;
    JOCTET * buffer;
    bool start_of_file;
  } my_source_mgr;

  my_source_mgr* src = (my_source_mgr *) (cinfo->src);
  if (src->pub.next_input_byte)
    return ftell(src->infile) - src->pub.bytes_in_buffer;
  else
    return 0;
}

unsigned long JPEG_Decompressor::GetFilePosition()
{
  return jpeg_stdio_ftell(&pd->jinfo);
}
