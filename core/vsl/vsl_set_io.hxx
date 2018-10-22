// This is core/vsl/vsl_set_io.hxx
#ifndef vsl_set_io_hxx_
#define vsl_set_io_hxx_
//:
// \file
// \brief  binary IO functions for std::set<T>
// \author K.Y.McGaul

#include <iostream>
#include "vsl_set_io.h"
#include <vsl/vsl_binary_io.h>

//====================================================================================
//: Write set to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const std::set<T>& v)
{
  constexpr short version_no = 1;
  vsl_b_write(s, version_no);
  vsl_b_write(s, v.size());
  for (typename std::set<T>::const_iterator iter = v.begin(); iter != v.end(); iter++)
    vsl_b_write(s,*iter);
}

//====================================================================================
//: Read set from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& is, std::set<T>& v)
{
  if (!is) return;

  v.clear();
  unsigned set_size;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
  case 1:
    vsl_b_read(is, set_size);
    for (unsigned i=0; i<set_size; i++)
    {
      T tmp;
      vsl_b_read(is,tmp);
      v.insert(tmp);
    }
    break;
  default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, std::set<T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//====================================================================================
//: Output a human readable summary to the stream
template <class T>
void vsl_print_summary(std::ostream& os, const std::set<T> &v)
{
  os << "Set length: " << v.size() << '\n';
  unsigned i =0;
  for (typename std::set<T>::const_iterator iter = v.begin();
       iter != v.end() && i<5; ++iter,++i)
  {
    os << ' ' << i << ": ";
    vsl_print_summary(os, *iter);
    os << '\n';
  }
  if (v.size() > 5)
    os << " ...\n";
}


#define VSL_SET_IO_INSTANTIATE(T) \
template void vsl_print_summary(std::ostream& s, const std::set<T >& v); \
template void vsl_b_write(vsl_b_ostream& s, const std::set<T >& v); \
template void vsl_b_read(vsl_b_istream& s, std::set<T >& v)

#endif // vsl_set_io_hxx_
