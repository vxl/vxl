#ifndef vidl_io_istream_base_h_
#define vidl_io_istream_base_h_
//:
// \file

#include <vsl/vsl_binary_io.h>
#include <vidl/vidl_istream_sptr.h>

//: Binary write istream
void vsl_b_write(vsl_b_ostream & os, vidl_istream_sptr const& i_stream);


//: Binary load istream
void vsl_b_read(vsl_b_istream & is, vidl_istream_sptr& i_stream);

#endif // vidl_io_istream_base_h_
