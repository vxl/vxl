// This is core/vil/vil_na.cxx
//:
// \file
// This file is a cut-and-paste of vnl_na.cxx

#include <istream>
#include <ios>
#include "vil_na.h"
#include <vxl_config.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A particular qNaN to indicate not available.
// This returns the bit pattern 0x7ff00000000007a2, as used by Octave and R
// Don't assume that any VXL functions will treat the value as NA rather than NaN, unless
// explicitly documented.
double vil_na(double)
{
  double a;

#if VXL_HAS_INT_64
  *reinterpret_cast<vxl_uint_64*>(&a) = 0x7ff00000000007a2LL;
#else
# if VXL_BIG_ENDIAN
#  define hw 0
#  define lw 1
# else  // VXL_LITTLE_ENDIAN
#  define hw 1
#  define lw 0
# endif
  reinterpret_cast<vxl_uint_32*>(&a)[hw]=0x7ff00000;
  reinterpret_cast<vxl_uint_32*>(&a)[lw]=0x000007a2;
#endif

  return a;
}



//: A particular qNaN to indicate not available.
// This returns the bit pattern 0x7f8007a2
// Don't assume that any VXL functions will treat the value as NA rather than NaN, unless
// explicitly documented.
float vil_na(float)
{
  float a;

  *reinterpret_cast<vxl_uint_32*>(&a) = 0x7f8007a2L;

  return a;
}


//: True if parameter is specific NA qNaN.
// Tests for bit pattern 0x7ff00000000007a2, as used by Octave and R
bool vil_na_isna(double x)
{
#if VXL_HAS_INT_64
  return ((*reinterpret_cast<vxl_uint_64*>(&x))&0xfff7ffffffffffffLL) // ignore signalling bit
    == 0x7ff00000000007a2LL;
#else
  return ((reinterpret_cast<vxl_int_32*>(&x)[hw]) & 0xfff7ffff) == 0x7ff00000 &&
         reinterpret_cast<vxl_int_32*>(&x)[lw] == 0x000007a2;
#endif
}

//: True if parameter is specific NA qNaN.
// Tests for bit pattern 0x7F8007a2
bool vil_na_isna(float x)
{
  return ((*reinterpret_cast<vxl_uint_32*>(&x))&0xffbfffffL) // ignore signalling bit
    == 0x7f8007a2L;
}
//----------------------------------------------------------------------
