#ifndef mbl_stats_nd_h_
#define mbl_stats_nd_h_

//:
// \file
// \brief Simple statistics (mean, variance) on vectors.
// \author Tim Cootes

#include <vcl_iosfwd.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/vnl_vector.h>

//: Simple statistics (mean, variance) on vectors.
//  Note: Uses unbiased estimate of variance (ie divide by (n_obs()-1))
class mbl_stats_nd
{
  vnl_vector<double> sum_;
  vnl_vector<double> sum_sq_;
  unsigned n_obs_;
 public:
  mbl_stats_nd() ;

  //: Remove all data
  void clear();

  //: Add given observation
  void obs(const vnl_vector<double>& v);

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

  //: Sum of current observations
  const vnl_vector<double>& sum() const { return sum_; }

  //: Sum of squares of current observations
  const vnl_vector<double> & sumSq() const { return sum_sq_; }

  //: Add statistics together
  mbl_stats_nd& operator+=(const mbl_stats_nd& s1);
  void print_summary(vcl_ostream& os) const;
  //: Version number for I/O
  short version_no() const;
  void b_write(vsl_b_ostream& bfs) const;
  void b_read(vsl_b_istream& bfs);

  //: Test for equality
  bool operator==(const mbl_stats_nd& s) const;

  friend
  mbl_stats_nd operator+(const mbl_stats_nd& s1, const mbl_stats_nd& s2);
};

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_stats_nd& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_stats_nd& b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const mbl_stats_nd& stats);

//: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const mbl_stats_nd& stats);

#endif
