// This is core/vnl/vnl_numeric_traits.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// Created: 12 Feb 98
//
//-----------------------------------------------------------------------------

#include <complex>
#include "vnl_numeric_traits.h"
#include <vcl_compiler.h>
#include <vxl_config.h>

constexpr bool vnl_numeric_traits<bool>::zero VCL_STATIC_CONST_INIT_INT_DEFN(false);
constexpr char vnl_numeric_traits<char>::zero VCL_STATIC_CONST_INIT_INT_DEFN(0);
constexpr unsigned char vnl_numeric_traits<unsigned char>::zero VCL_STATIC_CONST_INIT_INT_DEFN(0);
constexpr signed char vnl_numeric_traits<signed char>::zero VCL_STATIC_CONST_INIT_INT_DEFN(0);
constexpr short vnl_numeric_traits<short>::zero VCL_STATIC_CONST_INIT_INT_DEFN(0);
constexpr unsigned short vnl_numeric_traits<unsigned short>::zero VCL_STATIC_CONST_INIT_INT_DEFN(0);
constexpr int vnl_numeric_traits<int>::zero VCL_STATIC_CONST_INIT_INT_DEFN(0);
constexpr unsigned int vnl_numeric_traits<unsigned int>::zero VCL_STATIC_CONST_INIT_INT_DEFN(0);
constexpr long vnl_numeric_traits<long>::zero VCL_STATIC_CONST_INIT_INT_DEFN(0);
constexpr unsigned long vnl_numeric_traits<unsigned long>::zero VCL_STATIC_CONST_INIT_INT_DEFN(0);
#if VCL_HAS_LONG_LONG
constexpr long long vnl_numeric_traits<long long>::zero VCL_STATIC_CONST_INIT_INT_DEFN(0);
constexpr unsigned long long vnl_numeric_traits<unsigned long long>::zero VCL_STATIC_CONST_INIT_INT_DEFN(0);
#endif

constexpr bool vnl_numeric_traits<bool>::one VCL_STATIC_CONST_INIT_INT_DEFN(true);
constexpr char vnl_numeric_traits<char>::one VCL_STATIC_CONST_INIT_INT_DEFN(1);
constexpr unsigned char vnl_numeric_traits<unsigned char>::one VCL_STATIC_CONST_INIT_INT_DEFN(1);
constexpr signed char vnl_numeric_traits<signed char>::one VCL_STATIC_CONST_INIT_INT_DEFN(1);
constexpr short vnl_numeric_traits<short>::one VCL_STATIC_CONST_INIT_INT_DEFN(1);
constexpr unsigned short vnl_numeric_traits<unsigned short>::one VCL_STATIC_CONST_INIT_INT_DEFN(1);
constexpr int vnl_numeric_traits<int>::one VCL_STATIC_CONST_INIT_INT_DEFN(1);
constexpr unsigned int vnl_numeric_traits<unsigned int>::one VCL_STATIC_CONST_INIT_INT_DEFN(1);
constexpr long vnl_numeric_traits<long>::one VCL_STATIC_CONST_INIT_INT_DEFN(1);
constexpr unsigned long vnl_numeric_traits<unsigned long>::one VCL_STATIC_CONST_INIT_INT_DEFN(1);
#if VCL_HAS_LONG_LONG
constexpr long long vnl_numeric_traits<long long>::one VCL_STATIC_CONST_INIT_INT_DEFN(1);
constexpr unsigned long long vnl_numeric_traits<unsigned long long>::one VCL_STATIC_CONST_INIT_INT_DEFN(1);
#endif

constexpr bool vnl_numeric_traits<bool>::maxval VCL_STATIC_CONST_INIT_INT_DEFN(true);
constexpr char vnl_numeric_traits<char>::maxval VCL_STATIC_CONST_INIT_INT_DEFN(char(255)<0?127:char(255));
//  It is 127 when "char" is signed and 255 when "char" is unsigned.
constexpr unsigned char vnl_numeric_traits<unsigned char>::maxval VCL_STATIC_CONST_INIT_INT_DEFN(255);
constexpr signed char vnl_numeric_traits<signed char>::maxval VCL_STATIC_CONST_INIT_INT_DEFN(127);
constexpr short vnl_numeric_traits<short>::maxval VCL_STATIC_CONST_INIT_INT_DEFN( s16 );
constexpr unsigned short vnl_numeric_traits<unsigned short>::maxval VCL_STATIC_CONST_INIT_INT_DEFN( u16 );
constexpr int vnl_numeric_traits<int>::maxval VCL_STATIC_CONST_INIT_INT_DEFN( sizeof(int)==4?s32:s16 );
constexpr unsigned int vnl_numeric_traits<unsigned int>::maxval VCL_STATIC_CONST_INIT_INT_DEFN( sizeof(unsigned int)==4?u32:u16 );
constexpr long vnl_numeric_traits<long>::maxval VCL_STATIC_CONST_INIT_INT_DEFN( sizeof(long)==8?s64:s32 );
constexpr unsigned long vnl_numeric_traits<unsigned long>::maxval VCL_STATIC_CONST_INIT_INT_DEFN( sizeof(unsigned long)==8?u64:u32 );
#if VCL_HAS_LONG_LONG
constexpr long long vnl_numeric_traits<long long>::maxval VCL_STATIC_CONST_INIT_INT_DEFN( sizeof(long long)==8?s64:s32 );
constexpr unsigned long long vnl_numeric_traits<unsigned long long>::maxval VCL_STATIC_CONST_INIT_INT_DEFN( sizeof(unsigned long long)==8?u64:u32 );
#endif


constexpr float vnl_numeric_traits<float>::zero VCL_STATIC_CONST_INIT_FLOAT_DEFN(0.0F);
constexpr double vnl_numeric_traits<double>::zero VCL_STATIC_CONST_INIT_FLOAT_DEFN(0.0);
constexpr long double vnl_numeric_traits<long double>::zero VCL_STATIC_CONST_INIT_FLOAT_DEFN(0.0);

constexpr float vnl_numeric_traits<float>::one VCL_STATIC_CONST_INIT_FLOAT_DEFN(1.0F);
constexpr double vnl_numeric_traits<double>::one VCL_STATIC_CONST_INIT_FLOAT_DEFN(1.0);
constexpr long double vnl_numeric_traits<long double>::one VCL_STATIC_CONST_INIT_FLOAT_DEFN(1.0);

constexpr float vnl_numeric_traits<float>::maxval VCL_STATIC_CONST_INIT_FLOAT_DEFN(3.40282346638528860e+38F);
constexpr double vnl_numeric_traits<double>::maxval VCL_STATIC_CONST_INIT_FLOAT_DEFN(1.7976931348623157E+308);
constexpr long double vnl_numeric_traits<long double>::maxval VCL_STATIC_CONST_INIT_FLOAT_DEFN(1.7976931348623157E+308);

// Must use constructor-call syntax for initialization of complex specializations.
// std::complex
//
// 'complex<float>' is not suitable for being a constexpr because it is not a literal type
//                   because it is not an aggregate and has no constexpr
//                  constructors other than copy or move constructors
const std::complex<float> vnl_numeric_traits<std::complex<float> >::zero(0.0F);
const std::complex<double> vnl_numeric_traits<std::complex<double> >::zero(0.0);
const std::complex<long double> vnl_numeric_traits<std::complex<long double> >::zero(0.0L);

const std::complex<float> vnl_numeric_traits<std::complex<float> >::one(1.0F);
const std::complex<double> vnl_numeric_traits<std::complex<double> >::one(1.0);
const std::complex<long double> vnl_numeric_traits<std::complex<long double> >::one(1.0L);

// Unknown, so undefined. Will cause link errors if someone refers to it.
//const std::complex<float> vnl_numeric_traits<std::complex<float> >::maxval;
//const std::complex<double> vnl_numeric_traits<std::complex<double> >::maxval;
//const std::complex<long double> vnl_numeric_traits<std::complex<long double> >::maxval;

//--------------------------------------------------------------------------------
