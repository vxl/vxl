//:
// \file
// \author Isabel Restrepo
// \date 10-Feb-2011

#include "bvpl_taylor_basis_sample.h"
#include <vnl/io/vnl_io_matrix_fixed.h>
#include <vnl/io/vnl_io_vector_fixed.h>

std::ostream& operator << (std::ostream& os, const bvpl_taylor_basis2_sample& sample)
{
  os << "I0 = " << sample.I0 << ", G= " <<sample.G<< ", H=" << sample.H << std::endl;
  return os;
}

//I/O
void vsl_b_write(vsl_b_ostream & os, bvpl_taylor_basis2_sample const &sample)
{
  vsl_b_write(os, sample.version_no());
  vsl_b_write(os, sample.I0);
  vsl_b_write(os, sample.G);
  vsl_b_write(os, sample.H);

}

void vsl_b_write(vsl_b_ostream & os, bvpl_taylor_basis2_sample const * &sample)
{
  if (sample) {
    vsl_b_write(os, *sample);
  }
}

void vsl_b_read(vsl_b_istream & is, bvpl_taylor_basis2_sample &sample)
{
  if (!is) return;

  short version;
  vsl_b_read(is,version);
  switch (version)
  {
    case 1:
      vsl_b_read(is, sample.I0);
      vsl_b_read(is, sample.G);
      vsl_b_read(is, sample.H);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_sample<T>&)\n"
      << "           Unknown version number "<< version << '\n';
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      break;
  }

}

void vsl_b_read(vsl_b_istream & is, bvpl_taylor_basis2_sample *&sample)
{
  vsl_b_read(is, *sample);
}
