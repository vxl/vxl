// This is core/vgl/io/vgl_io_pointset_3d.h
#ifndef vgl_io_pointset_3d_h
#define vgl_io_pointset_3d_h

//:
// \file
// \author Noah Johnson
// \date 03-Nov-2020

#include <iostream>

#include <vgl/io/vgl_io_point_3d.h>
#include <vgl/io/vgl_io_vector_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>


//: Binary save vgl_pointset_3d to stream.
template <class T>
void
vsl_b_write(vsl_b_ostream & os, const vgl_pointset_3d<T> & v);

//: Binary load vgl_pointset_3d from stream.
template <class T>
void
vsl_b_read(vsl_b_istream & is, vgl_pointset_3d<T> & v);

//: Print human readable summary of object to a stream
template <class T>
void
vsl_print_summary(std::ostream & os, const vgl_pointset_3d<T> & b);

#endif // vgl_io_pointset_3d_h
