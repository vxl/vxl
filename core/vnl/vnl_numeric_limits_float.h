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
//-*- c++ -*-------------------------------------------------------------------
#ifndef vnl_numeric_limits_float_h_
#define vnl_numeric_limits_float_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : numeric_limits_float
//
// .SECTION Description
//    numeric_limits specialized for float.
//
// .NAME        numeric_limits_float - Float numeric limits.
// .FILE        vnl/numeric_limits_float.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 28 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_compiler.h>
#include <vnl/vnl_numeric_limits.h>

// IEEE 754 single precision
VCL_DECLARE_SPECIALIZATION(class vnl_numeric_limits<float>)

class vnl_numeric_limits<float> {
public:
  static const bool is_specialized VCL_STATIC_CONST_INIT_INT(true);
  inline static float min() { return 1.17549435E-38F; }
  inline static float max() { return 3.40282347E+38F; }
  static const int digits   VCL_STATIC_CONST_INIT_INT(24);
  static const int digits10 VCL_STATIC_CONST_INIT_INT(6);
  static const bool is_signed  VCL_STATIC_CONST_INIT_INT(true);
  static const bool is_integer VCL_STATIC_CONST_INIT_INT(false);
  static const bool is_exact   VCL_STATIC_CONST_INIT_INT(false);
  static const int radix VCL_STATIC_CONST_INIT_INT(2);
  inline static float epsilon()     { return 1.19209290E-07F; }
  inline static float round_error() { return 0.5F; }
  static const int min_exponent   VCL_STATIC_CONST_INIT_INT(-125);
  static const int min_exponent10 VCL_STATIC_CONST_INIT_INT(-37);
  static const int max_exponent   VCL_STATIC_CONST_INIT_INT(128);
  static const int max_exponent10 VCL_STATIC_CONST_INIT_INT(38);
  static const bool has_infinity      VCL_STATIC_CONST_INIT_INT(true);
  static const bool has_quiet_NaN     VCL_STATIC_CONST_INIT_INT(true);
  static const bool has_signaling_NaN VCL_STATIC_CONST_INIT_INT(true);
  static const bool has_denorm        VCL_STATIC_CONST_INIT_INT(false);
  static float infinity();
  static float quiet_NaN();
  static float signaling_NaN();
  inline static float denorm_min()    { return min(); }
  static const bool is_iec559  VCL_STATIC_CONST_INIT_INT(true);
  static const bool is_bounded VCL_STATIC_CONST_INIT_INT(true);
  static const bool is_modulo  VCL_STATIC_CONST_INIT_INT(false);
  static const bool traps      VCL_STATIC_CONST_INIT_INT(true);
  static const bool tinyness_before VCL_STATIC_CONST_INIT_INT(true);
  static const vnl_float_round_style round_style VCL_STATIC_CONST_INIT_INT(vnl_round_to_nearest);
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS numeric_limits_float.

