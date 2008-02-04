// This is brl/bbas/bsta/io/bsta_io_gaussian_x_y_theta.h
#ifndef bsta_io_gaussian_x_y_theta_h_
#define bsta_io_gaussian_x_y_theta_h_
//:
// \file
// \brief Binary I/O for full gaussians
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 3/28/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bsta/bsta_gaussian_x_y_theta.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector_fixed.h>
#include <vnl/io/vnl_io_matrix_fixed.h>

//: Binary save bsta_gaussian_x_y_theta to stream.

void
vsl_b_write(vsl_b_ostream &os, const bsta_gaussian_x_y_theta& g)
{
  vsl_b_write(os,g.mean());
  vsl_b_write(os,g.covar());
}

//: Binary load bsta_gaussian_full from stream.
void
vsl_b_read(vsl_b_istream &is, bsta_gaussian_x_y_theta& g)
{
  vnl_vector_fixed<float,3> mean;
  vnl_matrix_fixed<float,3,3> covar;
  vsl_b_read(is, mean);
  vsl_b_read(is, covar);
  g.set_mean(mean);
  g.set_covar(covar);
}

//: Print summary
void
vsl_print_summary(vcl_ostream &os, const bsta_gaussian_x_y_theta & g)
{
  os << "Gaussian (full) mean:"<<g.mean()<<" covar:"<<g.covar();
}

#endif // bsta_io_gaussian_x_y_theta_h_
