// This is core/vsl/vsl_vector_io.hxx
#ifndef vsl_vector_io_hxx_
#define vsl_vector_io_hxx_
//:
// \file
// \brief binary IO functions for std::vector<T>
// \author Tim Cootes

#include <iostream>
#include "vsl_vector_io.h"
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_block_binary.h>
#include <vsl/vsl_b_read_block_old.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vsl/vsl_indent.h>

//====================================================================================
//: Write vector to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const std::vector<T>& v)
{
  std::size_t n = v.size();
  // There is nothing in the STL standard that says that vector<> has
  // to store its data in a contiguous memory block. However, most
  // implementations do store data this way.
  // Check this assumption holds.
  assert(n == 0 || &v[n-1] + 1 == &v[0] + n);

  constexpr short version_no = 3;
  vsl_b_write(s, version_no);
  vsl_b_write(s,n);
  if (n!=0)
    vsl_block_binary_write(s, &v.front(), n);
}


template <class T> bool vsl_is_char(const T&);

template <> inline bool vsl_is_char(const unsigned char &)
{ return true; }
template <> inline bool vsl_is_char(const signed char &)
{ return true; }
template <class T> bool vsl_is_char(const T&) { return false; }

//====================================================================================
//: Read vector from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& is, std::vector<T>& v)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  unsigned n;
  vsl_b_read(is,n);
  v.resize(n); // Note that this resize means that the object must be default
               // constructable. It is very hard to see how to avoid this requirement,
               // without designing types that are constructable directly from a stream.

  // In some old versions of the standard STL there is no requirement for
  // vector<> to store its data in a contiguous memory block. However, most
  // implementations do store data this way.
  // Check this assumption holds.
  assert(n == 0 || &v[n-1] + 1 == &v[0] + n);

  switch (ver)
  {
   case 1:
    if (n!=0)
    {
      vsl_b_read_block_old(is, &v.front(), n);
    }
    break;
   case 2:
    if (n!=0)
    {
      if (vsl_is_char(v.front())) // signed char or unsigned char
      {
        vsl_block_binary_read_confirm_specialisation(is, false);
        vsl_b_read_block_old(is, &v.front(), n);
      }
      else
        vsl_block_binary_read(is, &v.front(), n);
    }
    break;
   case 3:
    if (n!=0)
    {
      vsl_block_binary_read(is, &v.front(), n);
    }
    break;


   default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, std::vector<T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//====================================================================================
//: Output a human readable summary to the stream
template <class T>
void vsl_print_summary(std::ostream& os, const std::vector<T> &v)
{
  os << vsl_indent() << "Vector length: " << v.size() << '\n';
  for (unsigned int i=0; i<v.size() && i<5; i++)
  {
    os << vsl_indent() << ' ' << i << ": ";
    vsl_indent_inc(os);
    vsl_print_summary(os, v[i]);
    os << '\n';
    vsl_indent_dec(os);
  }
  if (v.size() > 5)
    os << vsl_indent() << " ...\n";
}


#define VSL_VECTOR_IO_INSTANTIATE(T) \
template void vsl_print_summary(std::ostream& s, const std::vector<T >& v); \
template void vsl_b_write(vsl_b_ostream& s, const std::vector<T >& v); \
template void vsl_b_read(vsl_b_istream& s, std::vector<T >& v)

#endif // vsl_vector_io_hxx_
