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

#include <vil/vil_byte.h>

//: A simple RGB structure
struct vil_rgb_byte {
  vil_byte r;
  vil_byte g;
  vil_byte b;

  vil_rgb_byte(int ir, int ig, int ib) : r(ir), g(ig), b(ib) {}

  // -- Convert vbl_rgb to gray using standard (.299, .587, .114) weighting.
  vil_byte grey() const { return int(0.5+r*0.299+0.587*g+0.114*b); }  
};

#endif // vil_rgb_byte_h_
