// This is core/vgl/io/vgl_io_point_3d.txx
#ifndef vgl_io_point_3d_txx_
#define vgl_io_point_3d_txx_
//:
// \file

#include "vgl_io_point_3d.h"
#include <vgl/vgl_point_3d.h>
#include <vsl/vsl_binary_io.h>

//============================================================================
//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_point_3d<T> & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, p.x());
  vsl_b_write(os, p.y());
  vsl_b_write(os, p.z());
}

//============================================================================
//: Binary load self from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vgl_point_3d<T> & p)
{
  if (!is) return;

  short v;
  vsl_b_read(is, v);
  switch (v)
  {
   case 1:
    T x, y, z;
    vsl_b_read(is, x);
    vsl_b_read(is, y);
    vsl_b_read(is, z);
    p.set(x,y,z);
    break;

   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vgl_point_3d<T>&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//============================================================================
//: Output a human readable summary to the stream
template<class T>
void vsl_print_summary(vcl_ostream& os,const vgl_point_3d<T> & p)
{
    os<<'('<<p.x()<<','<<p.y()<<','<<p.z()<<')';
}

#define VGL_IO_POINT_3D_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream &, const vgl_point_3d<T > &); \
template void vsl_b_read(vsl_b_istream &, vgl_point_3d<T > &); \
template void vsl_b_write(vsl_b_ostream &, const vgl_point_3d<T > &)

#endif // vgl_io_point_3d_txx_
