// This is mul/vil2/vil2_image_view_fns.h
#ifndef vil2_image_view_fns_h_
#define vil2_image_view_fns_h_
//:
// \file
// \brief Various functions for manipulating image views
// \author Tim Cootes - Manchester

#include <vil2/vil2_image_view.h>
#include <vil/vil_rgb.h>


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

#endif // vil2_image_view_fns_h_
