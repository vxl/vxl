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
  assert_dimension_equal(height);
  assert_dimension_equal(width);
  assert_dimension_equal(bits_per_component);
  assert_dimension_equal(planes);
  assert_dimension_equal(components);
#undef assert_dimension_equal
//assert((in.components() * in.planes()) == (out.components() * out.planes()));

  int height = in.height();
  int width = in.width();

  // Simple implementation copies the whole buffer at once
  // This is only valid if planes and components are identical,
  // not just their product.  Hence the assert above.
  unsigned char* buf = new unsigned char[in.get_size_bytes()];
#ifdef DEBUG
  vcl_cerr << "...vil_copy() doing get_section()" << vcl_endl;
#endif
  in.get_section(buf, 0, 0, width, height);
#ifdef DEBUG
  vcl_cerr << "...vil_copy() doing put_section()" << vcl_endl;
#endif
  out.put_section(buf, 0, 0, width, height);
#ifdef DEBUG
  vcl_cerr << "...vil_copy() done" << vcl_endl;
#endif
  delete[] buf;
}

vil_memory_image
vil_copy(vil_image const& src)
{
  vil_memory_image dst( src.planes(), src.width(), src.height(), src.components(),
                        src.bits_per_component(), src.component_format() );
  vil_copy( src, dst );
  return dst;
}
