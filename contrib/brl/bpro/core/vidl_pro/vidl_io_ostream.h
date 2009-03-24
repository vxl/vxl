#ifndef vidl_io_ostream_base_h_
#define vidl_io_ostream_base_h_
//:
// \file

#include <vsl/vsl_binary_io.h>
#include <vidl/vidl_ostream_sptr.h>

//: Binary write ostream
void vsl_b_write(vsl_b_ostream & os, vidl_ostream_sptr const& o_stream);

//: Binary load ostream
void vsl_b_read(vsl_b_istream & is, vidl_ostream_sptr& o_stream);

#endif // vidl_io_ostream_base_h_
