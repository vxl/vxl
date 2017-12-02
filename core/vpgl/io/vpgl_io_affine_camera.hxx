#ifndef vpgl_io_affine_camera_hxx_
#define vpgl_io_affine_camera_hxx_
//:
// \file
#include "vpgl_io_affine_camera.h"
//
#include <vpgl/vpgl_affine_camera.h>
#include <vnl/io/vnl_io_matrix_fixed.h>

template <class T>
void vsl_b_write(vsl_b_ostream & os, vpgl_affine_camera<T> const& camera)
{
  if (!os) return;
  unsigned version = 1;
  vsl_b_write(os, version);
  vsl_b_write(os, camera.get_matrix());
  vsl_b_write(os, camera.viewing_distance());
}

//: Binary load camera from stream.
template <class T>
void vsl_b_read(vsl_b_istream & is, vpgl_affine_camera<T> &camera)
{
  if (!is) return;
  short ver;
  vsl_b_read(is, ver);
  vnl_matrix_fixed<T,3,4> Pnew;
  switch (ver)
  {
   case 1:
     vsl_b_read(is, Pnew);
     camera.set_matrix(Pnew);
     T view_dist;
     vsl_b_read(is, view_dist);
     camera.set_viewing_distance(view_dist);
    break;
   default:
    std::cerr << "I/O ERROR: vpgl_affine_camera::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream& os,const vpgl_affine_camera<T> & c)
{
  os << c << '\n';
}


#define VPGL_IO_AFFINE_CAMERA_INSTANTIATE(T) \
template void vsl_b_write(vsl_b_ostream & os, vpgl_affine_camera<T > const& camera); \
template void vsl_b_read(vsl_b_istream & is, vpgl_affine_camera<T > &camera); \
template void vsl_print_summary(std::ostream& os,const vpgl_affine_camera<T > & b)

#endif // vpgl_io_affine_camera_hxx_
