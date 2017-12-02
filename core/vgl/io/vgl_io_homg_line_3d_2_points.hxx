// This is core/vgl/io/vgl_io_homg_line_3d_2_points.hxx
#ifndef vgl_io_homg_line_3d_2_points_hxx_
#define vgl_io_homg_line_3d_2_points_hxx_
//:
// \file
// \brief  See vgl_io_homg_line_3d_2_points.h for a description of this file.
// \author John Kang (Manchester)

#include <iostream>
#include "vgl_io_homg_line_3d_2_points.h"
#include <vgl/io/vgl_io_homg_point_3d.h>

#include <vsl/vsl_binary_io.h>

//============================================================================
//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_homg_line_3d_2_points<T> & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, p.point_finite());
  vsl_b_write(os, p.point_infinite());
}

//============================================================================
//: Binary load self from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vgl_homg_line_3d_2_points<T> & p)
{
  if (!is) return;

  short v;
  vsl_b_read(is, v);
  switch (v)
  {
   case 1: {
    vgl_homg_point_3d<T> p1, p2;
    vsl_b_read(is, p1);
    vsl_b_read(is, p2);
    p.set(p1,p2);
    break;
   }

   default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vgl_homg_line_3d_2_points<T>&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//============================================================================
//: Output a human readable summary to the stream
template<class T>
void vsl_print_summary(std::ostream& os,const vgl_homg_line_3d_2_points<T> & p)
{
  os << '('; vsl_print_summary(os,p.point_finite());
  os << ','; vsl_print_summary(os,p.point_infinite());
  os << ')';
}

#define VGL_IO_HOMG_LINE_3D_2_POINTS_INSTANTIATE(T) \
template void vsl_print_summary(std::ostream &, \
                                const vgl_homg_line_3d_2_points<T > &); \
template void vsl_b_read(vsl_b_istream &, vgl_homg_line_3d_2_points<T > &); \
template void vsl_b_write(vsl_b_ostream &, vgl_homg_line_3d_2_points<T >const&)

#endif // vgl_io_homg_line_3d_2_points_hxx_
