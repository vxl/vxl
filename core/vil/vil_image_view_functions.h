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

//: Return an RGB component view of a 3-plane image.
//  Aborts if plane image not in correct format (ie planestep()!=1)
template<class T>
vil2_image_view<vil_rgb<T> > vil2_view_as_rgb(const vil2_image_view<T>& plane_view);

//: Create a view which appears as the transpose of this view.
//  i.e vil2_transpose(view)(i,j,p) = view(j,i,p)
template<class T>
vil2_image_view<T> vil2_transpose(const vil2_image_view<T>& view);

//: Create a reflected view in which i -> ni-1-i.
//  i.e vil2_flip_lr(view)(i,j,p) = view(ni-1-i,j,p)
template<class T>
vil2_image_view<T> vil2_flip_lr(const vil2_image_view<T>& view);

//: Create a reflected view in which y -> nj-1-j.
//  i.e vil2_flip_ud(view)(i,j,p) = view(i,nj-1-j,p)
template<class T>
vil2_image_view<T> vil2_flip_ud(const vil2_image_view<T>& view);

//: Fill view with given value
template<class T>
void vil2_fill(vil2_image_view<T>& view, T value);

//: Fill data[i*step] (i=0..n-1) with given value
template<class T>
void vil2_fill_line(T* data, unsigned n, int step, T value);

//: Fill row j in view with given value
template<class T>
void vil2_fill_row(vil2_image_view<T>& view, unsigned j, T value);

//: Fill column i in view with given value
template<class T>
void vil2_fill_col(vil2_image_view<T>& view, unsigned i, T value);

//: Compute minimum and maximum values over view
template<class T>
void vil2_value_range(T& min_value, T& max_value,const vil2_image_view<T>& view);

//: How to print value in vil2_print_all(image_view)
template<class T>
void vil2_print_value(vcl_ostream& s, const T& value);

//: print all image data to os in a grid
template<class T>
void vil2_print_all(vcl_ostream& os,const vil2_image_view<T>& view);

#endif // vil2_image_view_fns_h_
