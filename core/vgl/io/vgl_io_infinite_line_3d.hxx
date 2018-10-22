// This is core/vgl/io/vgl_io_infinite_line_3d.hxx
#ifndef vgl_io_infinite_line_3d_hxx_
#define vgl_io_infinite_line_3d_hxx_
//:
// \file

#include <iostream>
#include "vgl_io_infinite_line_3d.h"
#include "vgl_io_vector_2d.h"
#include "vgl_io_vector_3d.h"

//======================================================================
//: Binary save vgl_infinite_line_3d to stream.
template<class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_infinite_line_3d<T>& v)
{
  constexpr short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, v.x0());
  vsl_b_write(os, v.direction());
}

//======================================================================
//: Binary load vgl_infinite_line_3d from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vgl_infinite_line_3d<T>& v)
{
  if (!is) return;

  vgl_vector_2d<T> x0;
  vgl_vector_3d<T> dir;
  short w;
  vsl_b_read(is, w);
  switch (w)
  {
   case 1:
    vsl_b_read(is, x0);
    vsl_b_read(is, dir);
    v.set(x0,dir);
    break;

   default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vgl_infinite_line_3d<T>&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//======================================================================
//: Output a human readable summary of a vgl_infinite_line_3d object to the stream
template<class T>
void vsl_print_summary(std::ostream &os, const vgl_infinite_line_3d<T>& v)
{
    os << "infinite_line: (position vector=" << v.x0()<< " direction=" << v.direction() <<")";
}

#define VGL_IO_INFINITE_LINE_3D_INSTANTIATE(T) \
template void vsl_print_summary(std::ostream &, const vgl_infinite_line_3d<T >&); \
template void vsl_b_read(vsl_b_istream &, vgl_infinite_line_3d<T >&); \
template void vsl_b_write(vsl_b_ostream &, const vgl_infinite_line_3d<T >&)

#endif // vgl_io_infinite_line_3d_hxx_
