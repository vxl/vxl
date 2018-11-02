// This is bvpl_io_global_taylor.h
#ifndef bvpl_io_global_taylor_h
#define bvpl_io_global_taylor_h

//:
// \file
// \brief Binary IO for bvpl_global_taylor. Not implemented, only declared so it can be stored in the database
// \author Isabel Restrepo mir@lems.brown.edu
// \date  15-Feb-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <bvpl/bvpl_octree/bvpl_global_taylor.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>


//: Binary save parameters to stream.
template <class T, unsigned N>
void vsl_b_write(vsl_b_ostream & os, bvpl_global_taylor<T,N> const &)
{
  std::cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

//: Binary load parameters from stream.
template <class T, unsigned N>
void vsl_b_read(vsl_b_istream & is, bvpl_global_taylor<T,N> &p)
{
  std::cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

template <class T, unsigned N>
void vsl_print_summary(std::ostream &os, const bvpl_global_taylor<T,N> &p)
{
  std::cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}


template <class T, unsigned N>
void vsl_b_read(vsl_b_istream& is, bvpl_global_taylor<T,N>* p)
{
  std::cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}


template <class T, unsigned N>
void vsl_b_write(vsl_b_ostream& os, const bvpl_global_taylor<T,N>* &mp)
{
  std::cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

template <class T, unsigned N>
void vsl_print_summary(std::ostream& os, const bvpl_global_taylor<T,N>* &p)
{
  if (p==0)
    os << "NULL PTR";
  else {
    os << "T: ";
    vsl_print_summary(os, *p);
  }
}

#endif
