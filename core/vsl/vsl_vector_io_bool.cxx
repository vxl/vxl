//:
// \file
// \brief Specialised version of binary IO for vector<bool>
// \author Ian Scott
//

#include "vsl_vector_io.h"
#include <vsl/vsl_binary_io.h>
#include <vcl_compiler.h>
#include <iostream>

//====================================================================================
//: Write vector to binary stream
VCL_DEFINE_SPECIALIZATION
void vsl_b_write(vsl_b_ostream& s, const std::vector<bool>& v)
{
  const short version_no = 1;
  vsl_b_write(s, version_no);
  unsigned int n = (unsigned int)(v.size());
  vsl_b_write(s,n);
  for (unsigned int i=0; i<n; ++i)
    vsl_b_write(s, v[i]);
}

//====================================================================================
//: Read vector from binary stream
VCL_DEFINE_SPECIALIZATION
void vsl_b_read(vsl_b_istream& is, std::vector<bool>& v)
{
  if (!is) return;

  unsigned int n;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
  case 1:
    vsl_b_read(is,n);
    v.resize(n);
    for (unsigned int i=0; i<n; ++i)
    {
      bool b;
      vsl_b_read(is, b);
      v[i] = b;
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
VCL_DEFINE_SPECIALIZATION
void vsl_print_summary(std::ostream& os, const std::vector<bool> &v)
{
  os << "Vector length: " << v.size() << '\n';
  for (unsigned int i=0; i<v.size() && i<5; i++)
  {
    os << ' ' << i << ": ";
    vsl_print_summary(os, v[i]);
    os << '\n';
  }
  if (v.size() > 5)
    os << " ..." << '\n';
}


