//:
// \file
// \author Isabel Restrepo
// \date 15-Feb-2011

#include "bvpl_io_pca_error_scenes.h"


//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & /*os*/, bvpl_pca_error_scenes const & )
{
  std::cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & /*is*/, bvpl_pca_error_scenes & )
{
  std::cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

void vsl_print_summary(std::ostream & /*os*/, const bvpl_pca_error_scenes & )
{
  std::cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

void vsl_b_read(vsl_b_istream&  /*is*/,bvpl_pca_error_scenes*  /*p*/)
{
  std::cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

void vsl_b_write(vsl_b_ostream&  /*os*/, const bvpl_pca_error_scenes* & /*p*/)
{
  std::cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

void vsl_print_summary(std::ostream& os, const bvpl_pca_error_scenes* &p)
{
  if (p==nullptr)
    os << "NULL PTR";
  else {
    os << "T: ";
    vsl_print_summary(os, *p);
  }
}
