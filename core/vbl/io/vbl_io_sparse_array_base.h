// This is vxl/vbl/io/vbl_io_sparse_array_base.h
#ifndef vbl_io_sparse_array_base_h
#define vbl_io_sparse_array_base_h
//:
// \file
// \author Ian Scott
// \date 11-Apr-2001

#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_sparse_array_base.h>

//: Binary save vbl_sparse_array_base to stream.
template <class T, class Index>
void vsl_b_write(vsl_b_ostream &os, const vbl_sparse_array_base<T, Index> & v);

//: Binary load vbl_sparse_array_base from stream.
template <class T, class Index>
void vsl_b_read(vsl_b_istream &is, vbl_sparse_array_base<T, Index> & v);

//: Print human readable summary of object to a stream
template <class T, class Index>
void vsl_print_summary(vcl_ostream& os,const vbl_sparse_array_base<T, Index> & b);

#endif // vbl_io_sparse_array_base_h
