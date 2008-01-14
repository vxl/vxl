// This is brcv/seg/bsta/io/dbsta_io_gaussian_full.h
#ifndef dbsta_io_gaussian_full_h_
#define dbsta_io_gaussian_full_h_

//:
// \file
// \brief Binary I/O for full gaussians
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 3/28/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bsta/bsta_gaussian_full.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector_fixed.h>
#include <vnl/io/vnl_io_matrix_fixed.h>

//: Binary save bsta_gaussian_full to stream.
template <class T, unsigned n>
void
vsl_b_write(vsl_b_ostream &os, const bsta_gaussian_full<T,n>& g)
{
  vsl_b_write(os,g.mean());
  vsl_b_write(os,g.covar());
}

//: Binary load bsta_gaussian_full from stream.
template <class T, unsigned n>
void
vsl_b_read(vsl_b_istream &is, bsta_gaussian_full<T,n>& g)
{
  vnl_vector_fixed<T,n> mean;
  vnl_matrix_fixed<T,n,n> covar;
  vsl_b_read(is, mean);
  vsl_b_read(is, covar);
  g.set_mean(mean);
  g.set_covar(covar);
}

//: Print summary
template <class T, unsigned n>
void
vsl_print_summary(vcl_ostream &os, const bsta_gaussian_full<T,n>& g)
{
  os << "Gaussian (full) mean:"<<g.mean()<<" covar:"<<g.covar();
}


#endif // dbsta_io_gaussian_full_h_
