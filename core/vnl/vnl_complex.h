//-*- c++ -*-------------------------------------------------------------------
#ifndef vnl_complex_h_
#define vnl_complex_h_
// vnl_complex :
//
// .SECTION Description
//
//     OK, we don't want everyone to pay for complex when they don't need it, as
//     its ratio of expense to frequency of use is high. So we insist that
//     vnl_complex.h is included by any file in vnl wishing to use complex numbers.
//     
//     However some classes, like math, are really namespaces, and we would like
//     to add complex overloads to those namespaces here.  Until the compilers
//     catch up, we can't do that, so we use this hack it. The hack tells
//     headers that might declare complex stuff to do so only if vnl_complex.h
//     has been included before it.
//
//     Of course, files *outside* vnl should use <vcl/vcl_complex.h> if they want.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 11 Feb 98
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_compiler.h>

#define VNL_COMPLEX_AVAILABLE
#ifdef VNL_USED_COMPLEX
# if defined(__GNUC__)
#  warning "please include vnl/vnl_complex.h earlier"
# elif defined(VCL_SGI_CC)
   int;/*please include vnl/vnl_complex.h earlier*/
# elif defined(VCL_SUNPRO_CC)
#  error "please include vnl/vnl_complex.h earlier"
# else
#  pragma piglet "please include vnl/vnl_complex.h earlier"
# endif
#endif

// Get vcl_complex. The reason for the #defines is to make 
// sure that every use of complex in vnl goes through this 
// header. Thus, only this file should refer to vcl_complex.
#include <vcl/vcl_complex.h>
#define vnl_complex vcl_complex
#define vnl_float_complex vcl_float_complex
#define vnl_double_complex vcl_double_complex

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_complex.
