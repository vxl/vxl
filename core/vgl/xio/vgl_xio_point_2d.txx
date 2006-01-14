// This is core/vgl/xio/vgl_xio_point_2d.txx
#ifndef vgl_xio_point_2d_txx_
#define vgl_xio_point_2d_txx_
//:
// \file

#include "vgl_xio_point_2d.h"
#include <vsl/vsl_basic_xml_element.h>

//============================================================================
//: Binary save self to stream.
template<class T>
void x_write(vcl_ostream &os, const vgl_point_2d<T> & v, vcl_string name)
{
  vsl_basic_xml_element element(name);
  element.add_attribute("x", v.x());
  element.add_attribute("y", v.y());
  element.x_write(os);
}

#define VGL_XIO_POINT_2D_INSTANTIATE(T) \
template void x_write(vcl_ostream &, const vgl_point_2d<T > &, vcl_string)

#endif // vgl_xio_point_2d_txx_
