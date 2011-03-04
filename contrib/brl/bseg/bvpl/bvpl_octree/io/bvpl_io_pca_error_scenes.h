// This is bvpl_io_pca_error_scenes.h
#ifndef bvpl_io_pca_error_scenes_h
#define bvpl_io_pca_error_scenes_h

//:
// \file
// \brief Binary IO for bvpl_pca_error_scenes. Not implemented, only declared so it can be stored in the database
// \author Isabel Restrepo mir@lems.brown.edu
// \date  15-Feb-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/bvpl_octree/bvpl_pca_error_scenes.h>

#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>

//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bvpl_kernel const &bvpl_pca_error_scenes);


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bvpl_pca_error_scenes &m);


void vsl_print_summary(vcl_ostream &os, const bvpl_pca_error_scenes &m);


void vsl_b_read(vsl_b_istream& is, bvpl_pca_error_scenes* m);


void vsl_b_write(vsl_b_ostream& os, const bvpl_pca_error_scenes* &m);

void vsl_print_summary(vcl_ostream& os, const bvpl_pca_error_scenes* &m);


#endif
