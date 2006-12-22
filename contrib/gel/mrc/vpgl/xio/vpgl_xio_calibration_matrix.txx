// This is gel/mrc/vpgl/xio/vpgl_xio_calibration_matrix.txx
#ifndef vpgl_xio_calibration_matrix_txx_
#define vpgl_xio_calibration_matrix_txx_
//:
// \file

#include "vpgl_xio_calibration_matrix.h"
#include <vpgl/vpgl_calibration_matrix.h>
#include <vgl/xio/vgl_xio_point_2d.h>
#include <vsl/vsl_basic_xml_element.h>

//: xml write
template <class T>
void x_write(vcl_ostream &os, vpgl_calibration_matrix<T> p)
{
  vsl_basic_xml_element element("vpgl_calibration_matrix");
  element.add_attribute("focal_length", (T) p.focal_length());
  element.add_attribute("x_scale", p.x_scale());
  element.add_attribute("y_scale", p.y_scale());
  element.add_attribute("skew", p.skew());
  element.x_write_open(os);
  vgl_point_2d<T> point = p.principal_point();
  x_write(os, point, "principal_point");
  element.x_write_close(os);
}

#define VPGL_XIO_CALIBRATION_MATRIX_INSTANTIATE(T) \
template void x_write(vcl_ostream &, vpgl_calibration_matrix<T >)

#endif // vpgl_xio_calibration_matrix_txx_
