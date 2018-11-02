// This is core/vsl/vsl_deque_io.hxx
#ifndef vsl_deque_io_hxx_
#define vsl_deque_io_hxx_
//:
// \file
// \brief  binary IO functions for std::deque<T>
// \author K.Y.McGaul
//
// Implementation

#include <iostream>
#include "vsl_deque_io.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_indent.h>

//====================================================================================
//: Write deque to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const std::deque<T>& v)
{
  constexpr short version_no = 1;
  vsl_b_write(s, version_no);
  vsl_b_write(s, v.size());
  for (unsigned i=0; i<v.size(); i++)
    vsl_b_write(s,v[i]);
}

//====================================================================================
//: Read deque from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& is, std::deque<T>& v)
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
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, std::deque<T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//====================================================================================
//: Output a human readable summary to the stream
template <class T>
void vsl_print_summary(std::ostream& os, const std::deque<T> &v)
{
  os << "Deque length: " << v.size() << '\n';
  for (unsigned int i=0; i<v.size() && i<5; i++)
  {
    os << vsl_indent() << ' ' << i << ": ";
    vsl_indent_inc(os);
    vsl_print_summary(os,v[i]);
    os << '\n';
    vsl_indent_dec(os);
  }
  if (v.size() > 5)
    os << " ...\n";
}


#define VSL_DEQUE_IO_INSTANTIATE(T) \
template void vsl_print_summary(std::ostream&, const std::deque<T >&); \
template void vsl_b_write(vsl_b_ostream& s, const std::deque<T >& v); \
template void vsl_b_read(vsl_b_istream& s, std::deque<T >& v)

#endif // vsl_deque_io_hxx_
