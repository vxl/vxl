// This is core/vgl/xio/vgl_xio_point_3d.h
#ifndef vgl_xio_point_3d_h_
#define vgl_xio_point_3d_h_
//:
// \file
// \author Gamze Tunali
// \date 12/17/2005

#include <vgl/vgl_point_3d.h>
#include <vcl_string.h>

//: Binary save vgl_point_3d to stream.
template <class T>
void x_write(vcl_ostream &os, const vgl_point_3d<T> & v, vcl_string element_name);

#endif // vgl_xio_point_3d_h
