//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "vnl_numeric_traits.h"
#endif
//
// Class: vnl_numeric_traits
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 12 Feb 98
// Modifications:
//   980212 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_complex.h>

//#if defined(VCL_SUNPRO_CC)
//# define signed 
//#endif

#if defined(VCL_GCC_27) || defined(VCL_GCC_EGCS)
# define VCL_STATIC_CONST_REINIT_INT(x)
# undef VCL_IMPLEMENT_STATIC_CONSTS
# define VCL_IMPLEMENT_STATIC_CONSTS 1
#else
# define VCL_STATIC_CONST_REINIT_INT(x) = x
#endif

#if defined(_MSC_VER)
// VC60
#define VCL_IMPLEMENT_STATIC_CONSTS 1
#define VCL_STATIC_CONST_REINIT_INT(x) = x
#endif

#if VCL_IMPLEMENT_STATIC_CONSTS

#ifndef NO_STD_BOOL
const bool vnl_numeric_traits<bool>::zero VCL_STATIC_CONST_REINIT_INT(0);
const bool vnl_numeric_traits<bool>::one VCL_STATIC_CONST_REINIT_INT(1);
#endif

const unsigned char vnl_numeric_traits<unsigned char>::zero VCL_STATIC_CONST_REINIT_INT(0);
const unsigned char vnl_numeric_traits<unsigned char>::one VCL_STATIC_CONST_REINIT_INT(1);

const signed char vnl_numeric_traits<signed char>::zero VCL_STATIC_CONST_REINIT_INT(0);
const signed char vnl_numeric_traits<signed char>::one VCL_STATIC_CONST_REINIT_INT(1);

const unsigned short vnl_numeric_traits<unsigned short>::zero VCL_STATIC_CONST_REINIT_INT(0);
const unsigned short vnl_numeric_traits<unsigned short>::one VCL_STATIC_CONST_REINIT_INT(1);

const signed short vnl_numeric_traits<signed short>::zero VCL_STATIC_CONST_REINIT_INT(0);
const signed short vnl_numeric_traits<signed short>::one VCL_STATIC_CONST_REINIT_INT(1);

const unsigned int vnl_numeric_traits<unsigned int>::zero VCL_STATIC_CONST_REINIT_INT(0);
const unsigned int vnl_numeric_traits<unsigned int>::one VCL_STATIC_CONST_REINIT_INT(1);

const signed int vnl_numeric_traits<signed int>::zero VCL_STATIC_CONST_REINIT_INT(0);
const signed int vnl_numeric_traits<signed int>::one VCL_STATIC_CONST_REINIT_INT(1);

const signed long vnl_numeric_traits<signed long>::zero VCL_STATIC_CONST_REINIT_INT(0);
const signed long vnl_numeric_traits<signed long>::one VCL_STATIC_CONST_REINIT_INT(1);

const unsigned long vnl_numeric_traits<unsigned long>::zero VCL_STATIC_CONST_REINIT_INT(0);
const unsigned long vnl_numeric_traits<unsigned long>::one VCL_STATIC_CONST_REINIT_INT(1);

#endif

// egcs certainly needs these to be implemented
// fsm: so does gcc-2.95.
// PVr: and also gcc-2.8.1, of course.
const float vnl_numeric_traits<float>::zero VCL_STATIC_CONST_REINIT_INT(0.0F);
const float vnl_numeric_traits<float>::one VCL_STATIC_CONST_REINIT_INT(1.0F);

const double vnl_numeric_traits<double>::zero VCL_STATIC_CONST_REINIT_INT(0.0);
const double vnl_numeric_traits<double>::one VCL_STATIC_CONST_REINIT_INT(1.0);

//--------------------------------------------------------------------------------

// declaring these const crashes 2.7.2
vnl_double_complex vnl_numeric_traits<vnl_double_complex >::zero = 0.0;
vnl_double_complex vnl_numeric_traits<vnl_double_complex >::one = 1.0;

vnl_float_complex vnl_numeric_traits<vnl_float_complex >::zero = 0.0;
vnl_float_complex vnl_numeric_traits<vnl_float_complex >::one = 1.0;

