#ifndef vbl_printf_h_
#define vbl_printf_h_
#ifdef __GNUC__
#pragma interface
#endif

// .NAME	vbl_printf - vbl_printf to ostream
// .LIBRARY	vbl
// .HEADER	vxl package
// .INCLUDE	vbl/vbl_printf.h
// .FILE	vbl_printf.cxx
//
// .SECTION Description
//    Printf formatting is just plain handy sometimes.  This header provides
//    it for C++ programs in a standard-conforming manner.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 03 Jul 97
//
// .SECTION Modifications
//    970703 AWF Initial version.
//
//-----------------------------------------------------------------------------

//#include <vcl_cstdio.h> // just to ensure no clashes with weirdo stdios
#include <vcl_iosfwd.h>

// -- vbl_printf(cout, "%d\n", 12);
vcl_ostream& vbl_printf(vcl_ostream&, char const* fmt, ...);

#endif // vbl_printf_h_
