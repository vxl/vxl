#ifndef vil_rgb_byte_h_
#define vil_rgb_byte_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_rgb_byte
// .INCLUDE vil/vil_rgb_byte.h
// .FILE vil_rgb_byte.cxx
// .SECTION Author
//    awf@robots.ox.ac.uk
// Created: 17 Feb 00
//
// .SECTION Modifications
//     000217 AWF Initial version.

#ifndef vil_rgb_byte_dont_warn
// this message may be a bit too loud...
#ifdef __GNUC__
# warning "*******************************************************************"
# warning "*                                                                 *"
# warning "* vil_rgb_byte is deprecated. Use vil_rgb<unsigned char> instead. *"
# warning "*                                                                 *"
# warning "*******************************************************************"
#endif
#endif

#include <vil/vil_byte.h>
#include <vil/vil_rgb.h>

typedef vil_rgb<vil_byte> vil_rgb_byte;

#endif // vil_rgb_byte_h_
