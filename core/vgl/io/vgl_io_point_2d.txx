// This is core/vgl/io/vgl_io_point_2d.txx
#ifndef vgl_io_point_2d_txx_
#define vgl_io_point_2d_txx_
//:
// \file

#include "vgl_io_point_2d.h"

//============================================================================
//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_point_2d<T> & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, p.x());
  vsl_b_write(os, p.y());
}

//============================================================================
//: Binary load self from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vgl_point_2d<T> & p)
{
  if (!is) return;

  short v;
  vsl_b_read(is, v);
  switch (v)
  {
   case 1:
    T x, y;
    vsl_b_read(is, x);
    vsl_b_read(is, y);
    p.set(x,y);
    break;

   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vgl_point_2d<T>&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//============================================================================
//: Output a human readable summary to the stream
template<class T>
void vsl_print_summary(vcl_ostream& os,const vgl_point_2d<T> & p)
{
    os<<'('<<p.x()<<','<<p.y()<<')';
}

#define VGL_IO_POINT_2D_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream &, const vgl_point_2d<T > &); \
template void vsl_b_read(vsl_b_istream &, vgl_point_2d<T > &); \
template void vsl_b_write(vsl_b_ostream &, const vgl_point_2d<T > &)

#endif // vgl_io_point_2d_txx_
