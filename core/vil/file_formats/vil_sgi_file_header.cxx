// This is core/vil/file_formats/vil_sgi_file_header.cxx
//:
// \file
// \author David Hughes

#include <iomanip>
#include <iostream>

#include "vil_sgi_file_header.h"
#include <vil/vil_stream.h>
#include <vil/vil_stream_write.h>
#include <vil/vil_stream_read.h>

// The magic number for SGI images
#define SGI_SIGNATURE_BYTE_0 0x01
#define SGI_SIGNATURE_BYTE_1 0xDA

vil_sgi_file_header::vil_sgi_file_header()
{

  // Fill in all the numberical fields
  // Don't worry about the dummy values or image name, which are ignored
  magic[0] = SGI_SIGNATURE_BYTE_0;
  magic[1] = SGI_SIGNATURE_BYTE_1;
  storage = 0;
  bpc = 1;
  dimension = 0;
  xsize = 0;
  ysize = 0;
  zsize = 0;
  pixmin = 0;
  pixmax = 255;
  colormap = 0;
}

void vil_sgi_file_header::print(std::ostream &s) const
{
  s << "vil_sgi_file_header:\n"
    << "  magic    : " << std::hex
    << "0x" << unsigned(magic[0]) << ' '
    << "0x" << unsigned(magic[1]) << std::dec << std::endl
    << "  storage  : " << storage << std::endl
    << "  bpc      : " << bpc << std::endl
    << "  dimension: " << dimension << std::endl
    << "  xsize    : " << xsize << std::endl
    << "  ysize    : " << ysize << std::endl
    << "  zsize    : " << zsize << std::endl
    << "  pixmin   : " << pixmin << std::endl
    << "  pixmax   : " << pixmax << std::endl
    << "  colormap : " << colormap << std::endl << std::endl;
}

void vil_sgi_file_header::read(vil_stream *s)
{
  if (s->read(&magic, sizeof(magic)) == 0) {magic[0] = magic[1] = 0;}
  s->read(&storage, sizeof(storage));
  s->read(&bpc, sizeof(bpc));
  dimension = vil_stream_read_big_endian_uint_16(s);
  xsize = vil_stream_read_big_endian_uint_16(s);
  ysize = vil_stream_read_big_endian_uint_16(s);
  zsize = vil_stream_read_big_endian_uint_16(s);
  pixmin = vil_stream_read_big_endian_uint_32(s);
  pixmax = vil_stream_read_big_endian_uint_32(s);
  s->read(&dummy1, sizeof(dummy1));
  s->read(&image_name, sizeof(image_name));
  colormap = vil_stream_read_big_endian_uint_32(s);
  s->read(&dummy2, sizeof(dummy2));
}

void vil_sgi_file_header::write(vil_stream *s) const
{
  s->write(&magic, sizeof(magic));
  s->write(&storage, sizeof(storage));
  s->write(&bpc, sizeof(bpc));
  vil_stream_write_big_endian_uint_16(s, dimension);
  vil_stream_write_big_endian_uint_16(s, xsize);
  vil_stream_write_big_endian_uint_16(s, ysize);
  vil_stream_write_big_endian_uint_16(s, zsize);
  vil_stream_write_big_endian_uint_32(s, pixmin);
  vil_stream_write_big_endian_uint_32(s, pixmax);
  s->write(&dummy1, sizeof(dummy1));
  s->write(&image_name, sizeof(image_name));
  vil_stream_write_big_endian_uint_32(s, colormap);
  s->write(&dummy2, sizeof(dummy2));
}

bool vil_sgi_file_header::signature_valid() const
{
  return
    magic[0] == SGI_SIGNATURE_BYTE_0 &&
    magic[1] == SGI_SIGNATURE_BYTE_1;
}
