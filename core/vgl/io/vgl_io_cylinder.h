// This is core/vgl/io/vgl_io_cylinder.h
#ifndef vgl_io_cylinder_h
#define vgl_io_cylinder_h
//:
// \file
// \author Gamze D. Tunali (gamze@lems.brown.edu)
// \date   11 July 2005

#include <vgl/vgl_cylinder.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vgl_cylinder to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_cylinder<T> & cyl);

//: Binary load vgl_cylinder from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, vgl_cylinder<T> & cyl);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream& os, const vgl_cylinder<T> & cyl);

#define VGL_IO_CYLINDER_INSTANTIATE(T) extern "Please #include <vgl/io/vgl_io_cylinder.hxx>"

#endif // vgl_io_cylinder_h
