// This is core/vsl/vsl_list_io.hxx
#ifndef vsl_list_io_hxx_
#define vsl_list_io_hxx_
//:
// \file
// \brief  binary IO functions for std::list<T>
// \author K.Y.McGaul
//
// Implementation

#include <iostream>
#include "vsl_list_io.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_indent.h>

//====================================================================================
//: Write list to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const std::list<T>& v)
{
  constexpr short version_no = 1;
  vsl_b_write(s, version_no);
  vsl_b_write(s, v.size());
  for (typename std::list<T>::const_iterator iter = v.begin(); iter != v.end(); iter++)
    vsl_b_write(s,*iter);
}

//====================================================================================
//: Read list from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& is, std::list<T>& v)
{
  if (!is) return;

  v.clear();
  unsigned list_size;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
  case 1:
    vsl_b_read(is, list_size);
    for (unsigned i=0; i<list_size; i++)
    {
      T tmp;
      vsl_b_read(is,tmp);
      v.push_back(tmp);
    }
    break;
  default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, std::list<T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//====================================================================================
//: Output a human readable summary to the stream
template <class T>
void vsl_print_summary(std::ostream& os, const std::list<T> &v)
{
  unsigned i=0;
  os << "List length: " << v.size() << '\n';
  for (typename std::list<T>::const_iterator iter = v.begin();
       iter != v.end() && i<5; ++iter,++i)
  {
    os << vsl_indent() << ' ' << i << ": ";
    vsl_indent_inc(os);
    vsl_print_summary(os, *iter);
    os << '\n';
    vsl_indent_dec(os);
  }
  if (v.size() > 5)
    os << " ...\n";
}

#define VSL_LIST_IO_INSTANTIATE(T) \
template void vsl_print_summary(std::ostream&, const std::list<T >&); \
template void vsl_b_write(vsl_b_ostream& s, const std::list<T >& v); \
template void vsl_b_read(vsl_b_istream& s, std::list<T >& v)

#endif // vsl_list_io_hxx_
