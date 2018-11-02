#ifndef mbl_stats_1d_h_
#define mbl_stats_1d_h_

//:
// \file
// \brief Simple statistics on a 1D variable.
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <vector>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>

// windows thinks min and max are macros in this file, but they
// are not, don't know where they are defined (somewhere in vxl!)
#undef min
#undef max

//: Simple statistics on a 1D variable
// \code
//   // A rather trivial example
//   mbl_stats_1d stats,odd_stats,even_stats,sum_stats;
//
//   const int n = 10;
//   for (unsigned i=0;i<n;i++)
//   {
//     stats.obs(i);
//     if (i%2) even_stats.obs(i);
//     else     odd_stats.obs(i);
//   }
//
//   std::cout << stats << "\nStats of odd numbers :\n" << odd_stats;
//
//   sum_stats = odd_stats + even_stats;
//
//   std::cout << "Sum of odd and even stats\n" << sum_stats;
// \endcode
class mbl_stats_1d
{
  double sum_;
  double sum_sq_;
  double min_v_;
  double max_v_;
  unsigned n_obs_;
  double w_obs_;


public:

  //: Default constructor
  mbl_stats_1d();

  //: Construct with a set of observations
  mbl_stats_1d(const std::vector<double>& observations);

    //: Remove all data
  void clear();

    //: Add given observation
  void obs(double v);

    //: Add given weighted observation
    // \p weight doesn't affect the max or min records.
  void obs(double v, double weight);

    //: Number of discrete observations.
  int nObs() const { return n_obs_; }

    //: Weight of all observations
  double wObs() const { return w_obs_; }

    //: Mean of current observations
  double mean() const ;


    //: Standard deviation of current observations
  double sd() const;
    //: Standard error (sd of estimate of mean) of current observations
  double stdError() const;
    //: Variance of current observations
  double variance() const;
    //: Min of current observations
  double min() const;
    //: Max of current observations
  double max() const;
    //: Sum of current observations
  double sum() const;
    //: Sum of squares of current observations
  double sumSq() const;

  //: RMS of current observations;
  // \note If nobs==0, returns -1.0
  double rms() const;

    //: Add statistics together
  mbl_stats_1d& operator+=(const mbl_stats_1d& s1);
  void print_summary(std::ostream& os) const;
    //: Version number for I/O
  void b_write(vsl_b_ostream& bfs) const;
  void b_read(vsl_b_istream& bfs);

    //: Test for equality
  bool operator==(const mbl_stats_1d& s) const;

  friend
  mbl_stats_1d operator+(const mbl_stats_1d& s1, const mbl_stats_1d& s2);
};

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_stats_1d& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_stats_1d& b);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const mbl_stats_1d& stats);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const mbl_stats_1d& stats);

#endif
