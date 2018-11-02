// This is core/vgl/xio/vgl_xio_point_2d.h
#ifndef vgl_xio_point_2d_h
#define vgl_xio_point_2d_h
//:
// \file
// \author Gamze Tunali
// \date 25-Dec-2005

#include <string>
#include <iosfwd>
#include <vgl/vgl_point_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: XML save vgl_point_2d to stream.
template <class T>
void x_write(std::ostream &os, const vgl_point_2d<T> & v,
             std::string name="vgl_point_2d");

#endif // vgl_xio_point_2d_h
