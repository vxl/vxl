//-*- c++ -*-------------------------------------------------------------------
#ifndef vnl_numeric_limits_double_h_
#define vnl_numeric_limits_double_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : numeric_limits_double
//
// .SECTION Description
//    numeric_limits_double is a class that awf hasn't documented properly. FIXME
//
// .NAME        numeric_limits_double - Undocumented class FIXME
// .FILE        vnl/numeric_limits_double.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 28 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_compiler.h>
#include <vnl/vnl_numeric_limits.h>

// IEEE 754 double precision with denorm
VCL_DEFINE_SPECIALIZATION
class vnl_numeric_limits<double> {
public:
  static const bool is_specialized VCL_STATIC_CONST_INIT_INT(true);
  inline static double min() { return 2.2250738585072014e-308; }
  inline static double max() { return 1.7976931348623157e+308; }
  static const int digits   VCL_STATIC_CONST_INIT_INT(53);
  static const int digits10 VCL_STATIC_CONST_INIT_INT(15);
  static const bool is_signed  VCL_STATIC_CONST_INIT_INT(true);
  static const bool is_integer VCL_STATIC_CONST_INIT_INT(false);
  static const bool is_exact   VCL_STATIC_CONST_INIT_INT(false);
  static const int radix VCL_STATIC_CONST_INIT_INT(2);
  inline static double epsilon()     { return 2.220446049250313e-16; }
  inline static double round_error() { return 0.5; }
  static const int min_exponent   VCL_STATIC_CONST_INIT_INT(-1021);
  static const int min_exponent10 VCL_STATIC_CONST_INIT_INT(-307);
  static const int max_exponent   VCL_STATIC_CONST_INIT_INT(1024);
  static const int max_exponent10 VCL_STATIC_CONST_INIT_INT(308);
  static const bool has_infinity      VCL_STATIC_CONST_INIT_INT(true);
  static const bool has_quiet_NaN     VCL_STATIC_CONST_INIT_INT(true);
  static const bool has_signaling_NaN VCL_STATIC_CONST_INIT_INT(true);
  static const bool has_denorm        VCL_STATIC_CONST_INIT_INT(false);
  static double infinity();
  static double quiet_NaN();
  static double signaling_NaN();
  inline static double denorm_min() { return /* 5e-324 */ min(); }
  static const bool is_iec559  VCL_STATIC_CONST_INIT_INT(true);
  static const bool is_bounded VCL_STATIC_CONST_INIT_INT(true);
  static const bool is_modulo  VCL_STATIC_CONST_INIT_INT(false);
  static const bool traps      VCL_STATIC_CONST_INIT_INT(true);
  static const bool tinyness_before VCL_STATIC_CONST_INIT_INT(true);
  static const vnl_float_round_style round_style VCL_STATIC_CONST_INIT_INT(vnl_round_to_nearest);
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS numeric_limits_double.
