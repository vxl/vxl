// This is mul/vil2/io/vil2_io_image_view.h
#ifndef vil2_io_image_view_h_
#define vil2_io_image_view_h_
//:
// \file
// \author Tim Cootes

#include <vil2/vil2_image_view.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vil2_image_view to stream.
template<class T>
void vsl_b_write(vsl_b_ostream &os, const vil2_image_view<T>& image);

//: Binary load vil2_image_view from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vil2_image_view<T>& image);

//: Binary load vil2_image_view from stream  onto the heap
template<class T>
void vsl_b_read(vsl_b_istream &is, vil2_image_view<T>*& image);

//: Print human readable summary of a vil2_image_view object to a stream
template<class T>
void vsl_print_summary(vcl_ostream& os,const vil2_image_view<T>& image);

#endif
