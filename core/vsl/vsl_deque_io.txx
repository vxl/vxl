// This is core/vsl/vsl_deque_io.txx
#ifndef vsl_deque_io_txx_
#define vsl_deque_io_txx_
//:
// \file
// \brief  binary IO functions for vcl_deque<T>
// \author K.Y.McGaul
//
// Implementation

#include "vsl_deque_io.h"
#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>

//====================================================================================
//: Write deque to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_deque<T>& v)
{
  const short version_no = 1;
  vsl_b_write(s, version_no);
  vsl_b_write(s, v.size());
  for (unsigned i=0; i<v.size(); i++)
    vsl_b_write(s,v[i]);
}

//====================================================================================
//: Read deque from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& is, vcl_deque<T>& v)
{
  if (!is) return;

  unsigned deque_size;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
  case 1:
    vsl_b_read(is, deque_size);
    v.resize(deque_size);
    for (unsigned i=0; i<deque_size; i++)
      vsl_b_read(is,v[i]);
    break;
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vcl_deque<T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//====================================================================================
//: Output a human readable summary to the stream
template <class T>
void vsl_print_summary(vcl_ostream& os, const vcl_deque<T> &v)
{
  os << "Deque length: " << v.size() << vcl_endl;
  for (unsigned int i=0; i<v.size() && i<5; i++)
  {
    os << ' ' << i << ": ";
    vsl_print_summary(os,v[i]);
    os << vcl_endl;
  }
  if (v.size() > 5)
    os << " ...\n";
}


#define VSL_DEQUE_IO_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream&, const vcl_deque<T >&); \
template void vsl_b_write(vsl_b_ostream& s, const vcl_deque<T >& v); \
template void vsl_b_read(vsl_b_istream& s, vcl_deque<T >& v)

#endif // vsl_deque_io_txx_
