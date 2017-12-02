// This is core/vul/vul_printf.h
#ifndef vul_printf_h_
#define vul_printf_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
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

#include <iosfwd>
#include <vcl_compiler.h>

//: Print using printf format specifiers to an ostream.
// E.g.
// \code
// vul_printf(cout, "%d\n", 12);
// \endcode
std::ostream& vul_printf(std::ostream&, char const* fmt, ...);

#endif // vul_printf_h_
