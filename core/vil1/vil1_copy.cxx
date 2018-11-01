// This is core/vil1/vil1_copy.cxx
//:
// \file
// \brief Contains function vil1_copy()
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date 16 Feb 00
//
// \verbatim
//  Modifications
//   7 June 2001 - Peter Vanroose - fixed rowsize_bytes for 1-bit pixel type
// \endverbatim
//-----------------------------------------------------------------------------

#include "vil1_copy.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image.h>

void vil1_copy(vil1_image const& in, vil1_image& out)
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
  auto* buf = new unsigned char[in.get_size_bytes()];
#ifdef DEBUG
  std::cerr << "...vil1_copy() doing get_section()\n";
#endif
  in.get_section(buf, 0, 0, width, height);
#ifdef DEBUG
  std::cerr << "...vil1_copy() doing put_section()\n";
#endif
  out.put_section(buf, 0, 0, width, height);
#ifdef DEBUG
  std::cerr << "...vil1_copy() done\n";
#endif
  delete[] buf;
}

vil1_memory_image
vil1_copy(vil1_image const& src)
{
  vil1_memory_image dst( src.planes(), src.width(), src.height(), src.components(),
                         src.bits_per_component(), src.component_format() );
  vil1_copy( src, dst );
  return dst;
}
