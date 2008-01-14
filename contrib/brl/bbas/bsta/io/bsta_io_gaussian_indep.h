// This is brcv/seg/bsta/io/dbsta_io_gaussian_indep.h
#ifndef dbsta_io_gaussian_indep_h_
#define dbsta_io_gaussian_indep_h_

//:
// \file
// \brief Binary I/O for independent gaussians
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 3/28/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bsta/bsta_gaussian_indep.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector_fixed.h>

//: Binary save bsta_gaussian_indep to stream.
template <class T, unsigned n>
void
vsl_b_write(vsl_b_ostream &os, const bsta_gaussian_indep<T,n>& g)
{
  vsl_b_write(os,g.mean());
  vsl_b_write(os,g.diag_covar());
}

//: Binary load bsta_gaussian_indep from stream.
template <class T, unsigned n>
void
vsl_b_read(vsl_b_istream &is, bsta_gaussian_indep<T,n>& g)
{
  vnl_vector_fixed<T,n> mean, diag_covar;
  vsl_b_read(is, mean);
  vsl_b_read(is, diag_covar);
  g.set_mean(mean);
  g.set_covar(diag_covar);
}

//: Print summary
template <class T, unsigned n>
void
vsl_print_summary(vcl_ostream &os, const bsta_gaussian_indep<T,n>& g)
{
  os << "Gaussian (indep) mean:"<<g.mean()<<" diag_covar:"<<g.diag_covar();
}


#endif // dbsta_io_gaussian_indep_h_
