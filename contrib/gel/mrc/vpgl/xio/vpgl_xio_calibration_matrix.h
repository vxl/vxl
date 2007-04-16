// This is gel/mrc/vpgl/xio/vpgl_xio_calibration_matrix.h
#ifndef vpgl_xio_calibration_matrix_h_
#define vpgl_xio_calibration_matrix_h_
//:
// \file
// \author Gamze Tunali
// \date 10/31/2006

#include <vpgl/vpgl_calibration_matrix.h>
// not used? #include <vcl_string.h>
#include <vcl_iosfwd.h>

//: XML write
template <class T>
void x_write(vcl_ostream &os, vpgl_calibration_matrix<T> p);

#endif // vpgl_xio_calibration_matrix_h_
