// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
#ifdef __GNUC__
#pragma implementation "vnl_numeric_limits.h"
#pragma implementation "vnl_numeric_limits_float.h"
#pragma implementation "vnl_numeric_limits_int.h"
#pragma implementation "vnl_numeric_limits_double.h"
#endif
//
// Class: numeric_limits
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 28 Aug 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include "vnl_numeric_limits.h"

// FIXME: sunpro5.0 warns about anachronistic syntax without the
// template <> f, but 2.95 and egcs emit error messages. - fsm
#include <vcl/vcl_compiler.h>
#if defined(VCL_GCC_295) || defined(VCL_EGCS)
# undef VCL_DECLARE_SPECIALIZATION
# define VCL_DECLARE_SPECIALIZATION(f) /* template <> f; */
#endif

union vnl_numeric_limits_double_nan {
  double nan;
  unsigned char x[8];
  
  vnl_numeric_limits_double_nan() {
    x[0] = 0x7f;
    x[1] = 0xff;
    x[2] = 0xff;
    x[3] = 0xff;
    x[4] = 0xff;
    x[5] = 0xff;
    x[6] = 0xff;
    x[7] = 0xff;
  }
};
static vnl_numeric_limits_double_nan dnan;

union vnl_numeric_limits_double_inf {
  double inf;
  unsigned char x[8];
  
  vnl_numeric_limits_double_inf() {
    x[0] = 0x7f;
    x[1] = 0xf0;
    x[2] = 0x00;
    x[3] = 0x00;
    x[4] = 0x00;
    x[5] = 0x00;
    x[6] = 0x00;
    x[7] = 0x00;
  }
};
static vnl_numeric_limits_double_inf dinf;

VCL_DECLARE_SPECIALIZATION( double vnl_numeric_limits<double>::infinity() );
double vnl_numeric_limits<double>::infinity()
{
  return dinf.inf;
}

VCL_DECLARE_SPECIALIZATION( double vnl_numeric_limits<double>::quiet_NaN() );
double vnl_numeric_limits<double>::quiet_NaN()
{
  return dnan.nan;
}

VCL_DECLARE_SPECIALIZATION( double vnl_numeric_limits<double>::signaling_NaN() );
double vnl_numeric_limits<double>::signaling_NaN()
{
  return quiet_NaN();
}

// *****************************************************************************
union vnl_numeric_limits_float_nan {
  float nan;
  unsigned char x[4];
  
  vnl_numeric_limits_float_nan() {
    x[0] = 0x7f;
    x[1] = 0xff;
    x[2] = 0xff;
    x[3] = 0xff;
  }
};
static vnl_numeric_limits_float_nan fnan;

union vnl_numeric_limits_float_inf {
  float inf;
  unsigned char x[4];
  
  vnl_numeric_limits_float_inf() {
    x[0] = 0x7f;
    x[1] = 0x80;
    x[2] = 0x00;
    x[3] = 0x00;
  }
};
static vnl_numeric_limits_float_inf finf;

VCL_DECLARE_SPECIALIZATION( float vnl_numeric_limits<float>::infinity() );
float vnl_numeric_limits<float>::infinity()
{
  return finf.inf;
}

VCL_DECLARE_SPECIALIZATION( float vnl_numeric_limits<float>::quiet_NaN() );
float vnl_numeric_limits<float>::quiet_NaN()
{
  return fnan.nan;
}

VCL_DECLARE_SPECIALIZATION( float vnl_numeric_limits<float>::signaling_NaN() );
float vnl_numeric_limits<float>::signaling_NaN()
{
  return quiet_NaN();
}

#if defined(VCL_GCC)
# if !defined(VCL_GCC_EGCS)
template class vnl_numeric_limits<float>;
template class vnl_numeric_limits<double>;
# endif
#else
// #pragma instantiate vnl_numeric_limits<float>
// #pragma instantiate vnl_numeric_limits<double>

// Double inits
const bool vnl_numeric_limits<double>::is_specialized = true;
const int vnl_numeric_limits<double>::digits   = 53;
const int vnl_numeric_limits<double>::digits10 =  15;
const bool vnl_numeric_limits<double>::is_signed  = true;
const bool vnl_numeric_limits<double>::is_integer = false;
const bool vnl_numeric_limits<double>::is_exact   = false;
const int vnl_numeric_limits<double>::radix = 2;
const int vnl_numeric_limits<double>::min_exponent   = -1021;
const int vnl_numeric_limits<double>::min_exponent10 = -307;
const int vnl_numeric_limits<double>::max_exponent   = 1024;
const int vnl_numeric_limits<double>::max_exponent10 = 308;
const bool vnl_numeric_limits<double>::has_infinity      = true;
const bool vnl_numeric_limits<double>::has_quiet_NaN     = true;
const bool vnl_numeric_limits<double>::has_signaling_NaN = true;
const bool vnl_numeric_limits<double>::has_denorm        = true;
const bool vnl_numeric_limits<double>::is_iec559  = true;
const bool vnl_numeric_limits<double>::is_bounded = true;
const bool vnl_numeric_limits<double>::is_modulo  = false;
const bool vnl_numeric_limits<double>::traps      = true;
const bool vnl_numeric_limits<double>::tinyness_before = true;
const vnl_float_round_style vnl_numeric_limits<double>::round_style = vnl_round_to_nearest;

// Float inits
const bool vnl_numeric_limits<float>::is_specialized = true;
const int vnl_numeric_limits<float>::digits   = 24;
const int vnl_numeric_limits<float>::digits10 =  6;
const bool vnl_numeric_limits<float>::is_signed  = true;
const bool vnl_numeric_limits<float>::is_integer = false;
const bool vnl_numeric_limits<float>::is_exact   = false;
const int vnl_numeric_limits<float>::radix = 2;
const int vnl_numeric_limits<float>::min_exponent   = -125;
const int vnl_numeric_limits<float>::min_exponent10 = -37;
const int vnl_numeric_limits<float>::max_exponent   = 128;
const int vnl_numeric_limits<float>::max_exponent10 = 38;
const bool vnl_numeric_limits<float>::has_infinity      = true;
const bool vnl_numeric_limits<float>::has_quiet_NaN     = true;
const bool vnl_numeric_limits<float>::has_signaling_NaN = true;
const bool vnl_numeric_limits<float>::has_denorm        = false;
const bool vnl_numeric_limits<float>::is_iec559  = true;
const bool vnl_numeric_limits<float>::is_bounded = true;
const bool vnl_numeric_limits<float>::is_modulo  = false;
const bool vnl_numeric_limits<float>::traps      = true;
const bool vnl_numeric_limits<float>::tinyness_before = true;
const vnl_float_round_style vnl_numeric_limits<float>::round_style = vnl_round_to_nearest;
#endif
