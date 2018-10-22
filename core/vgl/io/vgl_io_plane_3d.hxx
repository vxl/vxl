// This is core/vgl/io/vgl_io_plane_3d.hxx
#ifndef vgl_io_plane_3d_hxx_
#define vgl_io_plane_3d_hxx_
//:
// \file

#include <iostream>
#include "vgl_io_plane_3d.h"
#include <vgl/vgl_plane_3d.h>
#include <vsl/vsl_binary_io.h>

//============================================================================
//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_plane_3d<T> & p)
{
  constexpr short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, p.a());
  vsl_b_write(os, p.b());
  vsl_b_write(os, p.c());
  vsl_b_write(os, p.d());
}

//============================================================================
//: Binary load self from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vgl_plane_3d<T> & p)
{
  if (!is) return;

  short v;
  vsl_b_read(is, v);
  switch (v)
  {
   case 1:
    T a, b, c, d;
    vsl_b_read(is, a);
    vsl_b_read(is, b);
    vsl_b_read(is, c);
    vsl_b_read(is, d);
    p.set(a,b,c,d);
    break;

   default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vgl_plane_3d<T>&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//============================================================================
//: Output a human readable summary to the stream
template<class T>
void vsl_print_summary(std::ostream& os,const vgl_plane_3d<T> & p)
{
    os<<'('<<p.a()<<','<<p.b()<<','<<p.c()<<','<<p.d()<<')';
}

#define VGL_IO_PLANE_3D_INSTANTIATE(T) \
template void vsl_print_summary(std::ostream &, const vgl_plane_3d<T > &); \
template void vsl_b_read(vsl_b_istream &, vgl_plane_3d<T > &); \
template void vsl_b_write(vsl_b_ostream &, const vgl_plane_3d<T > &)

#endif // vgl_io_plane_3d_hxx_
