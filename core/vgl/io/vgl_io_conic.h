// This is core/vgl/io/vgl_io_conic.h
#ifndef vgl_io_conic_h
#define vgl_io_conic_h
//:
// \file
// \author Peter Vanroose
// \date 21-Sep-2001

#include <vgl/vgl_conic.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vgl_conic to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, vgl_conic<T> const& p);

//: Binary load vgl_conic from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, vgl_conic<T> & p);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream& os, vgl_conic<T> const& p);

#define VGL_IO_CONIC_INSTANTIATE(T) extern "Please #include <vgl/io/vgl_io_conic.hxx>"

#endif // vgl_io_conic_h
