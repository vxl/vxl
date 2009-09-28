// This is//projects/vxl/src/contrib/brl/bseg/bvpl/util/io/bvpl_corner_pairs_io.h
#ifndef bvpl_corner_pairs_io_h
#define bvpl_corner_pairs_io_h

//:
// \file
// \brief 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  9/27/09
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>
#include <bvpl/util/bvpl_corner_pair_finder.h>


// Binary io, NOT IMPLEMENTED, signatures defined to use as a brdb_value

//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bvpl_corner_pairs const &kernel);


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bvpl_corner_pairs &k);


void vsl_print_summary(vcl_ostream &os, const bvpl_corner_pairs &k);


void vsl_b_read(vsl_b_istream& is, bvpl_corner_pairs* k);


void vsl_b_write(vsl_b_ostream& os, const bvpl_corner_pairs* &k);

void vsl_print_summary(vcl_ostream& os, const bvpl_corner_pairs* &k);


#endif
