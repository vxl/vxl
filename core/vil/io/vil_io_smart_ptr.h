// This is core/vil/io/vil_io_smart_ptr.h
#ifndef vil_io_smart_ptr_h
#define vil_io_smart_ptr_h
//:
// \file
// \brief Serialised binary IO functions for vil_smart_ptr<T>
// \author Tim Cootes/Ian Scott (Manchester)
//
// In order to use IO for smart pointers you will need to have
// the IO functions defined for pointers to MY_CLASS (class T.)
// If you have written I/O for polymorphic classes, some of these
// functions may already be defined.

#include <vsl/vsl_binary_io.h>
#include <vil/vil_smart_ptr.h>

//: Binary save vil_smart_ptr to stream.
// \relatesalso vil_smart_ptr
template <class T>
void vsl_b_write(vsl_b_ostream & os, const vil_smart_ptr<T> & v);

//: Binary load vil_smart_ptr from stream.
// \relatesalso vil_smart_ptr
template <class T>
void vsl_b_read(vsl_b_istream & is, vil_smart_ptr<T> & v);

//: Print human readable summary of object to a stream
// \relatesalso vil_smart_ptr
template <class T>
void vsl_print_summary(std::ostream & os,const vil_smart_ptr<T> & b);

#endif // vil_io_smart_ptr_h
