// This is vxl/vil2/io/vil2_io_smart_ptr.h
#ifndef vil2_io_smart_ptr_h
#define vil2_io_smart_ptr_h
//:
// \file
// \brief Serialised binary IO functions for vil2_smart_ptr<T>
// \author Tim Cootes/Ian Scott (Manchester)
//
// In order to use IO for smart pointers you will need to have
// the IO functions defined for pointers to MY_CLASS (class T.)
// If you have written io for polymorphic classes, some of these
// functions may already be defined.

#include <vsl/vsl_binary_io.h>
#include <vil2/vil2_smart_ptr.h>

//: Binary save vil2_smart_ptr to stream.
template <class T>
void vsl_b_write(vsl_b_ostream & os, const vil2_smart_ptr<T> & v);

//: Binary load vil2_sparse_matrix from stream.
template <class T>
void vsl_b_read(vsl_b_istream & is, vil2_smart_ptr<T> & v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(vcl_ostream & os,const vil2_smart_ptr<T> & b);

#endif // vil2_io_smart_ptr_h
