#ifndef vul_printf_h_
#define vul_printf_h_
#ifdef __GNUC__
#pragma interface
#endif

// This is vxl/vul/vul_printf.h

//:
// \file
// \brief Contains vul_printf function
//
//    Printf formatting is just plain handy sometimes.  This header provides
//    it for C++ programs in a standard-conforming manner.
//
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   03 Jul 97
//
// \verbatim
// Modifications
// 970703 AWF Initial version.
// PDA (Manchester) 21/03/2001: Tidied up the documentation
// \endverbatim

#include <vcl_iosfwd.h>

//: Print using printf format specifiers to a ostream
// \verbatim
// e.g.
// vul_printf(cout, "%d\n", 12);
// \endverbatim
vcl_ostream& vul_printf(vcl_ostream&, char const* fmt, ...);

#endif // vul_printf_h_
