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
//     001206 FSM make this identical to vil_rgb<vil_byte> to avoid confusion

#include <vil/vil_byte.h>
#include <vil/vil_rgb.h>

typedef vil_rgb<vil_byte> vil_rgb_byte;

#endif // vil_rgb_byte_h_
