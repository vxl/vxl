#ifndef vcl_iso_ios_h_
#define vcl_iso_ios_h_

#include <ios>

// 27.4
// ios
#ifndef vcl_ios
#define vcl_ios std::ios
#endif
// streamoff
#ifndef vcl_streamoff
#define vcl_streamoff std::streamoff
#endif
// streamsize
#ifndef vcl_streamsize
#define vcl_streamsize std::streamsize
#endif
// fpos
#ifndef vcl_fpos
#define vcl_fpos std::fpos
#endif
// basic_ios
#ifndef vcl_basic_ios
#define vcl_basic_ios std::basic_ios
#endif
// boolalpha
#ifndef vcl_boolalpha
#define vcl_boolalpha std::boolalpha
#endif
// noboolalpha
#ifndef vcl_noboolalpha
#define vcl_noboolalpha std::noboolalpha
#endif
// showbase
#ifndef vcl_showbase
#define vcl_showbase std::showbase
#endif
// noshowbase
#ifndef vcl_noshowbase
#define vcl_noshowbase std::noshowbase
#endif
// showpoint
#ifndef vcl_showpoint
#define vcl_showpoint std::showpoint
#endif
// noshowpoint
#ifndef vcl_noshowpoint
#define vcl_noshowpoint std::noshowpoint
#endif
// showpos
#ifndef vcl_showpos
#define vcl_showpos std::showpos
#endif
// noshowpos
#ifndef vcl_noshowpos
#define vcl_noshowpos std::noshowpos
#endif
// skipws
#ifndef vcl_skipws
#define vcl_skipws std::skipws
#endif
// noskipws
#ifndef vcl_noskipws
#define vcl_noskipws std::noskipws
#endif
// uppercase
#ifndef vcl_uppercase
#define vcl_uppercase std::uppercase
#endif
// nouppercase
#ifndef vcl_nouppercase
#define vcl_nouppercase std::nouppercase
#endif
// internal
#ifndef vcl_internal
#define vcl_internal std::internal
#endif
// left
#ifndef vcl_left
#define vcl_left std::left
#endif
// right
#ifndef vcl_right
#define vcl_right std::right
#endif
// dec
#ifndef vcl_dec
#define vcl_dec std::dec
#endif
// hex
#ifndef vcl_hex
#define vcl_hex std::hex
#endif
// oct
#ifndef vcl_oct
#define vcl_oct std::oct
#endif
// fixed
#ifndef vcl_fixed
#define vcl_fixed std::fixed
#endif
// scientific
#ifndef vcl_scientific
#define vcl_scientific std::scientific
#endif

// the std::ios::(in|out|etc) flags should
// live in this header file (27.4.2).

#endif
