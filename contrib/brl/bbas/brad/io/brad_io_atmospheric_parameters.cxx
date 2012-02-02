#include "brad_io_atmospheric_parameters.h"
//:
// \file
#include <brad/brad_atmospheric_parameters.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>

//: Binary save brad_atmospheric_parameters to stream.
void
vsl_b_write(vsl_b_ostream &os, const brad_atmospheric_parameters& m)
{
}

//: Binary load brad_atmospheric_parameters from stream.
void
vsl_b_read(vsl_b_istream &is, brad_atmospheric_parameters& m)
{
}

//: Print summary
void
vsl_print_summary(vcl_ostream &os, const brad_atmospheric_parameters& m)
{
  os << "brad_atmospheric_parameters ";
}

//: Binary write brad_atmospheric_parameters to stream
void vsl_b_write(vsl_b_ostream& os, const brad_atmospheric_parameters* &p){}
//: Binary write brad_atmospheric_parameters to stream
void vsl_b_write(vsl_b_ostream& os, brad_atmospheric_parameters_sptr& sptr){}
//: Binary write brad_atmospheric_parameters to stream
void vsl_b_write(vsl_b_ostream& os, brad_atmospheric_parameters_sptr const& sptr){}

//: Binary load brad_atmospheric_parameters scene from stream.
void vsl_b_read(vsl_b_istream& is, brad_atmospheric_parameters* p){}
//: Binary load brad_atmospheric_parameters scene from stream.
void vsl_b_read(vsl_b_istream& is, brad_atmospheric_parameters_sptr& sptr){}
//: Binary load brad_atmospheric_parameters scene from stream.
void vsl_b_read(vsl_b_istream& is, brad_atmospheric_parameters_sptr const& sptr){}

