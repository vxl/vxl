// This is vxl/vsl/vsl_vector_io.txx
#ifndef vsl_vector_io_txx_
#define vsl_vector_io_txx_
//:
// \file
// \brief binary IO functions for vcl_vector<T>
// \author Tim Cootes
//
// Implementation

#include "vsl_vector_io.h"
#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>
#include <vsl/vsl_binary_explicit_io.h>
#include <vcl_cassert.h>

//====================================================================================
//: Write vector to binary stream
template <class T>
void vsl_b_write(vsl_b_ostream& s, const vcl_vector<T>& v)
{
  // There is nothing in the STL standard that says that vector<> has
  // to store its data in a contiguous memory block. However, most
  // implementations do store data this way.
  // Check this assumption holds.
  assert(v.size() == 0 || &v[v.size() - 1] + 1 == &v[0] + v.size());

  const short version_no = 1;
  vsl_b_write(s, version_no);
  unsigned n = v.size();
  vsl_b_write(s,n);
  if (n)
    vsl_b_write_block(s, &v[0], n);
}

//====================================================================================
//: Read vector from binary stream
template <class T>
void vsl_b_read(vsl_b_istream& is, vcl_vector<T>& v)
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

    // There is nothing in the STL standard that says that vector<> has
    // to store its data in a contiguous memory block. However, most
    // implementations do store data this way.
    // Check this assumption holds.
    assert(v.size() == 0 || &v[v.size() - 1] + 1 == &v[0] + v.size());

    if (v.size())
    {
      vsl_b_read_block(is, &v[0], n);
    }
    break;
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vcl_vector<T>&) \n";
    vcl_cerr << "           Unknown version number "<< ver << "\n";
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//====================================================================================
//: Output a human readable summary to the stream
template <class T>
void vsl_print_summary(vcl_ostream& os, const vcl_vector<T> &v)
{
  os << "Vector length: " << v.size() << vcl_endl;
  for (unsigned int i=0; i<v.size() && i<5; i++)
  {
    os << " " << i << ": ";
    vsl_print_summary(os, v[i]);
    os << vcl_endl;
  }
  if (v.size() > 5)
    os << " ..." << vcl_endl;
}


#define VSL_VECTOR_IO_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream& s, const vcl_vector<T >& v); \
template void vsl_b_write(vsl_b_ostream& s, const vcl_vector<T >& v); \
template void vsl_b_read(vsl_b_istream& s, vcl_vector<T >& v)

#endif // vsl_vector_io_txx_
