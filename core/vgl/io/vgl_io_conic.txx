// This is core/vgl/io/vgl_io_conic.txx
#ifndef vgl_io_conic_txx_
#define vgl_io_conic_txx_
//:
// \file

#include "vgl_io_conic.h"
#include <vgl/vgl_conic.h>
#include <vsl/vsl_binary_io.h>

//============================================================================
//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream &os, vgl_conic<T> const& conic)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, conic.a());
  vsl_b_write(os, conic.b());
  vsl_b_write(os, conic.c());
  vsl_b_write(os, conic.d());
  vsl_b_write(os, conic.e());
  vsl_b_write(os, conic.f());
}

//============================================================================
//: Binary load self from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vgl_conic<T> & conic)
{
  if (!is) return;

  short v;
  vsl_b_read(is, v);
  switch (v)
  {
   case 1: // I/O version 1
    T a, b, c, d, e, f;
    vsl_b_read(is, a);
    vsl_b_read(is, b);
    vsl_b_read(is, c);
    vsl_b_read(is, d);
    vsl_b_read(is, e);
    vsl_b_read(is, f);
    conic.set(a,b,c,d,e,f);
    break;

   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vgl_conic<T>&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//============================================================================
//: Output a human readable summary to the stream
template<class T>
void vsl_print_summary(vcl_ostream& os, vgl_conic<T> const& conic)
{
    os<<conic;
}

#undef VGL_IO_CONIC_INSTANTIATE
#define VGL_IO_CONIC_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream &, vgl_conic<T > const&); \
template void vsl_b_read(vsl_b_istream &, vgl_conic<T > &); \
template void vsl_b_write(vsl_b_ostream &, vgl_conic<T > const&)

#endif // vgl_io_conic_txx_
