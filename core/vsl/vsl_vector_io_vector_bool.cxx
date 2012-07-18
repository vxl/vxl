//:
// \file
// \brief Old deprecated version Specialised version of binary IO for vector<vector<bool> > for backwards compatibility only.
// \author Kevin de Souza (based on vsl_vector_io_bool.cxx by Ian Scott)
//

#include "vsl_vector_io.h"
#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>


//====================================================================================
//: Read vector from binary stream
// \deprecated. Only kept for backwards compatibility.
void vsl_b_read_vec_vec_bool_old(vsl_b_istream& is, vcl_vector<vcl_vector<bool> >& v)
{
  if (!is) return;

  unsigned int n, m;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
  case 1:
    vsl_b_read(is, n);
    v.resize(n);
    for (unsigned int i=0; i<n; ++i)
    {
      vsl_b_read(is, m);
      v[i].resize(m);
      for (unsigned int j=0; j<m; ++j)
      {
        bool b;
        vsl_b_read(is, b);
        v[i][j] = b;
      }
    }
    break;

  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vcl_vector<vcl_vector<T> >&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}



