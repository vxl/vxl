//:
// \file
// \brief Specialised version of binary IO for vector<vector<bool> >
// \author Kevin de Souza (based on vsl_vector_io_bool.cxx by Ian Scott)
//

#include "vsl_vector_io.h"
#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>

//====================================================================================
//: Write vector to binary stream
VCL_DEFINE_SPECIALIZATION
void vsl_b_write(vsl_b_ostream& s, const vcl_vector<vcl_vector<bool> >& v)
{
  const short version_no = 1;
  vsl_b_write(s, version_no);
  unsigned n = v.size();
  vsl_b_write(s, n);
  for (unsigned i=0; i<n; ++i)
  {
    unsigned m = v[i].size();
    vsl_b_write(s, m);
    for (unsigned j=0; j<m; ++j)
    {
      vsl_b_write(s, v[i][j]);
    }
  }
}

//====================================================================================
//: Read vector from binary stream
VCL_DEFINE_SPECIALIZATION
void vsl_b_read(vsl_b_istream& is, vcl_vector<vcl_vector<bool> >& v)
{
  if (!is) return;

  unsigned n, m;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
  case 1:
    vsl_b_read(is, n);
    v.resize(n);
    for (unsigned i=0; i<n; ++i)
    {
      vsl_b_read(is, m);
      v[i].resize(m);
      for (unsigned j=0; j<m; ++j)
      {
        bool b;
        vsl_b_read(is, b);
        v[i][j] = b;
      }
    }
    break;
    
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vcl_vector<vcl_vector<T> >&) \n";
    vcl_cerr << "           Unknown version number "<< ver << "\n";
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//====================================================================================
//: Output a human readable summary to the stream
VCL_DEFINE_SPECIALIZATION
void vsl_print_summary(vcl_ostream& os, const vcl_vector<vcl_vector<bool> >& v)
{
  unsigned n = v.size();
  os << "Vector length: " << n << "\n";
  for (unsigned int i=0; i<n && i<5; i++)
  {
    os << " " << i << ": ";
    unsigned m = v[i].size();
    os << "\tVector length: " << m << "\n";
    for (unsigned int j=0; j<m && j<5; j++)
    {
      os << " " << j << ": ";
      vsl_print_summary(os, v[i][j]);
    }
    if (m > 5) os << " ... ";
    os << "\n";
  }
  if (n > 5) os << " ...";
  os << "\n";
  
  os << vcl_endl;
}


