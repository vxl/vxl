// This is brl/bbas/bsta/io/bsta_io_gaussian_sphere.h
#ifndef bsta_io_gaussian_sphere_h_
#define bsta_io_gaussian_sphere_h_

//:
// \file
// \brief Binary I/O for spherical gaussians
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 3/28/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bsta/bsta_gaussian_sphere.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector_fixed.h>

//: Binary save bsta_gaussian_sphere to stream.
template <class T, unsigned n>
void
vsl_b_write(vsl_b_ostream &os, const bsta_gaussian_sphere<T,n>& g)
{
  vsl_b_write(os,g.mean());
  vsl_b_write(os,g.var());
}

//: Binary load bsta_gaussian_sphere from stream.
template <class T, unsigned n>
void
vsl_b_read(vsl_b_istream &is, bsta_gaussian_sphere<T,n>& g)
{
  typedef typename bsta_gaussian_sphere<T,n>::vector_type vector_;
  vector_ mean;
  T var;
  vsl_b_read(is, mean);
  vsl_b_read(is, var);
  g.set_mean(mean);
  g.set_var(var);
}

//: Print summary
template <class T, unsigned n>
void
vsl_print_summary(vcl_ostream &os, const bsta_gaussian_sphere<T,n>& g)
{
  os << "Gaussian (sphere) mean:"<<g.mean()<<" var:"<<g.var();
}


#endif // bsta_io_gaussian_sphere_h_
