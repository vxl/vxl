//:
// \file
// \brief Specialised version of binary IO for vector<bool>
// \author Ian Scott
//

#include "vsl_vector_io.h"
#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>

//====================================================================================
//: Write vector to binary stream
VCL_DEFINE_SPECIALIZATION
void vsl_b_write(vsl_b_ostream& s, const vcl_vector<bool>& v)
{
  const short version_no = 1;
  vsl_b_write(s, version_no);
  unsigned n = v.size();
  vsl_b_write(s,n);
  for (unsigned i=0; i<n; ++i)
    vsl_b_write(s, v[i]);
}

//====================================================================================
//: Read vector from binary stream
VCL_DEFINE_SPECIALIZATION
void vsl_b_read(vsl_b_istream& is, vcl_vector<bool>& v)
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
    for (unsigned i=0; i<n; ++i)
    {
      bool b;
      vsl_b_read(is, b);
      v[i] = b;
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
VCL_DEFINE_SPECIALIZATION
void vsl_print_summary(vcl_ostream& os, const vcl_vector<bool> &v)
{
  os << "Vector length: " << v.size() << vcl_endl;
  for (unsigned int i=0; i<v.size() && i<5; i++)
  {
    os << ' ' << i << ": ";
    vsl_print_summary(os, v[i]);
    os << vcl_endl;
  }
  if (v.size() > 5)
    os << " ..." << vcl_endl;
}


