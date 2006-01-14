// This is core/vgl/xio/vgl_xio_vector_3d.h
#ifndef vgl_xio_vector_3d_h
#define vgl_xio_vector_3d_h
//:
// \file
// \author Gamze Tunali
// \date   12/19/2005

#include <vgl/vgl_vector_3d.h>
#include <vcl_string.h>
#include <vcl_iosfwd.h>

//: Binary save vgl_vector_3d to stream.
template <class T>
void x_write(vcl_ostream &os, const vgl_vector_3d<T> & v, vcl_string element_name);

#endif // vgl_xio_vector_3d_h
