// This is mul/vil2/vil2_image_view_fns.h
#ifndef vil2_image_view_fns_h_
#define vil2_image_view_fns_h_
//:
// \file
// \brief Various functions for manipulating image views
// \author Tim Cootes - Manchester

#include <vil2/vil2_image_view.h>
#include <vil/vil_rgb.h>


//: True if the actual images are identical.
// $\bigwedge_{i,j,p} {\textstyle src}(i,j,p) == {\textstyle dest}(i,j,p)$
// The data may be formatted differently in each memory chunk.
//  O(size).
// \relates vil2_image_view
template<class T>
bool vil2_deep_equality(const vil2_image_view<T> &lhs, const vil2_image_view<T> &rhs);

//: Return a 3-plane view of an RGB image
//  O(1).
// \relates vil2_image_view
template<class T>
vil2_image_view<T> vil2_view_as_planes(const vil2_image_view<vil_rgb<T> >& rgb_view);

//: Return an RGB component view of a 3-plane image.
//  Aborts if plane image not in correct format (ie planestep()!=1)
//  O(1).
// \relates vil2_image_view
template<class T>
vil2_image_view<vil_rgb<T> > vil2_view_as_rgb(const vil2_image_view<T>& plane_view);

//: Fill view with given value
//  O(size).
// \relates vil2_image_view
template<class T>
void vil2_fill(vil2_image_view<T>& view, T value);

//: Fill data[i*step] (i=0..n-1) with given value
// \relates vil2_image_view
template<class T>
void vil2_fill_line(T* data, unsigned n, int step, T value);

//: Fill row j in view with given value
//  O(ni).
// \relates vil2_image_view
template<class T>
void vil2_fill_row(vil2_image_view<T>& view, unsigned j, T value);

//: Fill column i in view with given value
//  O(nj).
// \relates vil2_image_view
template<class T>
void vil2_fill_col(vil2_image_view<T>& view, unsigned i, T value);

//: Compute minimum and maximum values over view
//  O(n).
// \relates vil2_image_view
template<class T>
void vil2_value_range(T& min_value, T& max_value,const vil2_image_view<T>& view);

#endif // vil2_image_view_fns_h_
