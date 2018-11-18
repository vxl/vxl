//:
// \file
// \brief Old deprecated version Specialised version of binary IO for vector<vector<bool> > for backwards compatibility only.
// \author Kevin de Souza (based on vsl_vector_io_bool.cxx by Ian Scott)
//

#include <iostream>
#include "vsl_vector_io.h"
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vcl_deprecated.h>

//====================================================================================
//: Read vector from binary stream in an old deprecated format.
// \deprecated in favour of correctly work normal vsl behaviour. Only kept for backwards compatibility.
// If no-one complains about the name change to this code, or otherwise asks for it
// to be kept, then it should be removed after the release of VXL 1.18. The name change will
// however remain, to avoid overriding normal performance of vsl(vec<vec<bool>>) IMS July 2012.
void vsl_b_read_vec_vec_bool_old(vsl_b_istream& is, std::vector<std::vector<bool> >& v)
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
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, std::vector<std::vector<T> >&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
