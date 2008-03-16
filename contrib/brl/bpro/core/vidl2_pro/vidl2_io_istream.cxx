//:
// \file
#include <vsl/vsl_binary_io.h>
#include <vidl2/vidl2_istream.h>
#include <core/vidl2_pro/vidl2_io_istream.h>
#include <vcl_iostream.h>


//: Binary write vidl2 istream to stream
void vsl_b_write(vsl_b_ostream & os, vidl2_istream_sptr const& view_base)
{
  vcl_cerr << "warning: vsl_b_write not implemented for vil_istream_sptr\n";
}

//: Binary load vidl2 istream from stream.
void vsl_b_read(vsl_b_istream & is, vidl2_istream_sptr &view_base)
{
  vcl_cerr << "warning: vsl_b_read not implemented for vil_istream_sptr\n";
}
