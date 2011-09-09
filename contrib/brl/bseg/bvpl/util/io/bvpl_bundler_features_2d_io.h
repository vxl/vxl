//this is /contrib/brl/bseg/util/io/bvpl_bundler_features_2d_io.h
#ifndef BVPL_BUNDLER_FEATURES_2D_IO_H_
#define BVPL_BUNDLER_FEATURES_2D_IO_H_
//:
// \file
// \brief 
// \author Brandon A. Mayer b.mayer1@gmail.com
// \date  9/8/11
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/util/bvpl_bundler_features_2d.h>

#include <vcl_iostream.h>

#include <vsl/vsl_binary_io.h>

//: Binary save parameters to stream.
void vsl_b_read(vsl_b_istream &is, bvpl_bundler_features_2d& bundler_features);

void vsl_b_write(vsl_b_ostream &os, bvpl_bundler_features_2d const& bundler_features);

void vsl_b_read(vsl_b_istream &is, bvpl_bundler_features_2d* &p);

void vsl_b_write(vsl_b_ostream &os, const bvpl_bundler_features_2d* p);

void vsl_print_summary(vcl_ostream& os, const bvpl_bundler_features_2d *p);

#endif //BVPL_BUNDLER_FEATURES_2D_IO_H_
