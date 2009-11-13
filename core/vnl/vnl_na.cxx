// This is core/vnl/vnl_na.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vnl_na.h"
#include <vxl_config.h>
#include <vcl_istream.h>
#include <vcl_ios.h>

//: A particular qNaN to indicate not available.
// This returns the bit pattern 0x7ff00000000007a2, as used by Octave and R
// Don't assume that any VXL functions will treat the value as NA rather than NaN, unless
// explicitly documented.
double vnl_na()
{
  double a;

#if VXL_HAS_INT_64
  *reinterpret_cast<vxl_uint_64*>(&a) = 0x7ff00000000007a2;

#else
# if VXL_BIG_ENDIAN
#  define vnl_math_hw 0;
#  define vnl_math_lw 1;
# else  // VXL_LITTLE_ENDIAN
#  define vnl_math_hw 1;
#  define vnl_math_lw 0;
# endif
  *reinterpret_cast<vxl_uint_32*>(&a)[hi]=0x7ff00000
  *reinterpret_cast<vxl_uint_32*>(&a)[lo]=0x000007a2
#endif

  return a;
}

//: True if parameter is specific NA qNaN.
// Tests for bit pattern 0x7ff00000000007a2, as used by Octave and R
bool vnl_na_isna(double x)
{
#if VXL_HAS_INT_64
  return ((*reinterpret_cast<vxl_uint_64*>(&x))&0xfff7ffffffffffff)
    == 0x7ff00000000007a2;

#else

  return ((reinterpret_cast<vxl_int_32*>(&a)[hi])&0fff7ffff))==0x7ff00000 &&
  reinterpret_cast<vxl_int_32*>(&a)[lo]==0x000007a2
#endif
}


//: Read a floating point number or "NA" from a stream.
void vnl_na_double_parse(vcl_istream &is, double& x)
{
  if (!is) return;
  is >> x;

  if (!!is || is.eof()) return;
  is.clear();

  char c=' ';
  is >> c;
  if (c != 'N' && c!='n')
  {
    is.putback(c);
    is.clear(vcl_ios::badbit);
    return;
  }
  is >> c;
  if (c != 'A' && c!='a')
  {
    is.putback(c);
    is.clear(vcl_ios::badbit);
    return;
  }
  x = vnl_na();

}

//----------------------------------------------------------------------
