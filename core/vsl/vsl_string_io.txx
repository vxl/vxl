// This is core/vsl/vsl_string_io.txx
#ifndef vsl_string_io_txx_
#define vsl_string_io_txx_
//:
// \file
// \brief  binary IO functions for vcl_basic_string<T>
// \author K.Y.McGaul

#include "vsl_string_io.h"
#include <vsl/vsl_binary_io.h>

//====================================================================================
//: Write basic_string to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_basic_string<T>& v)
{
  const short version_no = 1;
  vsl_b_write(s, version_no);
  unsigned n = v.length();
  vsl_b_write(s,n);
  for (int i=0;i<n;++i)
    vsl_b_write(s,v[i]);
}

//====================================================================================
//: Read basic_string from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& s, vcl_basic_string<T>& v)
{
  if (!is) return;

  unsigned n;
  short ver;
  vsl_b_read(s, ver);
  switch (ver)
  {
  case 1:
    vsl_b_read(s,n);
    v.resize(n);
    for (int i=0;i<n;++i)
      vsl_b_read(s,v[i]);
    break;
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vcl_basic_string<T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


#define VSL_STRING_IO_INSTANTIATE(T) \
template void vsl_b_write(vsl_b_ostream& s, const vcl_basic_string<T >& v); \
template void vsl_b_read(vsl_b_istream& s, vcl_basic_string<T >& v)

#endif // vsl_string_io_txx_
