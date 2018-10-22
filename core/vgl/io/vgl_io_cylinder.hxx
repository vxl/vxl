#ifndef vgl_io_cylinder_hxx_
#define vgl_io_cylinder_hxx_
//:
// \file
#include <iostream>
#include "vgl_io_cylinder.h"
#include <vgl/io/vgl_io_point_3d.h>
#include <vgl/io/vgl_io_vector_3d.h>
#include <vgl/vgl_cylinder.h>
#include <vsl/vsl_binary_io.h>


//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_cylinder<T> & cyl)
{
  constexpr short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, cyl.center());
  vsl_b_write(os, cyl.radius());
  vsl_b_write(os, cyl.length());
  vsl_b_write(os, cyl.orientation());
}

//: Binary load self from stream
template<class T>
void vsl_b_read(vsl_b_istream &is, vgl_cylinder<T> & cyl)
{
  vgl_point_3d<double> center;
  vgl_vector_3d<double> orient;
  double radius, length;

  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
   // read center
    vsl_b_read(is, center);
    cyl.set_center(center);

    // read radius
    vsl_b_read(is, radius);
    cyl.set_radius(radius);

    // read length
    vsl_b_read(is, length);
    cyl.set_length(length);

    // read orientation
    vsl_b_read(is, orient);
    cyl.set_orientation(orient);
    break;
   default:
    std::cerr << "vsol_cylinder: unknown I/O version " << ver << '\n';
  }
}

//: Print an ascii summary to the stream
template<class T>
void vsl_print_summary(std::ostream& os, const vgl_cylinder<T> & cyl)
{
  //os << *this;
  os << "Cylinder with center=" << cyl.center() << " radius=" << cyl.radius() << " length=" << cyl.length() << std::endl;
}

#undef VGL_IO_CYLINDER_INSTANTIATE
#define VGL_IO_CYLINDER_INSTANTIATE(T) \
template void vsl_print_summary(std::ostream &, const vgl_cylinder<T > &); \
template void vsl_b_read(vsl_b_istream &, vgl_cylinder<T > &); \
template void vsl_b_write(vsl_b_ostream &, const vgl_cylinder<T > &)

#endif
