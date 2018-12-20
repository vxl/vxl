#ifndef mbl_mv_stats_h_
#define mbl_mv_stats_h_

//:
// \file
// \brief Compute multivariate stat.s (mean, covar)
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <vcl_compiler.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

//: Compute multivariate stat.s (mean, covar).
//  Collect data by repeatedly calling add_sample(vec).
//  Keeps a running total of sum and cross products, from
//  which the mean and covariance can be computed.
class mbl_mv_stats
{
  vnl_vector<double> sum_;
  vnl_matrix<double> sum2_;
  unsigned n_obs_;
 public:
  mbl_mv_stats() ;

  //: Remove all data
  void clear();

  //: Add given observation
  void add_sample(const vnl_vector<double>& v);

  //: Number of observations
  unsigned n_obs() const { return n_obs_; }

  //: Mean of current observations
  vnl_vector<double> mean() const ;

  //: Standard deviation of current observations
  vnl_vector<double> sd() const;

  //: Standard error (sd of estimate of mean) of current observations
  vnl_vector<double> stdError() const;

  //: Variance of current observations
  vnl_vector<double> variance() const;

  //: Covariance matrix of samples added so far
  vnl_matrix<double> covar() const;

  //: Get the n_modes eigenvectors associated with the largest eigenvalues of covar()
  void get_eigen(vnl_matrix<double>& EVecs, vnl_vector<double>& evals, int n_modes=-1) const;


  //: Sum of current observations
  const vnl_vector<double>& sum() const { return sum_; }

  //: Sum of outer products of current observations
  const vnl_matrix<double> & sum2() const { return sum2_; }

  //: Add statistics together
  mbl_mv_stats& operator+=(const mbl_mv_stats& s1);

  void print_summary(std::ostream& os) const;

  void b_write(vsl_b_ostream& bfs) const;
  void b_read(vsl_b_istream& bfs);

  //: Test for equality
  bool operator==(const mbl_mv_stats& s) const;

  friend
  mbl_mv_stats operator+(const mbl_mv_stats& s1, const mbl_mv_stats& s2);
};

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_mv_stats& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_mv_stats& b);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const mbl_mv_stats& stats);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const mbl_mv_stats& stats);

#endif
