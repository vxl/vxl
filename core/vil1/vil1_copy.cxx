//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// Class: vil_copy
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 16 Feb 00
//
//-----------------------------------------------------------------------------

#include "vil_copy.h"

#include <vcl_cassert.h>
#include <vcl_climits.h>

#include <vil/vil_buffer.h>
#include <vil/vil_image.h>

void vil_copy(vil_image const& in, vil_image& out)
{
#define assert_dimension_equal(dim) assert(in.dim() == out.dim())
//  assert_dimension_equal(planes);
  assert_dimension_equal(height);
  assert_dimension_equal(width);
//  assert_dimension_equal(components);
  assert_dimension_equal(bits_per_component);
#undef assert_dimension_equal
  assert((in.components() * in.planes()) == (out.components() * out.planes()));

  // int planes = in.planes();
  int height = in.height();
  int width = in.width();
  int components = in.components();
  int bits_per_component = in.bits_per_component();
  
  int rowsize_bits = bits_per_component * components * width;
  int rowsize_bytes = rowsize_bits / CHAR_BIT;
  assert(rowsize_bytes * CHAR_BIT == rowsize_bits); // think again
  // int numchunks = planes * height;

#if 0
  // Simple implementation copies one row at a time.
  vil_buffer<unsigned char> buf(rowsize_bytes);
  for (int y = 0; y < height; ++y) {
    in .get_section(buf.data(), 0, y, width, 1);   // 0 was p
    out.put_section(buf.data(), 0, y, width, 1);  // 0 was p
  }
#else
  // Simple implementation copies the whole buffer at once
  vil_buffer<unsigned char> buf(rowsize_bytes*height);
  //cerr << "...vil_copy() doing get_section()" << endl;
  in .get_section(buf.data(), 0, 0, width, height);
  //cerr << "...vil_copy() doing put_section()" << endl;
  out.put_section(buf.data(), 0, 0, width, height);
  //cerr << "...vil_copy() done" << endl;
#endif
}
