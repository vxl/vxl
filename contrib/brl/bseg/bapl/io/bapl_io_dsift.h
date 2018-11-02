// This is brl/bseg/bapl/io/bapl_io_dsift.h
#ifndef BAPL_IO_DSIFT_H_
#define BAPL_IO_DSIFT_H_
//:
// \file
// \brief Binary IO for bapl_dsift class
//
// \author Brandon A. Mayer b.mayer1@gmail.com
//
// \date  May 16, 2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <iosfwd>
#include<bapl/bapl_dsift.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include<vsl/vsl_binary_io.h>

//: Binary read parameters from stream.
void vsl_b_read(vsl_b_istream &is, bapl_dsift &dsift);
//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream &os, bapl_dsift const& dsift);
//: Binary read parameters from stream.
void vsl_b_read(vsl_b_istream &is, bapl_dsift* &p);
//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream &os, const bapl_dsift* p);
void vsl_print_summary(std::ostream& os, const bapl_dsift *p);

#endif //BAPL_IO_DSIFT_H_
