#ifndef vidl2_io_ostream_base_h_
#define vidl2_io_ostream_base_h_

#include <vsl/vsl_binary_io.h>

#include <vidl2/vidl2_ostream_sptr.h>



//: Binary write ostream
void vsl_b_write(vsl_b_ostream & os, vidl2_ostream_sptr const& ostream);


//: Binary load ostream
void vsl_b_read(vsl_b_istream & is, vidl2_ostream_sptr& ostream);


#endif
