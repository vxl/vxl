// This is vxl/vnl/vnl_numeric_traits.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// vnl_numeric_traits
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 12 Feb 98
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_numeric_traits.h>
#include <vcl_complex.h>
#include <vxl_config.h>

static const long s16 = 0x7fffL;
static const unsigned long u16 = 0xffffL;
static const long s32 = 0x7fffffffL;
static const unsigned long u32 = 0xffffffffL;
#if VXL_HAS_INT_64 // need this arithmetic magic to avoid compiler errors
static const vxl_uint_64 u64 = (vxl_uint_64)(-1);
static const vxl_sint_64 s64 = u64/2;
#else // dummy
static const long s64 = 0L;
static const unsigned long u64 = 0L;
#endif

#if !VCL_CAN_STATIC_CONST_INIT_INT

#ifndef NO_STD_BOOL
const bool vnl_numeric_traits<bool>::zero = false;
const bool vnl_numeric_traits<bool>::one = true;
const bool vnl_numeric_traits<bool>::maxval = true;
#endif

const char vnl_numeric_traits<char>::zero = 0;
const char vnl_numeric_traits<char>::one = 1;
const char vnl_numeric_traits<char>::maxval = 127;

const unsigned char vnl_numeric_traits<unsigned char>::zero = 0;
const unsigned char vnl_numeric_traits<unsigned char>::one = 1;
const unsigned char vnl_numeric_traits<unsigned char>::maxval = 255;

const signed char vnl_numeric_traits<signed char>::zero = 0;
const signed char vnl_numeric_traits<signed char>::one = 1;
const signed char vnl_numeric_traits<signed char>::maxval = 127;

const short vnl_numeric_traits<short>::zero = 0;
const short vnl_numeric_traits<short>::one = 1;

const unsigned short vnl_numeric_traits<unsigned short>::zero = 0;
const unsigned short vnl_numeric_traits<unsigned short>::one = 1;

const int vnl_numeric_traits<int>::zero = 0;
const int vnl_numeric_traits<int>::one = 1;

const unsigned int vnl_numeric_traits<unsigned int>::zero = 0;
const unsigned int vnl_numeric_traits<unsigned int>::one = 1;

const long vnl_numeric_traits<long>::zero = 0L;
const long vnl_numeric_traits<long>::one = 1L;

const unsigned long vnl_numeric_traits<unsigned long>::zero = 0L;
const unsigned long vnl_numeric_traits<unsigned long>::one = 1L;

#endif

const short vnl_numeric_traits<short>::maxval = s16;
const unsigned short vnl_numeric_traits<unsigned short>::maxval = u16;
const int vnl_numeric_traits<int>::maxval = sizeof(int)==4?s32:s16;
const unsigned int vnl_numeric_traits<unsigned int>::maxval = sizeof(unsigned int)==4?u32:u16;
const long vnl_numeric_traits<long>::maxval = sizeof(long)==8?s64:s32;
const unsigned long vnl_numeric_traits<unsigned long>::maxval = sizeof(unsigned long)==8?u64:u32;

#if !VCL_CAN_STATIC_CONST_INIT_FLOAT

const float vnl_numeric_traits<float>::zero = 0.0F;
const float vnl_numeric_traits<float>::one = 1.0F;
const float vnl_numeric_traits<float>::maxval = 3.40282346638528860e+38F;

const double vnl_numeric_traits<double>::zero = 0.0;
const double vnl_numeric_traits<double>::one = 1.0;
const double vnl_numeric_traits<double>::maxval = 1.7976931348623157E+308;

const long double vnl_numeric_traits<long double>::zero = 0.0;
const long double vnl_numeric_traits<long double>::one = 1.0;
const long double vnl_numeric_traits<long double>::maxval = 1.7976931348623157E+308;

#endif

//--------------------------------------------------------------------------------

const vcl_complex<float> vnl_numeric_traits<vcl_complex<float> >::zero = 0.0;
const vcl_complex<float> vnl_numeric_traits<vcl_complex<float> >::one = 1.0;

const vcl_complex<double> vnl_numeric_traits<vcl_complex<double> >::zero = 0.0;
const vcl_complex<double> vnl_numeric_traits<vcl_complex<double> >::one = 1.0;

const vcl_complex<long double> vnl_numeric_traits<vcl_complex<long double> >::zero = 0.0;
const vcl_complex<long double> vnl_numeric_traits<vcl_complex<long double> >::one = 1.0;

//--------------------------------------------------------------------------------
