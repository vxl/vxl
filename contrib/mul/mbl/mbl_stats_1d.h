#ifndef mbl_stats_1d_h_
#define mbl_stats_1d_h_

//:
// \file
// \brief Simple statistics on a 1D variable.
// \author Tim Cootes

#include <vcl_iosfwd.h>
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
//   for (int i=0;i<n;i++)
//   {
//     stats.obs(i);
//     if (i%2) even_stats.obs(i);
//     else     odd_stats.obs(i);
//   }
//
//   vcl_cout << stats << "\nStats of odd numbers :\n" << odd_stats;
//
//   sum_stats = odd_stats + even_stats;
//
//   vcl_cout << "Sum of odd and even stats\n" << sum_stats;
// \endcode
class mbl_stats_1d
{
  double sum_;
  double sum_sq_;
  double min_v_;
  double max_v_;
  int n_obs_;
 public:
  mbl_stats_1d() ;

    //: Remove all data
  void clear();

    //: Add given observation
  void obs(double v);

    //: Number of observations
  int nObs() const;

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

    //: Add statistics together
  mbl_stats_1d& operator+=(const mbl_stats_1d& s1);
  void print_summary(vcl_ostream& os) const;
    //: Version number for I/O
  short version_no() const;
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
vcl_ostream& operator<<(vcl_ostream& os,const mbl_stats_1d& stats);

//: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const mbl_stats_1d& stats);

#endif
