// This is core/vsl/vsl_string_io.hxx
#ifndef vsl_string_io_hxx_
#define vsl_string_io_hxx_
//:
// \file
// \brief  binary IO functions for std::basic_string<T>
// \author K.Y.McGaul

#include "vsl_string_io.h"
#include <vsl/vsl_binary_io.h>

//====================================================================================
//: Write basic_string to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const std::basic_string<T>& v)
{
  constexpr short version_no = 1;
  vsl_b_write(s, version_no);
  unsigned n = v.length();
  vsl_b_write(s,n);
  for (int i=0;i<n;++i)
    vsl_b_write(s,v[i]);
}

//====================================================================================
//: Read basic_string from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& is, std::basic_string<T>& v)
{
  if (!is) return;

  unsigned n;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsl_b_read(is,n);
    v.resize(n);
    for (int i=0;i<n;++i)
      vsl_b_read(is,v[i]);
    break;
   default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, std::basic_string<T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


#define VSL_STRING_IO_INSTANTIATE(T) \
template void vsl_b_write(vsl_b_ostream& s, const std::basic_string<T >& v); \
template void vsl_b_read(vsl_b_istream& s, std::basic_string<T >& v)

#endif // vsl_string_io_hxx_
