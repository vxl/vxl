// This is mul/vil2/vil2_image_view_fns.h
#ifndef vil2_image_view_fns_h_
#define vil2_image_view_fns_h_
//:
// \file
// \brief Various functions for manipulating image views
// \author Tim Cootes - Manchester

#include <vil2/vil2_image_view.h>
#include <vil/vil_rgb.h>

//: Return a 3-plane view of an RGB image
template<class T>
vil2_image_view<T> vil2_view_as_planes(const vil2_image_view<vil_rgb<T> >& rgb_view);

//: Return an RGB component view of a 3-plane image
//  Aborts if plane image not in correct format (ie planestep()!=1)
template<class T>
vil2_image_view<vil_rgb<T> > vil2_view_as_rgb(const vil2_image_view<T>& plane_view);

//: Create a view which appears as the transpose of this view.
//  i.e vil2_transpose(view)(x,y,p) = view(y,x,p)
template<class T>
vil2_image_view<T> vil2_transpose(const vil2_image_view<T>& view);

//: Create a reflected view in which x -> (nx-1)-x
//  i.e vil2_reflect_x(view)(x,y,p) = view(nx-1-x,y,p)
template<class T>
vil2_image_view<T> vil2_reflect_x(const vil2_image_view<T>& view);

//: Create a reflected view in which y -> (ny-1)-y
//  i.e vil2_reflect_y(view)(x,y,p) = view(x,ny-1-y,p)
template<class T>
vil2_image_view<T> vil2_reflect_y(const vil2_image_view<T>& view);

//: Fill view with given value
template<class T>
void vil2_fill(vil2_image_view<T>& view, T value);

//: Fill data[i*step] (i=0..n-1) with given value
template<class T>
void vil2_fill_line(T* data, unsigned n, int step, T value);

//: Fill row y in view with given value
template<class T>
void vil2_fill_row(vil2_image_view<T>& view, unsigned y, T value);

//: Fill column x in view with given value
template<class T>
void vil2_fill_col(vil2_image_view<T>& view, unsigned x, T value);

//: Compute minimum and maximum values over view
template<class T>
void vil2_value_range(T& min_value, T& max_value,const vil2_image_view<T>& view);

//: How to print value in vil2_print_all(image_view)
template<class T>
void vil2_print_value(vcl_ostream& s, const T& value);

//: Explicit overload for unsigned char
void vil2_print_value(vcl_ostream& s, const unsigned char& value);

//: Explicit overload of print for rgb
void vil2_print_value(vcl_ostream& s, const vil_rgb<unsigned char>& value);

//: Explicit overload of print for rgb<float>  (truncates to int)
void vil2_print_value(vcl_ostream& s, const vil_rgb<float>& value);

//: Compute minimum and maximum values over view
void vil2_value_range(vil_rgb<unsigned char>& min_value, vil_rgb<unsigned char>& max_value,
                      const vil2_image_view<vil_rgb<unsigned char> >& view);

//: Compute minimum and maximum values over view
void vil2_value_range(vil_rgb<float>& min_value, vil_rgb<float>& max_value,
                      const vil2_image_view<vil_rgb<float> >& view);

//: print all image data to os in a grid
template<class T>
void vil2_print_all(vcl_ostream& os,const vil2_image_view<T>& view);

#endif // vil2_image_view_fns_h_
