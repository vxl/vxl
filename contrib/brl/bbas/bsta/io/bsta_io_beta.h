// This is brl/bbas/bsta/io/bsta_io_beta.h
#ifndef bsta_io_beta_h_
#define bsta_io_beta_h_
//:
// \file
// \brief Binary I/O for full betas
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date March 28, 2006
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <bsta/bsta_beta.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector_fixed.h>
#include <vnl/io/vnl_io_matrix_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Binary save bsta_beta to stream.
template <class T>
void
vsl_b_write(vsl_b_ostream &os, const bsta_beta<T>& g)
{
  vsl_b_write(os,g.alpha());
  vsl_b_write(os,g.beta());
}

//: Binary load bsta_beta from stream.
template <class T>
void
vsl_b_read(vsl_b_istream &is, bsta_beta<T>& g)
{
  T alpha;
  T beta;
  vsl_b_read(is, alpha);
  vsl_b_read(is, beta);
  g.set_alpha_beta(alpha, beta);
}

//: Print summary
template <class T>
void
vsl_print_summary(std::ostream &os, const bsta_beta<T>& g)
{
  os << "Beta - alpha:"<<g.alpha()<<" beta:"<<g.beta();
}


#endif // bsta_io_beta_h_
