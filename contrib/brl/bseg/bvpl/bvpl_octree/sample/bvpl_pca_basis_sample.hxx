#ifndef bvpl_pca_basis_sample_hxx_
#define bvpl_pca_basis_sample_hxx_
//:
// \file
// \author Isabel Restrepo
// \date 28-Feb-2011

#include "bvpl_pca_basis_sample.h"

#include <vnl/io/vnl_io_vector_fixed.h>
#include <vsl/vsl_binary_io.h>


template <unsigned dim>
std::ostream& operator << (std::ostream& os, const bvpl_pca_basis_sample<dim>& sample)
{
  os << "PCA projection = " << sample.pca_projections_ << std::endl;
  return os;
}

//I/O
template <unsigned dim>
void vsl_b_write(vsl_b_ostream & os, bvpl_pca_basis_sample<dim> const &sample)
{
  vsl_b_write(os, sample.version_no());
  vsl_b_write(os, sample.pca_projections_);
}

template <unsigned dim>
void vsl_b_write(vsl_b_ostream & os, bvpl_pca_basis_sample<dim> const * &sample)
{
  if (sample) {
    vsl_b_write(os, *sample);
  }
}

template <unsigned dim>
void vsl_b_read(vsl_b_istream & is, bvpl_pca_basis_sample<dim> &sample)
{
  if (!is) return;

  short version;
  vsl_b_read(is,version);
  switch (version)
  {
    case 1:
      vsl_b_read(is, sample.pca_projections_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_sample<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      break;
  }
}

template <unsigned dim>
void vsl_b_read(vsl_b_istream & is, bvpl_pca_basis_sample<dim> *&sample)
{
  vsl_b_read(is, *sample);
}


#define BVPL_PCA_BASIS_SAMPLE_INSTANTIATE(T) \
template class bvpl_pca_basis_sample<T >; \
template std::ostream& operator << (std::ostream&, const bvpl_pca_basis_sample<T >&); \
template void vsl_b_write(vsl_b_ostream &, bvpl_pca_basis_sample<T > const &); \
template void vsl_b_write(vsl_b_ostream &, bvpl_pca_basis_sample<T > const *&); \
template void vsl_b_read(vsl_b_istream &, bvpl_pca_basis_sample<T > &); \
template void vsl_b_read(vsl_b_istream &, bvpl_pca_basis_sample<T > *&)

#endif // bvpl_pca_basis_sample_hxx_
