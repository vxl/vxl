//:
// \file
// \author Isabel Restrepo
// \date 15-Feb-2011

#include "bvpl_io_taylor_scenes_map.h"


//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & /*os*/, bvpl_taylor_scenes_map const & )
{
  vcl_cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & /*is*/, bvpl_taylor_scenes_map & )
{
  vcl_cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

void vsl_print_summary(vcl_ostream & /*os*/, const bvpl_taylor_scenes_map & )
{
  vcl_cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

void vsl_b_read(vsl_b_istream& is,bvpl_taylor_scenes_map* p)
{
  vcl_cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

void vsl_b_write(vsl_b_ostream& os, const bvpl_taylor_scenes_map* &p)
{
  vcl_cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

void vsl_print_summary(vcl_ostream& os, const bvpl_taylor_scenes_map* &p)
{
  if (p==0)
    os << "NULL PTR";
  else {
    os << "T: ";
    vsl_print_summary(os, *p);
  }
}
