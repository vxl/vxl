#include <iostream>
#include "vidl_io_ostream.h"
//:
// \file
#include <vsl/vsl_binary_io.h>
#include <vidl/vidl_ostream.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Binary write vidl ostream to stream
void vsl_b_write(vsl_b_ostream & /*os*/, vidl_ostream_sptr const& /*view_base*/)
{
  std::cerr << "warning: vsl_b_write not implemented for vil_ostream_sptr\n";
}

//: Binary load vidl ostream from stream.
void vsl_b_read(vsl_b_istream & /*is*/, vidl_ostream_sptr & /*view_base*/)
{
  std::cerr << "warning: vsl_b_read not implemented for vil_ostream_sptr\n";
}
