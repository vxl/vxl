#include <vsl/vsl_binary_io.h>
#include <vidl2/vidl2_ostream.h>
#include <core/vidl2_pro/vidl2_io_ostream.h>


//: Binary write vidl2 ostream to stream
void vsl_b_write(vsl_b_ostream & os, vidl2_ostream_sptr const& view_base)
{
  vcl_cerr << "warning: vsl_b_write not implemented for vil_ostream_sptr" << vcl_endl;

}


//: Binary load vidl2 ostream from stream.
void vsl_b_read(vsl_b_istream & is, vidl2_ostream_sptr &view_base)
{
  vcl_cerr << "warning: vsl_b_read not implemented for vil_ostream_sptr" << vcl_endl;

}
