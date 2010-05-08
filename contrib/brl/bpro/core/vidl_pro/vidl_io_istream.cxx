#include "vidl_io_istream.h"
//:
// \file
#include <vsl/vsl_binary_io.h>
#include <vidl/vidl_istream.h>
#include <vcl_iostream.h>


//: Binary write vidl istream to stream
void vsl_b_write(vsl_b_ostream & /*os*/, vidl_istream_sptr const& /*view_base*/)
{
  vcl_cerr << "warning: vsl_b_write not implemented for vil_istream_sptr\n";
}

//: Binary load vidl istream from stream.
void vsl_b_read(vsl_b_istream & /*is*/, vidl_istream_sptr & /*view_base*/)
{
  vcl_cerr << "warning: vsl_b_read not implemented for vil_istream_sptr\n";
}
