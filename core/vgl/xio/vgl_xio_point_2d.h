// This is core/vgl/xio/vgl_xio_point_2d.h
#ifndef vgl_xio_point_2d_h
#define vgl_xio_point_2d_h
//:
// \file
// \author Gamze Tunali
// \date 25-Dec-2005

#include <vgl/vgl_point_2d.h>
#include <vcl_string.h>
#include <vcl_iosfwd.h>

//: XML save vgl_point_2d to stream.
template <class T>
void x_write(vcl_ostream &os, const vgl_point_2d<T> & v, vcl_string name);

#endif // vgl_xio_point_2d_h
