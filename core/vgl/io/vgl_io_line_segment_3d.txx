// This is core/vgl/io/vgl_io_line_segment_3d.txx
#ifndef vgl_io_line_segment_3d_txx_
#define vgl_io_line_segment_3d_txx_
//:
// \file

#include "vgl_io_line_segment_3d.h"
#include <vgl/io/vgl_io_point_3d.h>

//============================================================================
//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_line_segment_3d<T> & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, p.point1());
  vsl_b_write(os, p.point2());
}

//============================================================================
//: Binary load self from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vgl_line_segment_3d<T> & p)
{
  if (!is) return;

  short v;
  vsl_b_read(is, v);
  switch (v)
  {
   case 1: {
    vgl_point_3d<T> p1, p2;
    vsl_b_read(is, p1);
    vsl_b_read(is, p2);
    p.set(p1,p2);
    break;
   }

   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vgl_line_segment_3d<T>&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//============================================================================
//: Output a human readable summary to the stream
template<class T>
void vsl_print_summary(vcl_ostream& os,const vgl_line_segment_3d<T> & l)
{
  os<<"Segment from ";
  vsl_print_summary(os,l.point1());
  os<<" to ";
  vsl_print_summary(os,l.point2());
}

#define VGL_IO_LINE_SEGMENT_3D_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream &, vgl_line_segment_3d<T >const&); \
template void vsl_b_read(vsl_b_istream &, vgl_line_segment_3d<T > &); \
template void vsl_b_write(vsl_b_ostream &, const vgl_line_segment_3d<T > &)

#endif // vgl_io_line_segment_3d_txx_
