#ifndef vil_colour_space_h_
#define vil_colour_space_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

//: Functions to convert between different colour spaces.
// See Foley and van Dam, "Computer Graphics, Principles and Practice".


//: This is a linear transformation.
template <class T>
void vil_colour_space_RGB_to_YIQ(T const in[3], T out[3]);


//:
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

#endif
