// This is mul/vil2/vil2_image_view_fns.h
#ifndef vil2_image_view_fns_h_
#define vil2_image_view_fns_h_
//:
// \file
// \brief Various functions for manipulating image views
// \author Tim Cootes - Manchester

#include <vil2/vil2_image_view.h>
#include <vil/vil_rgb.h>


//: Return an ni x nj window of im with offset (x0,y0)
// \relates vil2_image_view
template<class T>
vil2_image_view<T> vil2_window(const vil2_image_view<T> &im, unsigned i0, unsigned ni, unsigned j0, unsigned nj);

//: Return a view of im's plane p 
// \relates vil2_image_view
template<class T>
vil2_image_view<T> vil2_plane(const vil2_image_view<T> &im, unsigned p);

//: Copy src to dest, without changing dest's view parameters.
// This is useful if you want to copy on image into a window on another image.
// src and dest must have identical sizes, and types.
// \relates vil2_image_view
template<class T>
void vil2_reformat_copy(const vil2_image_view<T> &src, vil2_image_view<T> &dest);

//: True if the actual images are identical.
// $\bigwedge_{i,j,p} {\textstyle src}(i,j,p) == {\textstyle dest}(i,j,p)$
// The data may be formatted differently in each memory chunk.
// \relates vil2_image_view
template<class T>
bool vil2_deep_equality(const vil2_image_view<T> &lhs, const vil2_image_view<T> &rhs);

//: Create a copy of an image, with completly new underlying memory.
// \relates vil2_image_view
template<class T>
vil2_image_view<T> vil2_deep_copy(const vil2_image_view<T> &rhs);

//: Return a 3-plane view of an RGB image
// \relates vil2_image_view
template<class T>
vil2_image_view<T> vil2_view_as_planes(const vil2_image_view<vil_rgb<T> >& rgb_view);

//: Return an RGB component view of a 3-plane image.
//  Aborts if plane image not in correct format (ie planestep()!=1)
// \relates vil2_image_view
template<class T>
vil2_image_view<vil_rgb<T> > vil2_view_as_rgb(const vil2_image_view<T>& plane_view);

//: Create a view which appears as the transpose of this view.
//  i.e vil2_transpose(view)(i,j,p) = view(j,i,p)
// \relates vil2_image_view
template<class T>
vil2_image_view<T> vil2_transpose(const vil2_image_view<T>& view);

//: Create a reflected view in which i -> ni-1-i.
//  i.e vil2_flip_lr(view)(i,j,p) = view(ni-1-i,j,p)
// \relates vil2_image_view
template<class T>
vil2_image_view<T> vil2_flip_lr(const vil2_image_view<T>& view);

//: Create a reflected view in which y -> nj-1-j.
//  i.e vil2_flip_ud(view)(i,j,p) = view(i,nj-1-j,p)
// \relates vil2_image_view
template<class T>
vil2_image_view<T> vil2_flip_ud(const vil2_image_view<T>& view);

//: Fill view with given value
// \relates vil2_image_view
template<class T>
void vil2_fill(vil2_image_view<T>& view, T value);

//: Fill data[i*step] (i=0..n-1) with given value
// \relates vil2_image_view
template<class T>
void vil2_fill_line(T* data, unsigned n, int step, T value);

//: Fill row j in view with given value
// \relates vil2_image_view
template<class T>
void vil2_fill_row(vil2_image_view<T>& view, unsigned j, T value);

//: Fill column i in view with given value
// \relates vil2_image_view
template<class T>
void vil2_fill_col(vil2_image_view<T>& view, unsigned i, T value);

//: Compute minimum and maximum values over view
// \relates vil2_image_view
template<class T>
void vil2_value_range(T& min_value, T& max_value,const vil2_image_view<T>& view);

//: How to print value in vil2_print_all(image_view)
// \relates vil2_image_view
template<class T>
void vil2_print_value(vcl_ostream& s, const T& value);

//: print all image data to os in a grid
// \relates vil2_image_view
template<class T>
void vil2_print_all(vcl_ostream& os,const vil2_image_view<T>& view);

#endif // vil2_image_view_fns_h_
