//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "vil_copy.h"
#endif
//
// Class: vil_copy
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 16 Feb 00
// Modifications:
//   000216 AWF Initial version.
//   000217 JS  planes*components because get_section returns RGBRGB
//
//-----------------------------------------------------------------------------

#include "vil_copy.h"

#include <assert.h>
#include <vil/vil_generic_image.h>

void vil_copy(vil_generic_image const* in, vil_generic_image* out)
{
#define assert_dimension_equal(dim) assert(in->dim() == out->dim())
//  assert_dimension_equal(planes);
  assert_dimension_equal(height);
  assert_dimension_equal(width);
//  assert_dimension_equal(components);
  assert_dimension_equal(bits_per_component);
#undef assert_dimension_equal
  assert((in->components() * in->planes()) == (out->components() * out->planes()));

  // int planes = in->planes();
  int height = in->height();
  int width = in->width();
  int components = in->components();
  int bits_per_component = in->bits_per_component();
  
  int chunksize_bits = bits_per_component * components * width;
  int chunksize = chunksize_bits / 8;
  assert(chunksize * 8 == chunksize_bits);
  // int numchunks = planes * height;

#if 0
  // Simple implementation copies one row at a time.
  unsigned char* buf = new unsigned char[chunksize];
  for(int y = 0; y < height; ++y) {
    in->get_section(buf, 0, y, width, 1);   // 0 was p
    out->put_section(buf, 0, y, width, 1);  // 0 was p
  }
#else
  // Simple implementation copies the whole buffer at once
  unsigned char* buf = new unsigned char[chunksize*height];
  in->get_section(buf, 0, 0, width, height);
  out->put_section(buf, 0, 0, width, height);
#endif
  delete [] buf;
}
