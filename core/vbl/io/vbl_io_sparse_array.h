#ifndef vbl_io_sparse_array_h
#define vbl_io_sparse_array_h
#ifdef __GNUC__
#pragma interface
#endif

// This is vxl/vbl/io/vbl_io_sparse_array.h

//:
// \file
// \author Christine Beeston
// \date 23-Mar-2001

#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_sparse_array.h>

//: Binary save vbl_sparse_array to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, const vbl_sparse_array<T> & v);

//: Binary load vbl_sparse_array from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, vbl_sparse_array<T> & v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(vcl_ostream& os,const vbl_sparse_array<T> & b);


#endif // vbl_io_sparse_array_h
