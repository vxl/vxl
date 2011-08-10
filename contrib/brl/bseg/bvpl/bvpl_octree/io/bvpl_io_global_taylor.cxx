//:
// \file
// \author Isabel Restrepo
// \date 21-Jul-2011

#include "bvpl_io_global_taylor.h"


//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & /*os*/, bvpl_global_taylor const & )
{
  vcl_cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & /*is*/, bvpl_global_taylor & )
{
  vcl_cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

void vsl_print_summary(vcl_ostream & /*os*/, const bvpl_global_taylor & )
{
  vcl_cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

void vsl_b_read(vsl_b_istream& is,bvpl_global_taylor* p)
{
  vcl_cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

void vsl_b_write(vsl_b_ostream& os, const bvpl_global_taylor* &p)
{
  vcl_cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

void vsl_print_summary(vcl_ostream& os, const bvpl_global_taylor* &p)
{
  if (p==0)
    os << "NULL PTR";
  else {
    os << "T: ";
    vsl_print_summary(os, *p);
  }
}
