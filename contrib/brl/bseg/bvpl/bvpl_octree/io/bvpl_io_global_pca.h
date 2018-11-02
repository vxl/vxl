// This is brl/bseg/bvpl/bvpl_octree/io/bvpl_io_global_pca.h
#ifndef bvpl_io_global_pca_h
#define bvpl_io_global_pca_h
//:
// \file
// \brief Binary IO for bvpl_global_pca_125. Not implemented, only declared so it can be stored in the database
// \author Isabel Restrepo mir@lems.brown.edu
// \date  15-Feb-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <bvpl/bvpl_octree/bvpl_global_pca.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>

typedef bvpl_global_pca<125> bvpl_global_pca_125;

//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bvpl_global_pca_125 const &);


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bvpl_global_pca_125 &m);


void vsl_print_summary(std::ostream &os, const bvpl_global_pca_125 &m);


void vsl_b_read(vsl_b_istream& is, bvpl_global_pca_125* m);


void vsl_b_write(vsl_b_ostream& os, const bvpl_global_pca_125* &m);

void vsl_print_summary(std::ostream& os, const bvpl_global_pca_125* &m);


#endif
