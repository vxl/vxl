// This is core/vil1/file_formats/vil1_jpeg_compressor.cxx
//:
// \file
// \author fsm

#include <iostream>
#include "vil1_jpeg_compressor.h"
#include "vil1_jpeg_destination_mgr.h"
#include <vil1/vil1_stream.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

vil1_jpeg_compressor::vil1_jpeg_compressor(vil1_stream *s)
  : stream(s)
  , ready(false)
{
  stream->ref();

  // setup the standard error handler in the jpeg library
  jobj.err = jpeg_std_error(&jerr);

  // Zero just in case..
  jobj.next_scanline = 0;

  // construct the compression object :
  jpeg_create_compress(&jobj);

  // set the data destination
  vil1_jpeg_stream_dst_set(&jobj, stream);
}

bool vil1_jpeg_compressor::write_scanline(unsigned line, JSAMPLE const *scanline) {
  if (!ready) {
    // rewind the stream
    vil1_jpeg_stream_dst_rewind(&jobj, stream);

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

vil1_jpeg_compressor::~vil1_jpeg_compressor() {
  // finish compression if necessary
  if (ready)
    jpeg_finish_compress(&jobj);

  // destroy the compression object
  jpeg_destroy_compress(&jobj);

  //
  stream->unref();
  stream = nullptr;
}
