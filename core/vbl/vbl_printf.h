//-*- c++ -*-------------------------------------------------------------------
#ifndef vbl_printf_h_
#define vbl_printf_h_
#ifdef __GNUC__
#pragma interface
#endif

// .NAME        vbl_printf - vbl_printf to ostream
// .LIBRARY     vbl
// .HEADER	vxl package
// .INCLUDE     vbl/vbl_printf.h
// .FILE        vbl/vbl_printf.cxx
//
// .SECTION Description
//    Printf formatting is just plain handy sometimes.  This header provides
//    it for C++ programs in a standard-conforming manner.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 03 Jul 97
//
//-----------------------------------------------------------------------------

//#include <vcl/vcl_cstdio.h> // just to ensure no clashes with weirdo stdios
#include <vcl/vcl_iosfwd.h>

// -- vbl_printf(cout, "%d\n", 12);
vcl_ostream& vbl_printf(vcl_ostream&, char const* fmt, ...);

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vbl_printf.
