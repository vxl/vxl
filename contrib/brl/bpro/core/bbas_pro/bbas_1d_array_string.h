#ifndef bbas_1d_array_string_h
#define bbas_1d_array_string_h
//:
// \file
#include <iostream>
#include <string>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vbl/vbl_array_1d.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class bbas_1d_array_string: public vbl_ref_count
{
 public:
  bbas_1d_array_string(unsigned n):data_array(n, ""){}
  vbl_array_1d<std::string> data_array;
};

typedef vbl_smart_ptr<bbas_1d_array_string> bbas_1d_array_string_sptr;

//: Binary write to stream
void vsl_b_write(vsl_b_ostream & os, bbas_1d_array_string const& a);

//: Binary write pointer to stream
void vsl_b_write(vsl_b_ostream& os, const bbas_1d_array_string_sptr &aptr);

//: Binary load from stream.
void vsl_b_read(vsl_b_istream & is, bbas_1d_array_string &a);

//: Binary load pointer from stream.

void vsl_b_read(vsl_b_istream& is, bbas_1d_array_string* aptr);
void vsl_b_read(vsl_b_istream& is, bbas_1d_array_string_sptr& aptr);

#include <bpro/core/bbas_pro/bbas_1d_array_string_sptr.h>
#endif
