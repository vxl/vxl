#ifndef vpgl_io_perspective_camera_hxx_
#define vpgl_io_perspective_camera_hxx_

#include "vpgl_io_perspective_camera.h"
//:
// \file
#include <vpgl/vpgl_perspective_camera.h>
#include <vnl/io/vnl_io_matrix_fixed.h>
#include <vnl/io/vnl_io_vector_fixed.h>
#include <vgl/io/vgl_io_point_3d.h>

template <class T>
void vsl_b_write(vsl_b_ostream & os, vpgl_perspective_camera<T> const& camera)
{
  if (!os) return;
  unsigned version = 1;
  vsl_b_write(os, version);
  vnl_matrix_fixed<T,3,3> K = camera.get_calibration().get_matrix();
  vgl_rotation_3d<T> R = camera.get_rotation();
  vnl_vector_fixed<T,3> rod = R.as_rodrigues();
  vgl_point_3d<T> center = camera.get_camera_center();
  vsl_b_write(os, K);
  vsl_b_write(os, rod);
  vsl_b_write(os, center);
}

//: Binary load camera from stream.
template <class T>
void vsl_b_read(vsl_b_istream & is, vpgl_perspective_camera<T> &camera)
{
  if (!is) return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
   {
     vnl_matrix_fixed<T,3,3> K;
     vnl_vector_fixed<T,3> rod;
     vgl_point_3d<T> center;
     vsl_b_read(is, K);
     vsl_b_read(is, rod);
     vgl_rotation_3d<T> R(rod);
     vsl_b_read(is, center);
     camera.set_calibration(K);
     camera.set_camera_center(center);
     camera.set_rotation(R);
     break;
   }
   default:
    std::cerr << "I/O ERROR: vpgl_perspective_camera::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream& os,const vpgl_perspective_camera<T> & c)
{
  os << c << '\n';
}


#define VPGL_IO_PERSPECTIVE_CAMERA_INSTANTIATE(T) \
template void vsl_b_write(vsl_b_ostream & os, vpgl_perspective_camera<T > const& camera); \
template void vsl_b_read(vsl_b_istream & is, vpgl_perspective_camera<T > &camera); \
template void vsl_print_summary(std::ostream& os,const vpgl_perspective_camera<T > & b)

#endif // vpgl_io_perspective_camera_hxx_
