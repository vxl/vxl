#ifndef vpgl_io_rational_camera_hxx_
#define vpgl_io_rational_camera_hxx_

#include "vpgl_io_rational_camera.h"
//:
// \file
#include <vpgl/vpgl_rational_camera.h>
#include <vnl/io/vnl_io_matrix_fixed.h>

template <class T>
void vsl_b_write(vsl_b_ostream & os, vpgl_rational_camera<T> const& camera)
{
  if (!os) return;
  unsigned version = 1;
  vsl_b_write(os, version);
  // write rational coefficients
  vsl_b_write(os, camera.coefficient_matrix());
  // write scale offsets
  std::vector<vpgl_scale_offset<T> > scale_off = camera.scale_offsets();
  vsl_b_write(os,scale_off.size());
  for (unsigned i=0; i<scale_off.size(); ++i) {
    vsl_b_write(os,scale_off[i].scale());
    vsl_b_write(os,scale_off[i].offset());
  }
}

//: Binary load camera from stream.
template <class T>
void vsl_b_read(vsl_b_istream & is, vpgl_rational_camera<T> &camera)
{
  if (!is) return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
    case 1:
    {
      // read rational coefficients
      vnl_matrix_fixed<T, 4, 20> coefficient_matrix;
      vsl_b_read(is,coefficient_matrix);
      // read scale+offsets
      unsigned n_scale_offsets;
      vsl_b_read(is,n_scale_offsets);
      std::vector<vpgl_scale_offset<T> > scale_off(n_scale_offsets);
      for (unsigned i=0; i<n_scale_offsets; ++i) {
        T scale, off;
        vsl_b_read(is,scale);
        vsl_b_read(is,off);
        scale_off[i] = vpgl_scale_offset<T>(scale,off);
      }
      camera.set_coefficients(coefficient_matrix);
      camera.set_scale_offsets(scale_off);
      break;
    }
    default:
      std::cerr << "I/O ERROR: vpgl_rational_camera::b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< ver << '\n';
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream& os,const vpgl_rational_camera<T> & c)
{
  os << c << '\n';
}


#define VPGL_IO_RATIONAL_CAMERA_INSTANTIATE(T) \
template void vsl_b_write(vsl_b_ostream & os, vpgl_rational_camera<T > const& camera); \
template void vsl_b_read(vsl_b_istream & is, vpgl_rational_camera<T > &camera); \
template void vsl_print_summary(std::ostream& os,const vpgl_rational_camera<T > & b)

#endif // vpgl_io_rational_camera_hxx_
