// This is core/vgl/io/vgl_io_box_3d.txx
#ifndef vgl_io_box_3d_txx_
#define vgl_io_box_3d_txx_
//:
// \file

#include "vgl_io_box_3d.h"
#include <vgl/vgl_box_3d.h>
#include <vsl/vsl_binary_io.h>

//============================================================================
//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_box_3d<T> & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, p.min_x());
  vsl_b_write(os, p.min_y());
  vsl_b_write(os, p.min_z());
  vsl_b_write(os, p.max_x());
  vsl_b_write(os, p.max_y());
  vsl_b_write(os, p.max_z());
}

//============================================================================
//: Binary load self from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vgl_box_3d<T> & p)
{
  if (!is) return;

  short v;
  T min_pos[3];
  T max_pos[3];
  vsl_b_read(is, v);
  switch (v)
  {
   case 1:
    vsl_b_read(is, min_pos[0]);
    vsl_b_read(is, min_pos[1]);
    vsl_b_read(is, min_pos[2]);
    vsl_b_read(is, max_pos[0]);
    vsl_b_read(is, max_pos[1]);
    vsl_b_read(is, max_pos[2]);
    p.set_min_position(min_pos);
    p.set_max_position(max_pos);
    break;

   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vgl_box_3d<T>&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//============================================================================
//: Output a human readable summary to the stream
template<class T>
void vsl_print_summary(vcl_ostream& os,const vgl_box_3d<T> & p)
{
  if (p.is_empty())
    os<<"Empty 3d box\n";
  else
    os<<"3d box with opposite corners at ("
      <<p.min_x() << ',' << p.min_y() << ',' << p.min_z() <<") and ("
      <<p.max_x() << ',' << p.max_y() << ',' << p.max_z() <<")\n";
}

#define VGL_IO_BOX_3D_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream &, const vgl_box_3d<T > &); \
template void vsl_b_read(vsl_b_istream &, vgl_box_3d<T > &); \
template void vsl_b_write(vsl_b_ostream &, const vgl_box_3d<T > &)

#endif // vgl_io_box_3d_txx_
