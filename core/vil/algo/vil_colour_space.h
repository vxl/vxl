// This is core/vil/algo/vil_colour_space.h
#ifndef vil_colour_space_h_
#define vil_colour_space_h_
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

//: Linear transformation from YIQ to RGB colour spaces
template <class T>
void vil_colour_space_YIQ_to_RGB(T const in[3], T out[3]);


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


//: Linear transformation from RGB to YUV colour spaces
template <class T>
void vil_colour_space_RGB_to_YUV(T const in[3], T out[3]);

//: Linear transformation from YUV to RGB colour spaces
template <class T>
void vil_colour_space_YUV_to_RGB(T const in[3], T out[3]);

//: Transformation from analog RGB to analog YPbPr colour spaces
//  The input and out value lie in the ranges:
//  - R', G', B' in [0; 1]
//  - Y' in [0; 1]
//  - Pb in [-0.5; 0.5]
//  - Pr in [-0.5; 0.5]
template <class T>
void vil_colour_space_RGB_to_YPbPr_601(T const RGB[3], T YPbPr[3]);

//: Transformation from analog YPbPr to RGB colour spaces
//  The input and out value lie in the ranges:
//  - Y' in [0; 1]
//  - Pb in [-0.5; 0.5]
//  - Pr in [-0.5; 0.5]
//  - R', G', B' in [0; 1]
template <class T>
void vil_colour_space_YPbPr_601_to_RGB(T const YPbPr[3], T RGB[3]);

//: Transformation from 8 bit RGB to 8 bit YCbCr colour spaces
//  The input and out value lie in the ranges:
//  - R'd, G'd, B'd   in {0, 1, 2, ..., 255}
//  - Y'              in {16, 17, ..., 235}
//    with
//         * footroom in {1, 2, ..., 15}
//         * headroom in {236, 237, ..., 254}
//         * sync.    in {0, 255}
//  - Cb, Cr          in {16, 17, ..., 240}
void vil_colour_space_RGB_to_YCbCr_601(const unsigned char RGB[3], unsigned char YCbCr[3]);

//: Transformation from 8 bit YCbCr to 8 bit RGB colour spaces
//  The input and out value lie in the ranges:
//  - Y'              in {16, 17, ..., 235}
//    with
//         * footroom in {1, 2, ..., 15}
//         * headroom in {236, 237, ..., 254}
//         * sync.    in {0, 255}
//  - Cb, Cr          in {16, 17, ..., 240}
//  - R'd, G'd, B'd   in {0, 1, 2, ..., 255}
void vil_colour_space_YCbCr_601_to_RGB(const unsigned char YCbCr[3], unsigned char RGB[3]);

#endif // vil_colour_space_h_
