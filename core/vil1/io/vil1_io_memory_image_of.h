// This is core/vil1/io/vil1_io_memory_image_of.h
#ifndef vil1_io_memory_image_of_h_
#define vil1_io_memory_image_of_h_
//:
// \file
// \brief Perform serialised binary IO for vil1_memory_image_of
// \author Ian Scott (Manchester)
// \date 23-Mar-2001

#include <vil1/vil1_memory_image_of.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vgl_my_class to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, const vil1_memory_image_of<T> & b);

//: Binary load vgl_my_class from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, vil1_memory_image_of<T> & b);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(vcl_ostream& os,const vil1_memory_image_of<T> & b);

#endif // vil1_io_memory_image_of_h_
