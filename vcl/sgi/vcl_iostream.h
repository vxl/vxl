#ifndef vcl_sgi_iostream_h_
#define vcl_sgi_iostream_h_
/*
  fsm@robots.ox.ac.uk
*/

#include <iostream.h>
#define vcl_generic_iostream_STD /* */
#include "generic/vcl_iostream.h"

inline istream& operator>>(istream& s, signed char& c) 
{
  char i;
  s >> i;
  c = i;
  return s;
}

// This is not there.
#undef  vcl_streamsize
#define vcl_streamsize   unsigned

// ios::openmode is a bitmap (but possibly implemented as an enum).
// On SGI it's called ios::open_mode.
// The SGI compiler thinks that "enum | enum" is an int,
// while the standard says that "bitmap | bitmap" is a bitmap.
// So we don't use ios::open_mode but define or own:
#undef  vcl_ios_openmode
#define vcl_ios_openmode vcl_ios_openmode
struct vcl_ios_openmode
{
  ios::open_mode m;
  vcl_ios_openmode(int m_ = 0) : m(ios::open_mode(m_)) { }
  operator ios::open_mode () const { return m; }
  vcl_ios_openmode operator|(vcl_ios_openmode that) {
    return vcl_ios_openmode(int(m) | int(that.m));
  }
};
#undef  vcl_ios_in
#define vcl_ios_in     vcl_ios_openmode(ios::in)
#undef  vcl_ios_out
#define vcl_ios_out    vcl_ios_openmode(ios::out)
#undef  vcl_ios_ate
#define vcl_ios_ate    vcl_ios_openmode(ios::ate)
#undef  vcl_ios_app
#define vcl_ios_app    vcl_ios_openmode(ios::app)
#undef  vcl_ios_trunc
#define vcl_ios_trunc  vcl_ios_openmode(ios::trunc)
// SGI CC has no ios::bin, but since UNIX makes no distinction
// between binary and non-binary, 0 works just as well.
#undef  vcl_ios_binary
#define vcl_ios_binary vcl_ios_openmode(0)

#endif // vcl_sgi_iostream_h_
