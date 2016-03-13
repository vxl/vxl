#ifndef vcl_ios_h_
#define vcl_ios_h_

#include "vcl_compiler.h"
#include <ios>
//vcl alias names to std names
// the std::ios::(in|out|etc) flags should
// live in this header file (27.4.2).
#define vcl_ios std::ios
#define vcl_streamoff std::streamoff
#define vcl_streamsize std::streamsize
#define vcl_fpos std::fpos
#define vcl_basic_ios std::basic_ios
#define vcl_ios_base std::ios_base
#define vcl_ios_basefield std::ios::basefield
#define vcl_ios_floatfield std::ios::floatfield
#define vcl_ios_adjustfield std::ios::adjustfield
#define vcl_ios_boolalpha std::ios::boolalpha
#define vcl_ios_noboolalpha std::ios::noboolalpha
#define vcl_ios_showbase std::ios::showbase
#define vcl_ios_noshowbase std::ios::noshowbase
#define vcl_ios_showpoint std::ios::showpoint
#define vcl_ios_noshowpoint std::ios::noshowpoint
#define vcl_ios_showpos std::ios::showpos
#define vcl_ios_noshowpos std::ios::noshowpos
#define vcl_ios_skipws std::ios::skipws
#define vcl_ios_noskipws std::ios::noskipws
#define vcl_ios_uppercase std::ios::uppercase
#define vcl_ios_nouppercase std::ios::nouppercase
#define vcl_ios_internal std::ios::internal
#define vcl_ios_left std::ios::left
#define vcl_ios_right std::ios::right
#define vcl_ios_dec std::ios::dec
#define vcl_ios_hex std::ios::hex
#define vcl_ios_oct std::ios::oct
#define vcl_ios_fixed std::ios::fixed
#define vcl_ios_scientific std::ios::scientific
#define vcl_ios_fmtflags std::ios::fmtflags
#define vcl_ios_seekdir std::ios::seekdir
#define vcl_ios_beg std::ios::beg
#define vcl_ios_cur std::ios::cur
#define vcl_ios_end std::ios::end

#endif // vcl_ios_h_
