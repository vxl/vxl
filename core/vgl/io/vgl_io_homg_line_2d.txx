// This is core/vgl/io/vgl_io_homg_line_2d.txx
#ifndef vgl_io_homg_line_2d_txx_
#define vgl_io_homg_line_2d_txx_
//:
// \file

#include "vgl_io_homg_line_2d.h"
#include <vsl/vsl_binary_io.h>

//======================================================================
//: Binary save vgl_homg_line_2d to stream.
template<class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_homg_line_2d<T>& v)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, v.a());
  vsl_b_write(os, v.b());
  vsl_b_write(os, v.c());
}

//======================================================================
//: Binary load vgl_homg_line_2d from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vgl_homg_line_2d<T>& v)
{
  if (!is) return;

  T a,b,c;
  short w;
  vsl_b_read(is, w);
  switch (w)
  {
   case 1:
    vsl_b_read(is, a);
    vsl_b_read(is, b);
    vsl_b_read(is, c);
    v.set(a,b,c);
    break;

   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vgl_homg_line_2d<T>&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//======================================================================
//: Output a human readable summary of a vgl_homg_line_2d object to the stream
template<class T>
void vsl_print_summary(vcl_ostream &os, const vgl_homg_line_2d<T>& v)
{
    os<<"Homg_line: ( "<<v.a()<<"x + "<<v.b()<<"y + "<<v.c()<<"z = 0 )";
}

#define VGL_IO_HOMG_LINE_2D_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream &, const vgl_homg_line_2d<T >&); \
template void vsl_b_read(vsl_b_istream &, vgl_homg_line_2d<T >&); \
template void vsl_b_write(vsl_b_ostream &, const vgl_homg_line_2d<T >&)

#endif // vgl_io_homg_line_2d_txx_
