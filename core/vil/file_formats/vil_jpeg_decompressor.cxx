// This is core/vil/file_formats/vil_jpeg_decompressor.cxx
//:
// \file
// \author fsm
// \verbatim
//  Modifications
//     11 Oct 2002 Ian Scott - converted to vil
//\endverbatim

#include <iostream>
#include "vil_jpeg_decompressor.h"
#include "vil_jpeg_source_mgr.h"
#include <vil/vil_stream.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h>

#define trace if (true) { } else std::cerr

//: using jpeg decompressor objects :
// -# supply an error manager, e.g. with jpeg_std_err().
//    this \e must be done before initializing the object.
// -# initialize with jpeg_create_decompress().
// -# supply a data stream, e.g. with jpeg_std_source().
// -# call jpeg_read_header() to start reading the data stream. this will read
//    to the start of the compressed data and store various tables and parameters.
//    if you just want the image parameters and not the data, it's ok to stop
//    now, so long as you call jpeg_abort_decompress() or jpeg_destroy_decompress()
//    to release resources.
// -# call jpeg_finish_decompress() if you read all the data. if you only read
//    some of the data, call jpeg_abort_decompress().
// -# destruct the object with jpeg_destroy_decompress().

vil_jpeg_decompressor::vil_jpeg_decompressor(vil_stream *s)
  : stream(s)
  , ready(false)
  , valid(false)
  , biffer(nullptr)
{
  stream->ref();

  // setup the standard error handler in the jpeg library
  jobj.err = jpeg_std_error(&jerr);

  // construct the decompression object :
  jpeg_create_decompress(&jobj);

  // Increase the amount of memory that can be used.
  // Default (1Mb) was too small.
#if defined(VXL_ADDRESS_BITS) && VXL_ADDRESS_BITS == 32
  jobj.mem->max_memory_to_use = 300 * 1024 * 1024;
#elif defined(VXL_ADDRESS_BITS) && VXL_ADDRESS_BITS == 64
  jobj.mem->max_memory_to_use = 1024 * 1024 * 1024;
#else
  /* use the default memory settings */
#endif

  // we need to read the header here, in order to get parameters such as size.
  //
  // set the data source
  vil_jpeg_stream_src_set(&jobj, stream);

  // rewind the stream
  vil_jpeg_stream_src_rewind(&jobj, stream);

  // now we may read the header.
  jpeg_read_header(&jobj, TRUE);

  // This seems to be necessary. jpeglib.h claims that one can use
  // jpeg_calc_output_dimensions() instead, but I never bothered to try.
#if 1
  // bogus decompression to get image parameters.
  jpeg_start_decompress(&jobj);

  // this aborts the decompression, but doesn't destroy the object.
  jpeg_abort_decompress(&jobj);
#endif
}

// read the given scanline, skipping/rewinding as required.
JSAMPLE const *vil_jpeg_decompressor::read_scanline(unsigned line)
{
  // if the client tries to read the same scanline again, it should be free.
  if (valid && line == jobj.output_scanline-1)
    return biffer;

  if (ready && line<jobj.output_scanline) {
    trace << "...aborting\n";
    // bah! have to restart
    jpeg_abort_decompress(&jobj);

    //
    ready = false;
    valid = false;
  }

  if (!ready) {
    trace << "...restarting\n";

    // rewind stream
    vil_jpeg_stream_src_rewind(&jobj, stream);

    // read header
    jpeg_read_header(&jobj, TRUE);

    // start decompression
    jpeg_start_decompress(&jobj);

    //
    ready = true;
    valid = false;
  }

  // allocate scanline buffer, if necessary.
  if (!biffer) {
    trace << "...allocate buffer\n";
    unsigned row_size = jobj.output_width * jobj.output_components;
    biffer = new JSAMPLE[row_size];
  }

  // 'buffer' is a pointer to a 1-element array whose 0th element is biffer.
#if 0
  JSAMPLE *buffer[1] = { biffer };
#else
  JSAMPARRAY buffer = &biffer;
#endif

  // read till we've read the line we want :
  while (jobj.output_scanline <= line) {
    if (jpeg_read_scanlines(&jobj, buffer, 1) != 1) {
      jpeg_abort_decompress(&jobj);
      ready = false;
      valid = false;
      return nullptr;
    }
  }

  // end reached ?
  if (jobj.output_scanline >= jobj.image_height) {
    trace << "...reached end\n";
    jpeg_finish_decompress(&jobj); // this will call vil_jpeg_term_source()
    ready = false;
  }

  // success.
  valid = true; // even if we have reached the end.
  return biffer;
}


vil_jpeg_decompressor::~vil_jpeg_decompressor()
{
  // destroy the pool associated with jobj
  (*jobj.mem->free_pool) ((j_common_ptr) &jobj, JPOOL_IMAGE);

  // destroy the decompression object
  jpeg_destroy_decompress(&jobj);

  //
  stream->unref();
  stream = nullptr;

  //
  if (biffer)
    delete [] biffer;
  biffer = nullptr;
}
