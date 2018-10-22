// This is core/vgl/io/vgl_io_vector_3d.hxx
#ifndef vgl_io_vector_3d_hxx_
#define vgl_io_vector_3d_hxx_
//:
// \file

#include <iostream>
#include "vgl_io_vector_3d.h"
#include <vgl/vgl_vector_3d.h>
#include <vsl/vsl_binary_io.h>

//============================================================================
//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_vector_3d<T> & v)
{
  constexpr short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, v.x());
  vsl_b_write(os, v.y());
  vsl_b_write(os, v.z());
}

//============================================================================
//: Binary load self from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vgl_vector_3d<T> & vec)
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
    vec.set(x,y,z);
    break;

   default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vgl_vector_3d<T>&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//============================================================================
//: Output a human readable summary to the stream
template<class T>
void vsl_print_summary(std::ostream& os,const vgl_vector_3d<T> & v)
{
    os<<'('<<v.x()<<','<<v.y()<<','<<v.z()<<')';
}

#define VGL_IO_VECTOR_3D_INSTANTIATE(T) \
template void vsl_print_summary(std::ostream &, const vgl_vector_3d<T > &); \
template void vsl_b_read(vsl_b_istream &, vgl_vector_3d<T > &); \
template void vsl_b_write(vsl_b_ostream &, const vgl_vector_3d<T > &)

#endif // vgl_io_vector_3d_hxx_
