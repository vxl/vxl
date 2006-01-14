// This is core/vgl/xio/vgl_xio_vector_3d.txx
#ifndef vgl_xio_vector_3d_txx_
#define vgl_xio_vector_3d_txx_
//:
// \file

#include "vgl_xio_vector_3d.h"
#include <vgl/vgl_vector_3d.h>
#include <vsl/vsl_basic_xml_element.h>

//============================================================================
//: XML save self to stream.
template<class T>
void x_write(vcl_ostream &os, const vgl_vector_3d<T> & v, vcl_string element_name)
{   
   vsl_basic_xml_element xml_element(element_name);
   xml_element.add_attribute("x", v.x());
   xml_element.add_attribute("y", v.y());
   xml_element.add_attribute("z", v.z());
   xml_element.x_write(os);  
     
}

#define VGL_XIO_VECTOR_3D_INSTANTIATE(T) \
template void x_write(vcl_ostream &, const vgl_vector_3d<T > &, vcl_string) 

#endif // vgl_xio_vector_3d_txx_
