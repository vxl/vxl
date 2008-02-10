#ifndef vidl2_io_istream_base_h_
#define vidl2_io_istream_base_h_

#include <vsl/vsl_binary_io.h>

#include <vidl2/vidl2_istream_sptr.h>



//: Binary write istream
void vsl_b_write(vsl_b_ostream & os, vidl2_istream_sptr const& istream);


//: Binary load istream
void vsl_b_read(vsl_b_istream & is, vidl2_istream_sptr& istream);


#endif
