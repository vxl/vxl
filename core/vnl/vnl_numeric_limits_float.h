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
VCL_DEFINE_SPECIALIZATION
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

