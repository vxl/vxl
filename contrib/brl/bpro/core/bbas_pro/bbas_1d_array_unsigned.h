#ifndef bbas_1d_array_unsigned_h
#define bbas_1d_array_unsigned_h
//:
// \file
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vbl/vbl_array_1d.h>
#include <vsl/vsl_binary_io.h>

class bbas_1d_array_unsigned: public vbl_ref_count
{
 public:
  bbas_1d_array_unsigned(unsigned n) : data_array(n, (unsigned)(-1)) {}
  vbl_array_1d<unsigned> data_array;
};

typedef vbl_smart_ptr<bbas_1d_array_unsigned> bbas_1d_array_unsigned_sptr;

//: Binary write array float to stream
void vsl_b_write(vsl_b_ostream & os, bbas_1d_array_unsigned const& a);

//: Binary write  array float pointer to stream
void vsl_b_write(vsl_b_ostream& os, const bbas_1d_array_unsigned* &b);

//: Binary load array float pointer from stream.
void vsl_b_read(vsl_b_istream & is, bbas_1d_array_unsigned &a);

//: Binary load array float pointer from stream.
void vsl_b_read(vsl_b_istream& is, bbas_1d_array_unsigned* b);


#endif
