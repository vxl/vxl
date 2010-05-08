#include "vidl_io_ostream.h"
//:
// \file
#include <vsl/vsl_binary_io.h>
#include <vidl/vidl_ostream.h>
#include <vcl_iostream.h>

//: Binary write vidl ostream to stream
void vsl_b_write(vsl_b_ostream & /*os*/, vidl_ostream_sptr const& /*view_base*/)
{
  vcl_cerr << "warning: vsl_b_write not implemented for vil_ostream_sptr\n";
}

//: Binary load vidl ostream from stream.
void vsl_b_read(vsl_b_istream & /*is*/, vidl_ostream_sptr & /*view_base*/)
{
  vcl_cerr << "warning: vsl_b_read not implemented for vil_ostream_sptr\n";
}
