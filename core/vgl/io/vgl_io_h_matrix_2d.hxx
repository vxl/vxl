// This is core/vgl/io/vgl_io_h_matrix_2d.hxx
#ifndef vgl_io_h_matrix_2d_hxx_
#define vgl_io_h_matrix_2d_hxx_
//:
// \file

#include <iostream>
#include "vgl_io_h_matrix_2d.h"
#include <vsl/vsl_binary_io.h>

//: Binary save vgl_h_matrix_2d to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &bfs, const vgl_h_matrix_2d<T>& t)
{
  static const short io_version_no = 1;
  vsl_b_write(bfs,io_version_no);
  vnl_matrix_fixed<T,3,3> m = t.get_matrix();
  vsl_b_write(bfs,m[0][0]); vsl_b_write(bfs,m[0][1]); vsl_b_write(bfs,m[0][2]);
  vsl_b_write(bfs,m[1][0]); vsl_b_write(bfs,m[1][1]); vsl_b_write(bfs,m[1][2]);
  vsl_b_write(bfs,m[2][0]); vsl_b_write(bfs,m[2][1]); vsl_b_write(bfs,m[2][2]);
}

//: Binary load vgl_h_matrix_2d from stream.
template <class T>
void vsl_b_read(vsl_b_istream &bfs, vgl_h_matrix_2d<T>& v)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version) {
    case 1:
      T xx,xy,xt,yx,yy,yt,tx,ty,tt;
      vsl_b_read(bfs,xx); vsl_b_read(bfs,xy); vsl_b_read(bfs,xt);
      vsl_b_read(bfs,yx); vsl_b_read(bfs,yy); vsl_b_read(bfs,yt);
      vsl_b_read(bfs,tx); vsl_b_read(bfs,ty); vsl_b_read(bfs,tt);
      v.set(0,0,xx); v.set(0,1,xy); v.set(0,2,xt);
      v.set(1,0,yx); v.set(1,1,yy); v.set(1,2,yt);
      v.set(2,0,tx); v.set(2,0,ty); v.set(2,2,tt);
      break;
    default:
      std::cerr << "I/O ERROR: vgl_h_matrix_2d::b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}


#define VGL_IO_H_MATRIX_2D_INSTANTIATE(T) \
template void vsl_b_read(vsl_b_istream &, vgl_h_matrix_2d<T >&); \
template void vsl_b_write(vsl_b_ostream &, const vgl_h_matrix_2d<T >&)

#endif // vgl_io_h_matrix_2d_hxx_
