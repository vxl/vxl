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

//: How to print value in vil2_print_all(image_view)
template<class T>
void vil2_print_value(vcl_ostream& s, const T& value);

//: Explicit overload of print for rgb
void vil2_print_value(vcl_ostream& s, const vil_rgb<unsigned char>& value);

//: Explicit overload of print for rgb<float>  (truncates to int)
void vil2_print_value(vcl_ostream& s, const vil_rgb<float>& value);

//: Convenience function for loading an image into an image view.
vil2_image_view_base * vil2_load_into_memory(const char *);

//: print all image data to os in a grid
template<class T>
void vil2_print_all(vcl_ostream& os,const vil2_image_view<T>& view);

#endif // vil2_image_view_fns_h_
