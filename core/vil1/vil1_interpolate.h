// This is core/vil1/vil1_interpolate.h
#ifndef vil1_interpolate_h_
#define vil1_interpolate_h_
//:
// \file
// \brief Interpolation methods
// \author awf@robots.ox.ac.uk
// \date 02 Apr 00

#include <vil1/vil1_memory_image_of.h>

//: Return nearest neighbour
template <class T, class U>
bool vil1_interpolate_nearest(vil1_memory_image_of<T> const& img, double src_x, double src_y, U* out);
// fsm: changed signature to avoid having two instantiation
// macros. [inst(float,float) conflicted with inst(float,double)].

//: Compute bilinearly interpolated value from img(src_x, src_y), and place in *U
//This form allows one to choose the return type, while the older one forced it to be
//numeric_traits<T>::real_t
template <class T, class U>
bool vil1_interpolate_bilinear(vil1_memory_image_of<T> const& img, double src_x, double src_y, U* out);

//: Compute bilinearly interpolated value, and image gradient Ix, Iy at img(src_x, src_y).
template <class T, class U>
bool vil1_interpolate_bilinear_grad(vil1_memory_image_of<T> const& img, double src_x, double src_y,
                                    U* out_i, U* out_dx, U* out_dy);

//: Compute bicubically interpolated value at img(src_x, src_y)
template <class T, class U>
bool vil1_interpolate_bicubic(vil1_memory_image_of<T> const& img, double src_x, double src_y, U* out);

#endif // vil1_interpolate_h_
