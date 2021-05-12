// This is core/vgl/io/vgl_io_oriented_box_2d.hxx
#ifndef vgl_io_oriented_box_2d_hxx_
#define vgl_io_oriented_box_2d_hxx_
//:
// \file

#include <iostream>
#include "vgl_io_oriented_box_2d.h"
#include <vsl/vsl_binary_io.h>

//============================================================================
//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_oriented_box_2d<T> & p)
{
  constexpr short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, p.major_axis());
  vsl_b_write(os, p.height());
}

//============================================================================
//: Binary load self from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vgl_oriented_box_2d<T> & p)
{
  if (!is) return;

  short v;
  vgl_line_segment_2d<T> major_axis;
  T height;
  vsl_b_read(is, v);
  switch (v)
  {
   case 1:
    vsl_b_read(is, major_axis);
    vsl_b_read(is, height);

    p.set(major_axis, height / static_cast<T>(2));
    break;

   default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vgl_oriented_box_2d<T>&)\n"
              << "           Unknown version number "<< v << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//============================================================================
//: Output a human readable summary to the stream
template<class T>
void vsl_print_summary(std::ostream& os,const vgl_oriented_box_2d<T> & p)
{
  os << "2d oriented box with major axis (" << p.major_axis() << ")" << std::endl;
}

#define VGL_IO_ORIENTED_BOX_2D_INSTANTIATE(T) \
template void vsl_print_summary(std::ostream &, const vgl_oriented_box_2d<T > &); \
template void vsl_b_read(vsl_b_istream &, vgl_oriented_box_2d<T > &); \
template void vsl_b_write(vsl_b_ostream &, const vgl_oriented_box_2d<T > &)

#endif // vgl_io_oriented_box_2d_hxx_
