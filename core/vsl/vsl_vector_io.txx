// This is core/vsl/vsl_vector_io.txx
#ifndef vsl_vector_io_txx_
#define vsl_vector_io_txx_
//:
// \file
// \brief binary IO functions for vcl_vector<T>
// \author Tim Cootes

#include "vsl_vector_io.h"
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_block_binary.h>
#include <vsl/vsl_b_read_block_old.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vsl/vsl_indent.h>

//====================================================================================
//: Write vector to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_vector<T>& v)
{
  unsigned n = v.size();
  // There is nothing in the STL standard that says that vector<> has
  // to store its data in a contiguous memory block. However, most
  // implementations do store data this way.
  // Check this assumption holds.
  assert(n == 0 || &v[n-1] + 1 == &v[0] + n);

  const short version_no = 2;
  vsl_b_write(s, version_no);
  vsl_b_write(s,n);
  if (n!=0)
    vsl_block_binary_write(s, &v.front(), n);
}

//====================================================================================
//: Read vector from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& is, vcl_vector<T>& v)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  unsigned n;
  vsl_b_read(is,n);
  v.resize(n);

  // There is nothing in the STL standard that says that vector<> has
  // to store its data in a contiguous memory block. However, most
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
      vsl_block_binary_read(is, &v.front(), n);
    }
    break;

   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vcl_vector<T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//====================================================================================
//: Output a human readable summary to the stream
template <class T>
void vsl_print_summary(vcl_ostream& os, const vcl_vector<T> &v)
{
  os << vsl_indent() << "Vector length: " << v.size() << vcl_endl;
  for (unsigned int i=0; i<v.size() && i<5; i++)
  {
    os << vsl_indent() << ' ' << i << ": ";
    vsl_indent_inc(os);
    vsl_print_summary(os, v[i]);
    os << vcl_endl;
    vsl_indent_dec(os);
  }
  if (v.size() > 5)
    os << vsl_indent() << " ...\n";
}


#define VSL_VECTOR_IO_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream& s, const vcl_vector<T >& v); \
template void vsl_b_write(vsl_b_ostream& s, const vcl_vector<T >& v); \
template void vsl_b_read(vsl_b_istream& s, vcl_vector<T >& v)

#endif // vsl_vector_io_txx_
