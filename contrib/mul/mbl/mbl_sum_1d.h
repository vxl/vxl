#ifndef mbl_sum_1d_h_
#define mbl_sum_1d_h_

//:
// \file
// \brief Computes running sum of 1D variable, so mean accessible.
// \author Tim Cootes

#include <vcl_iosfwd.h>
#include <vsl/vsl_binary_io.h>

//: Computes running sum of 1D variable, so mean accessible.
// \code
//   // A rather trivial example
//   mbl_sum_1d stats,stats2;
//
//   const int n = 10;
//   for (int i=0;i<n;i++)
//   {
//     stats.obs(i);
//     stats.obs(i+10);
//   }
//
//   vcl_cout<<stats;
//   vcl_cout<<"Mean of numbers 0..9:"<<stats.mean()<<vcl_endl;
//   mbl_sum_1d stats3 = stats+stats2;
//   vcl_cout<<"Mean of numbers 0..19:"<<stats3.mean()<<vcl_endl;
// \endcode
class mbl_sum_1d
{
  double sum_;
  int n_obs_;
 public:
  mbl_sum_1d() ;

    //: Remove all data
  void clear();

    //: Add given observation
  inline void obs(double v) { sum_+=v; n_obs_++; }

    //: Number of observations
  int nObs() const { return n_obs_; }

    //: Mean of current observations
  double mean() const { return n_obs_>0 ? sum_/n_obs_ : 0; }

    //: Sum of current observations
  double sum() const { return sum_; }

    //: Add statistics together
  mbl_sum_1d& operator+=(const mbl_sum_1d& s1);


  void print_summary(vcl_ostream& os) const;

    //: Version number for I/O
  short version_no() const;
  void b_write(vsl_b_ostream& bfs) const;
  void b_read(vsl_b_istream& bfs);

    //: Test for equality
  bool operator==(const mbl_sum_1d& s) const;

  friend
  mbl_sum_1d operator+(const mbl_sum_1d& s1, const mbl_sum_1d& s2);
};

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_sum_1d& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_sum_1d& b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const mbl_sum_1d& stats);

//: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const mbl_sum_1d& stats);

#endif
