// This is bvpl_io_discover_pca_kernels.h
#ifndef bvpl_io_discover_pca_kernels_h
#define bvpl_io_discover_pca_kernels_h

//:
// \file
// \brief Binary IO for bvpl_discover_pca_kernels. Not implemented, only declared so it can be stored in the database
// \author Isabel Restrepo mir@lems.brown.edu
// \date  15-Feb-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <bvpl/bvpl_octree/bvpl_discover_pca_kernels.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>

//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bvpl_discover_pca_kernels const &);


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bvpl_discover_pca_kernels &m);


void vsl_print_summary(std::ostream &os, const bvpl_discover_pca_kernels &m);


void vsl_b_read(vsl_b_istream& is, bvpl_discover_pca_kernels* m);


void vsl_b_write(vsl_b_ostream& os, const bvpl_discover_pca_kernels* &m);

void vsl_print_summary(std::ostream& os, const bvpl_discover_pca_kernels* &m);


#endif
