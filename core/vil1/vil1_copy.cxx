// This is ./vxl/vil/vil_copy.cxx
#ifdef __GNUC__
#pragma implementation
#endif

//:
// \file
// vil_copy
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date 16 Feb 00
//
// \verbatim
// Modifications:
// 7 June 2001 - Peter Vanroose - fixed rowsize_bytes for 1-bit pixel type
// \endverbatim
//
//-----------------------------------------------------------------------------

#include "vil_copy.h"

#include <vcl_cassert.h>
#include <vcl_climits.h>
#include <vcl_vector.h>

#include <vil/vil_image.h>
#include <vil/vil_memory_image.h>

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

  int height = in.height();
  int width = in.width();
  int components = in.components();
  int bits_per_component = in.bits_per_component();

  int rowsize_bits = bits_per_component * components * width;
  int rowsize_bytes = (rowsize_bits+CHAR_BIT-1) / CHAR_BIT;
//assert(rowsize_bytes * CHAR_BIT == rowsize_bits); // not for e.g. 1-bit pixels

  // Simple implementation copies the whole buffer at once
  vcl_vector<unsigned char> buf(rowsize_bytes*height);
#ifdef DEBUG
  vcl_cerr << "...vil_copy() doing get_section()" << vcl_endl;
#endif
  in .get_section(/* xxx */&buf[0], 0, 0, width, height);
#ifdef DEBUG
  vcl_cerr << "...vil_copy() doing put_section()" << vcl_endl;
#endif
  out.put_section(/* xxx */&buf[0], 0, 0, width, height);
#ifdef DEBUG
  vcl_cerr << "...vil_copy() done" << vcl_endl;
#endif
}

vil_memory_image
vil_copy(vil_image const& src)
{
  vil_memory_image dst( src.planes(), src.width(), src.height(), src.components(),
                        src.bits_per_component(), src.component_format() );
  vil_copy( src, dst );
  return dst;
}
