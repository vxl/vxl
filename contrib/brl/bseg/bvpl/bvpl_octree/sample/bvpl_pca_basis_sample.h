// This is bvpl_pca_basis_sample.h
#ifndef bvpl_pca_basis_sample_h
#define bvpl_pca_basis_sample_h

//:
// \file
// \brief A data structure that contains the vector of projections on pca space
// \author Isabel Restrepo mir@lems.brown.edu
// \date  28-Feb-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_compiler.h>
#include <vsl/vsl_binary_io.h>

template < vxl::indexsize_t dim>
class bvpl_pca_basis_sample{

public:
  bvpl_pca_basis_sample(){
    pca_projections_.fill(0);
  }

  bvpl_pca_basis_sample(vnl_vector_fixed<double,dim> pp){
    pca_projections_ = pp;
  }
  inline short version_no() const{ return 1; }

  //: Vector of projections on pca space
  vnl_vector_fixed<double,dim> pca_projections_;

};


template < vxl::indexsize_t dim>
std::ostream& operator << (std::ostream& os, const bvpl_pca_basis_sample<dim>& sample);

template < vxl::indexsize_t dim>
void vsl_b_write(vsl_b_ostream & os, bvpl_pca_basis_sample<dim> const &sample);

template < vxl::indexsize_t dim>
void vsl_b_write(vsl_b_ostream & os, bvpl_pca_basis_sample<dim> const * &sample);

template < vxl::indexsize_t dim>
void vsl_b_read(vsl_b_istream & is, bvpl_pca_basis_sample<dim> &sample);

template < vxl::indexsize_t dim>
void vsl_b_read(vsl_b_istream & is, bvpl_pca_basis_sample<dim> *&sample);

#endif
