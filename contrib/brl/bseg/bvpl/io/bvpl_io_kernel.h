// This is contrib/brl/bseg/bvpl/io/bvpl_kernel_io.h
#ifndef bvpl_kernel_io_h
#define bvpl_kernel_io_h

//:
// \file
// \brief 
//
// \author Isabel Restrepo mir@lems.brown.edu
//
// \date  6/23/09
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>
#include <bvpl/bvpl_kernel_factory.h>


// Binary io, NOT IMPLEMENTED, signatures defined to use as a brdb_value

//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bvpl_kernel const &kernel);


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bvpl_kernel &k);


void vsl_print_summary(vcl_ostream &os, const bvpl_kernel &k);


void vsl_b_read(vsl_b_istream& is, bvpl_kernel* k);


void vsl_b_write(vsl_b_ostream& os, const bvpl_kernel* &k);

void vsl_print_summary(vcl_ostream& os, const bvpl_kernel* &k);

void vsl_b_read(vsl_b_istream& is, bvpl_kernel_sptr k);


void vsl_b_write(vsl_b_ostream& os, const bvpl_kernel_sptr &k);

void vsl_print_summary(vcl_ostream& os, const bvpl_kernel_sptr &k);
#endif
