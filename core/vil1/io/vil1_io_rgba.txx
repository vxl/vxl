// This is vxl/vil/io/vil_io_rgba.txx
#ifndef vil_io_rgba_txx_
#define vil_io_rgba_txx_
//:
// \file

#include "vil_io_rgba.h"

//========================================================================
//: Binary save vil_rgba to stream.
template<class T>
void vsl_b_write(vsl_b_ostream &os, const vil_rgba<T>& v)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, v.R());
  vsl_b_write(os, v.G());
  vsl_b_write(os, v.B());
  vsl_b_write(os, v.A());
}

//========================================================================
//: Binary load vil_rgba from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vil_rgba<T>& v)
{
  if (!is) return;

  short w;
  vsl_b_read(is, w);
  switch(w)
  {
  case 1:
    vsl_b_read(is, v.r);
    vsl_b_read(is, v.g);
    vsl_b_read(is, v.b);
    vsl_b_read(is, v.a);
    break;

  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vil_rgba<T>&) \n";
    vcl_cerr << "           Unknown version number "<< w << "\n";
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//========================================================================
//: Output a human readable summary of a vil_rgba object to the stream
template<class T>
void vsl_print_summary(vcl_ostream &os, const vil_rgba<T>& v)
{
    os<<"Rgba : ( "<<v.R()<<" , "<<v.G()<<" , "<<v.B()<<" , "<<v.A()<<" )";
}

#define VIL_IO_RGBA_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream &, const vil_rgba<T >&); \
template void vsl_b_read(vsl_b_istream &, vil_rgba<T >&); \
template void vsl_b_write(vsl_b_ostream &, const vil_rgba<T >&)

#endif // vil_io_rgba_txx_
