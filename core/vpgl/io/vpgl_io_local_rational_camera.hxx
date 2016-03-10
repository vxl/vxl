#ifndef vpgl_io_local_rational_camera_hxx_
#define vpgl_io_local_rational_camera_hxx_

#include "vpgl_io_local_rational_camera.h"
//:
// \file
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/io/vpgl_io_rational_camera.h>
#include <vnl/io/vnl_io_matrix_fixed.h>
#include <vpgl/io/vpgl_io_lvcs.h>

template <class T>
void vsl_b_write(vsl_b_ostream & os, vpgl_local_rational_camera<T> const& camera)
{
  if (!os) return;
  unsigned version = 1;
  vsl_b_write(os, version);
  // write rational camera parent
  vpgl_rational_camera<T> const& rat_cam = static_cast<vpgl_rational_camera<T> const& >(camera);
  vsl_b_write(os, rat_cam);
  vpgl_lvcs lvcs = camera.lvcs();
  vsl_b_write(os, lvcs);
}

//: Binary load camera from stream.
template <class T>
void vsl_b_read(vsl_b_istream & is, vpgl_local_rational_camera<T> &camera)
{
  if (!is) return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
    case 1:
    {
      // read rational camera
      vpgl_rational_camera<T> rat_cam;
      vsl_b_read(is, rat_cam);
      vpgl_lvcs lvcs;
      // read lvcs
      vsl_b_read(is, lvcs);
      vpgl_local_rational_camera<T> lrat_cam(lvcs, rat_cam);
      camera = lrat_cam;
      break;
    }
    default:
      std::cerr << "I/O ERROR: vpgl_local_rational_camera::b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< ver << '\n';
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream& os,const vpgl_local_rational_camera<T> & c)
{
  os << c << '\n';
}


#define VPGL_IO_LOCAL_RATIONAL_CAMERA_INSTANTIATE(T) \
template void vsl_b_write(vsl_b_ostream & os, vpgl_local_rational_camera<T > const& camera); \
template void vsl_b_read(vsl_b_istream & is, vpgl_local_rational_camera<T > &camera); \
template void vsl_print_summary(std::ostream& os,const vpgl_local_rational_camera<T > & b)

#endif // vpgl_io_local_rational_camera_hxx_
