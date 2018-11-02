// This is brl/bseg/bvpl/kernels/io/bvpl_io_kernel.h
#ifndef bvpl_kernel_io_h
#define bvpl_kernel_io_h
//:
// \file
// \brief Binary io for kernel and kernel_vector
// \author Isabel Restrepo mir@lems.brown.edu
// \date  June 23, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bvpl/kernels/bvpl_kernel_factory.h>


// Binary io, NOT IMPLEMENTED, signatures defined to use as a brdb_value

//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bvpl_kernel const &kernel);


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bvpl_kernel &k);


void vsl_print_summary(std::ostream &os, const bvpl_kernel &k);


void vsl_b_read(vsl_b_istream& is, bvpl_kernel* k);


void vsl_b_write(vsl_b_ostream& os, const bvpl_kernel* &k);

void vsl_print_summary(std::ostream& os, const bvpl_kernel* &k);


/******************bvpl_kernel_vector**************************/

//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bvpl_kernel_vector const &kernel);


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bvpl_kernel_vector &k);


void vsl_print_summary(std::ostream &os, const bvpl_kernel_vector &k);


void vsl_b_read(vsl_b_istream& is, bvpl_kernel_vector* k);


void vsl_b_write(vsl_b_ostream& os, const bvpl_kernel_vector* &k);

void vsl_print_summary(std::ostream& os, const bvpl_kernel_vector* &k);

#endif
