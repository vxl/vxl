#ifndef vcl_iso_iostream_h_
#define vcl_iso_iostream_h_

#include <iostream>

// cin
#ifndef vcl_cin
#define vcl_cin std::cin
#endif
// cout
#ifndef vcl_cout
#define vcl_cout std::cout
#endif
// cerr
#ifndef vcl_cerr
#define vcl_cerr std::cerr
#endif
// clog
#ifndef vcl_clog
#define vcl_clog std::clog
#endif
// the following are not actually in the
// <iostream> synopsis:
// ios
#ifndef vcl_ios
#define vcl_ios std::ios
#endif
// ios::in
#ifndef vcl_ios_in
#define vcl_ios_in std::ios::in
#endif
// ios::out
#ifndef vcl_ios_out
#define vcl_ios_out std::ios::out
#endif
// ios::ate
#ifndef vcl_ios_ate
#define vcl_ios_ate std::ios::ate
#endif
// ios::app
#ifndef vcl_ios_app
#define vcl_ios_app std::ios::app
#endif
// ios::trunc
#ifndef vcl_ios_trunc
#define vcl_ios_trunc std::ios::trunc
#endif
// ios::binary
#ifndef vcl_ios_binary
#define vcl_ios_binary std::ios::binary
#endif
// hex
#ifndef vcl_hex
#define vcl_hex std::hex
#endif
// dec
#ifndef vcl_dec
#define vcl_dec std::dec
#endif
// ws
#ifndef vcl_ws
#define vcl_ws std::ws
#endif
// setprecision
#ifndef vcl_setprecision
#define vcl_setprecision std::setprecision
#endif
// endl
#ifndef vcl_endl
#define vcl_endl std::endl
#endif
// flush
#ifndef vcl_flush
#define vcl_flush std::flush
#endif
// streampos
#ifndef vcl_streampos
#define vcl_streampos std::streampos
#endif
// streambuf
#ifndef vcl_streambuf
#define vcl_streambuf std::streambuf
#endif
// streamsize
#ifndef vcl_streamsize
#define vcl_streamsize std::streamsize
#endif

// allowed for now
using std::cin;
using std::cout;
using std::cerr;
using std::clog;
using std::endl;
using std::flush;

#endif
