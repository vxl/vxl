// This is brl/bbas/brad/io/brad_io_atmospheric_parameters.h
#ifndef brad_io_atmospheric_parameters_h_
#define brad_io_atmospheric_parameters_h_
//:
// \file
// \brief Binary I/O for atmospheric parameters
// \author D. E. Crispell
// \date January 27, 2012
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <brad/brad_atmospheric_parameters.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Binary save brad_atmospheric_parameters to stream.
void
vsl_b_write(vsl_b_ostream &os, const brad_atmospheric_parameters& m);

//: Binary load brad_atmospheric_parameters from stream.
void
vsl_b_read(vsl_b_istream &is, brad_atmospheric_parameters& m);

//: Print summary
void
vsl_print_summary(std::ostream &os, const brad_atmospheric_parameters& m);

//: Binary write brad_atmospheric_parameters to stream
void vsl_b_write(vsl_b_ostream& os, const brad_atmospheric_parameters* &p);
//: Binary write brad_atmospheric_parameters to stream
void vsl_b_write(vsl_b_ostream& os, brad_atmospheric_parameters_sptr& sptr);
//: Binary write brad_atmospheric_parameters to stream
void vsl_b_write(vsl_b_ostream& os, brad_atmospheric_parameters_sptr const& sptr);

//: Binary load brad_atmospheric_parameters scene from stream.
void vsl_b_read(vsl_b_istream& is, brad_atmospheric_parameters* p);
//: Binary load brad_atmospheric_parameters scene from stream.
void vsl_b_read(vsl_b_istream& is, brad_atmospheric_parameters_sptr& sptr);
//: Binary load brad_atmospheric_parameters scene from stream.
void vsl_b_read(vsl_b_istream& is, brad_atmospheric_parameters_sptr const& sptr);


#endif // brad_io_atmospheric_parameters_h_
