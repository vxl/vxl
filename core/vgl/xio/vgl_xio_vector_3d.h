// This is core/vgl/xio/vgl_xio_vector_3d.h
#ifndef vgl_xio_vector_3d_h
#define vgl_xio_vector_3d_h
//:
// \file
// \author Gamze Tunali
// \date   Dec 19, 2005

#include <string>
#include <iosfwd>
#include <vgl/vgl_vector_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: XML save vgl_vector_3d to stream.
template <class T>
void x_write(std::ostream &os, const vgl_vector_3d<T> & v,
             std::string element_name="vgl_vector_3d");

#endif // vgl_xio_vector_3d_h
