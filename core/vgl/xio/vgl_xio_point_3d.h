// This is core/vgl/xio/vgl_xio_point_3d.h
#ifndef vgl_xio_point_3d_h_
#define vgl_xio_point_3d_h_
//:
// \file
// \author Gamze Tunali
// \date Dec 17, 2005

#include <vgl/vgl_point_3d.h>
#include <string>
#include <vcl_compiler.h>
#include <iosfwd>

//: XML save vgl_point_3d to stream.
template <class T>
void x_write(std::ostream &os, const vgl_point_3d<T> & v,
             std::string element_name="vgl_point_3d");

#endif // vgl_xio_point_3d_h_
