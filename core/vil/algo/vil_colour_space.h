// This is core/vil/algo/vil_colour_space.h
#ifndef vil_colour_space_h_
#define vil_colour_space_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Functions to convert between different colour spaces.
//
// Functions to convert between different colour spaces.
// See Foley and van Dam, "Computer Graphics, Principles and Practice".
//
// \author fsm

//: Linear transformation from RGB to YIQ colour spaces
template <class T>
void vil_colour_space_RGB_to_YIQ(T const in[3], T out[3]);


//: Transform from RGB to HSV colour spaces
// The input RGB values must lie in [0, L], for some positive L. Usually
// L=1 or 255.
//
// The output HSV values will lie in the ranges:
//    H : [0, 360)   (an angle, in bloody degrees)
//    S : [0, 1]
//    V : [0, L]
//
// [HSV is also known as HSB]
template <class T>
void vil_colour_space_RGB_to_HSV(T r, T g, T b, T *h, T *s, T *v);

//: Transform from HSV to RGB colour space
//
// The input HSV values will lie in the ranges:
//    H : [0, 360)   (an angle, in bloody degrees)
//    S : [0, 1]
//    V : [0, 255]
//
// The output RGB values will lie in [0, 255]
//
// [HSV is also known as HSB]
template <class T>
void vil_colour_space_HSV_to_RGB(T h, T s, T v, T *r, T *g, T *b);

#endif // vil_colour_space_h_
