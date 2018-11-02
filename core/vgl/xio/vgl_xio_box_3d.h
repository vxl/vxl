// This is core/vgl/xio/vgl_xio_box_3d.h
#ifndef vgl_xio_box_3d_h_
#define vgl_xio_box_3d_h_
//:
// \file
// \author Gamze Tunali
// \date Dec 19, 2005

#include <string>
#include <iosfwd>
#include <vgl/vgl_box_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: XML save vgl_box_3d to stream.
template <class T>
void x_write(std::ostream &os, const vgl_box_3d<T> & v,
             std::string element_name="vgl_box_3d");


#endif // vgl_xio_box_3d_h_
