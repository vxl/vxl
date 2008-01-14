// This is brcv/seg/bsta/io/dbsta_io_gaussian_angles_1d.h
#ifndef dbsta_io_gaussian_angles_1d_h_
#define dbsta_io_gaussian_angles_1d_h_

//:
// \file
// \brief Binary I/O for full gaussians
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 3/28/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bsta/bsta_gaussian_angles_1d.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector_fixed.h>
#include <vnl/io/vnl_io_matrix_fixed.h>

//: Binary save bsta_gaussian_angles_1d to stream.

void
vsl_b_write(vsl_b_ostream &os, const bsta_gaussian_angles_1d& g)
{
  vsl_b_write(os,g.mean());
  vsl_b_write(os,g.var());
}

//: Binary load bsta_gaussian_full from stream.
void
vsl_b_read(vsl_b_istream &is, bsta_gaussian_angles_1d& g)
{
 float mean;
  float var;
  vsl_b_read(is, mean);
  vsl_b_read(is, var);
  g.set_mean(mean);
  g.set_var(var);
}

//: Print summary
void
vsl_print_summary(vcl_ostream &os, const bsta_gaussian_angles_1d & g)
{
  os << "Gaussian (full) mean:"<<g.mean()<<" var:"<<g.var();
}


#endif // dbsta_io_gaussian_full_h_
