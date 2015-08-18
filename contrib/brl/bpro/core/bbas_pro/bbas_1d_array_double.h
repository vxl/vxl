#ifndef bbas_1d_array_double_h
#define bbas_1d_array_double_h
//:
// \file
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vbl/vbl_array_1d.h>
#include <vsl/vsl_binary_io.h>

class bbas_1d_array_double: public vbl_ref_count
{
 public:
  bbas_1d_array_double(unsigned n):data_array(n,-1.0f){}
  vbl_array_1d<double> data_array;
};

typedef vbl_smart_ptr<bbas_1d_array_double> bbas_1d_array_double_sptr;

//: Binary write array double to stream
void vsl_b_write(vsl_b_ostream & os, bbas_1d_array_double const& a);

//: Binary write  array double pointer to stream
void vsl_b_write(vsl_b_ostream& os, const bbas_1d_array_double* &b);

//: Binary load array double pointer from stream.
void vsl_b_read(vsl_b_istream & is, bbas_1d_array_double &a);

//: Binary load array double pointer from stream.
void vsl_b_read(vsl_b_istream& is, bbas_1d_array_double* b);


#endif
