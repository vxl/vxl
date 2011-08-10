// This is bvpl_io_global_taylor.h
#ifndef bvpl_io_global_taylor_h
#define bvpl_io_global_taylor_h

//:
// \file
// \brief Binary IO for bvpl_global_taylor. Not implemented, only declared so it can be stored in the database
// \author Isabel Restrepo mir@lems.brown.edu
// \date  15-Feb-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/bvpl_octree/bvpl_global_taylor.h>

#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>

typedef bvpl_global_taylor bvpl_global_taylor;

//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bvpl_global_taylor const &);


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bvpl_global_taylor &m);


void vsl_print_summary(vcl_ostream &os, const bvpl_global_taylor &m);


void vsl_b_read(vsl_b_istream& is, bvpl_global_taylor* m);


void vsl_b_write(vsl_b_ostream& os, const bvpl_global_taylor* &m);

void vsl_print_summary(vcl_ostream& os, const bvpl_global_taylor* &m);


#endif
