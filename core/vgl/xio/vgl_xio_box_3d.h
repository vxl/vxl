// This is core/vgl/xio/vgl_xio_box_3d.h
#ifndef vgl_xio_box_3d_h_
#define vgl_xio_box_3d_h_
// :
// \file
// \author Gamze Tunali
// \date Dec 19, 2005

#include <vgl/vgl_box_3d.h>
#include <vcl_string.h>
#include <vcl_iosfwd.h>

// : XML save vgl_box_3d to stream.
template <class T>
void x_write(vcl_ostream & os, const vgl_box_3d<T> & v, vcl_string element_name = "vgl_box_3d");

#endif // vgl_xio_box_3d_h_
