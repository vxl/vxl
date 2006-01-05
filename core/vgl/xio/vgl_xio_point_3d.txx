// This is core/vgl/io/vgl_io_point_3d.txx
#ifndef vgl_xio_point_3d_txx_
#define vgl_xio_point_3d_txx_
//:
// \file

#include "vgl_xio_point_3d.h"
#include <vgl/vgl_point_3d.h>
#include <bxml/bxml_basic_element.h>

//============================================================================
//: XML write self to stream.
template<class T>
void x_write(vcl_ostream &os, const vgl_point_3d<T> & p, vcl_string element_name)
{  
    bxml_basic_element xml_element(element_name);
    xml_element.add_attribute("x", p.x());
    xml_element.add_attribute("y", p.y());
    xml_element.add_attribute("z", p.z());
    xml_element.x_write(os);
}

#define VGL_XIO_POINT_3D_INSTANTIATE(T) \
template void x_write(vcl_ostream &, const vgl_point_3d<T> &, vcl_string)

#endif // vgl_xio_point_3d_txx_
