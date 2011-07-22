#ifndef bbas_1d_array_float_h
#define bbas_1d_array_float_h
//:
// \file
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vbl/vbl_array_1d.h>
#include <vsl/vsl_binary_io.h>

class bbas_1d_array_float: public vbl_ref_count
{
 public:
  bbas_1d_array_float(unsigned n):data_array(n,-1.0f){}
  vbl_array_1d<float> data_array;
};

typedef vbl_smart_ptr<bbas_1d_array_float> bbas_1d_array_float_sptr;

//: Binary write array float to stream
void vsl_b_write(vsl_b_ostream & os, bbas_1d_array_float const& a);

//: Binary write  array float pointer to stream
void vsl_b_write(vsl_b_ostream& os, const bbas_1d_array_float* &b);

//: Binary load array float pointer from stream.
void vsl_b_read(vsl_b_istream & is, bbas_1d_array_float &a);

//: Binary load array float pointer from stream.
void vsl_b_read(vsl_b_istream& is, bbas_1d_array_float* b);


#endif
