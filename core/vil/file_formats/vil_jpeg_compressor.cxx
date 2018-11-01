// This is core/vil/file_formats/vil_jpeg_compressor.cxx
//:
// \file
// \author fsm
// \verbatim
//  Modifications
//     11 Oct 2002 Ian Scott - converted to vil
//\endverbatim

#include <iostream>
#include "vil_jpeg_compressor.h"
#include "vil_jpeg_destination_mgr.h"
#include <vil/vil_stream.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h>

vil_jpeg_compressor::vil_jpeg_compressor(vil_stream *s)
  : stream(s)
  , ready(false), quality(75)
{
  stream->ref();

  // setup the standard error handler in the jpeg library
  jobj.err = jpeg_std_error(&jerr);

  // Zero just in case..
  jobj.next_scanline = 0;

  // construct the compression object :
  jpeg_create_compress(&jobj);

  // Increase the amount of memory that can be used.
  // Default (1Mb) was too small.
#if defined(VXL_ADDRESS_BITS) && VXL_ADDRESS_BITS == 32
  jobj.mem->max_memory_to_use = 300 * 1024 * 1024;
#elif defined(VXL_ADDRESS_BITS) && VXL_ADDRESS_BITS == 64
  jobj.mem->max_memory_to_use = 1024 * 1024 * 1024;
#else
  /* use the default memory settings */
#endif

  // set the data destination
  vil_jpeg_stream_dst_set(&jobj, stream);
}

bool vil_jpeg_compressor::write_scanline(unsigned line, JSAMPLE const *scanline)
{
  if (!ready) {
    // rewind the stream
    vil_jpeg_stream_dst_rewind(&jobj, stream);

    //
    jobj.next_scanline = 0;

    // set colorspace of input image. FIXME.
    switch (jobj.input_components) {
     case 1:
      jobj.in_color_space = JCS_GRAYSCALE;
      break;
     case 3:
      jobj.in_color_space = JCS_RGB;
      break;
     default:
      std::cerr << __FILE__ " : urgh!\n";
      return false;
    }

    jpeg_set_defaults(&jobj);
    jpeg_set_quality(&jobj, quality, TRUE);

    // start compression
    jpeg_boolean write_all_tables = TRUE;
    jpeg_start_compress (&jobj, write_all_tables);

    //
    ready = true;
  }

  //
  if (line != jobj.next_scanline) {
    std::cerr << "scanlines must be written in order\n";
    return false;
  }

  // write the scanline
  { auto *tmp = const_cast<JSAMPLE*>(scanline);
  jpeg_write_scanlines(&jobj, &tmp, 1); }

  // finish if the last scanline is written
  if (line == jobj.image_height - 1) {
    jpeg_finish_compress(&jobj);
    ready = false;
  }

  return true;
}

vil_jpeg_compressor::~vil_jpeg_compressor()
{
  // finish compression if necessary
  if (ready)
    jpeg_finish_compress(&jobj);

  // destroy the compression object
  jpeg_destroy_compress(&jobj);

  //
  stream->unref();
  stream = nullptr;
}

void vil_jpeg_compressor::set_quality(int q)
{
  quality = q;
}

int vil_jpeg_compressor::get_quality()
{
  return quality;
}
