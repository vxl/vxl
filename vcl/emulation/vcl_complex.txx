//-*- c++ -*-------------------------------------------------------------------
//
// Module: complex
// Purpose: instantiations macro for complex<TYPE>
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: MOT
// Modifications:
//   200498 AWF Now GCC uses this complex as well.
//-----------------------------------------------------------------------------
#include <vcl/vcl_compiler.h>

// This assumes that emulation/complex.cxx is included implies emulation/complex.h included
#include "vcl_complex.h"

#ifdef __SUNPRO_CC
#undef VCL_INSTANTIATE_INLINE
#define VCL_INSTANTIATE_INLINE(fn_decl)  template  fn_decl ;
#endif

#undef VCL_COMPLEX_INSTANTIATE
#define VCL_COMPLEX_INSTANTIATE(FLOAT)\
template class vcl_complex<FLOAT >; \
VCL_INSTANTIATE_INLINE(ostream& operator<<(ostream&,vcl_complex<FLOAT >const&));\
VCL_INSTANTIATE_INLINE(bool operator==(vcl_complex<FLOAT >const&,vcl_complex<FLOAT >const&));\
VCL_INSTANTIATE_INLINE(bool operator==(FLOAT,vcl_complex<FLOAT >const&));	\
VCL_INSTANTIATE_INLINE(bool operator==(vcl_complex<FLOAT >const&,FLOAT));	\
VCL_INSTANTIATE_INLINE(FLOAT imag(vcl_complex<FLOAT >const&));		\
VCL_INSTANTIATE_INLINE(FLOAT real(vcl_complex<FLOAT >const&));		\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > sqrt (vcl_complex<FLOAT >const& x));\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > operator + (vcl_complex<FLOAT >const&,vcl_complex<FLOAT >const&));\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > operator + (vcl_complex<FLOAT >const&,FLOAT));\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > operator + (FLOAT,vcl_complex<FLOAT >const&));\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > operator - (vcl_complex<FLOAT >const&,vcl_complex<FLOAT >const&));\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > operator - (vcl_complex<FLOAT >const&,FLOAT));\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > operator - (FLOAT,vcl_complex<FLOAT >const&));\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > operator * (vcl_complex<FLOAT >const&,vcl_complex<FLOAT >const&));\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > operator * (vcl_complex<FLOAT >const&,FLOAT));\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > operator * (FLOAT,vcl_complex<FLOAT >const&));\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > operator / (vcl_complex<FLOAT >const&,vcl_complex<FLOAT >const&));\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > operator / (vcl_complex<FLOAT >const&,FLOAT));\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > operator / (FLOAT,vcl_complex<FLOAT >const&));\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > polar (FLOAT,FLOAT));		\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > pow (vcl_complex<FLOAT >const&,vcl_complex<FLOAT >const&));\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > pow (vcl_complex<FLOAT >const&,FLOAT));\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > pow (vcl_complex<FLOAT >const&,int));\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > pow (FLOAT,vcl_complex<FLOAT >const&));\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > exp (vcl_complex<FLOAT >const&));	\
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT > log (vcl_complex<FLOAT >const&));	\
VCL_INSTANTIATE_INLINE(FLOAT arg (vcl_complex<FLOAT >const&));		\
VCL_INSTANTIATE_INLINE(FLOAT abs (vcl_complex<FLOAT >const&));		\
VCL_INSTANTIATE_INLINE(FLOAT norm (vcl_complex<FLOAT >const&));         \
VCL_INSTANTIATE_INLINE(vcl_complex<FLOAT> conj (vcl_complex<FLOAT >const&));
