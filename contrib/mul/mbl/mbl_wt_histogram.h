#ifndef mbl_wt_histogram_h_
#define mbl_wt_histogram_h_

//:
// \file
// \brief Simple object to build histogram from supplied data, with weights
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>

//: Simple object to build histogram from supplied data, with weights
//  Each observation is supplied with a weight.  Record total weight
//  in each bin of the histogram.
class mbl_wt_histogram
{
  //: Bin i is given by [xlo_+i*dx_,xlo_+(i+1)*dx_)
  double xlo_;

  //: Bin width. Bin i is given by [xlo_+i*dx_,xlo_+(i+1)*dx_)
  double dx_;

  //: Total weight in each bin
  std::vector<double> wt_sum_;

  //: Number below lowest bin
  double wt_below_;

  //: Number above highest bin
  double wt_above_;

  //: Total number of examples supplied
  int n_obs_;

  //: Total sum of weights supplied
  double total_wt_;
 public:
  //: Construct with no bins
  mbl_wt_histogram();

  //: Construct with given number of bins over given range
  mbl_wt_histogram(double x_lo, double x_hi, int n_bins);

  //: Define number and size of bins
  void set_bins(double x_lo, double x_hi, int n_bins);

  //: Remove all data
  void clear();

  //: Add given observation
  void obs(double v, double wt);

  //: Number of bins
  int n_bins() const { return wt_sum_.size(); }

  //: Bin width. Bin i is given by [bin_t0()+i*dx(),bin_t0()+(i+1)*dx())
  double xlo() const { return xlo_; }

  //: Bin width. Bin i is given by [bin_t0()+i*dx(),bin_t0()+(i+1)*dx())
  double dx() const { return dx_; }

  //: Number of observations
  int n_obs() const { return n_obs_;}

  //: Total sum of weights supplied
  double total_wt() const { return total_wt_; }

  //: Total weight in each bin
  const std::vector<double>& wt_sum() const { return wt_sum_; }

  //: Total weight below lowest bin (bin_t0())
  double wt_below() const { return wt_below_; }

  //: Total weight above highest bin (bin_t0()+(n_bins()-1) * dx())
  double wt_above() const { return wt_above_; }

  //: Write out probabilities (freq/wt) to a named file
  //  Can then be plotted by your favorite tool
  //
  //  Format: (bin-centre) prob     (one per line)
  // \return true if successful
  bool write_probabilities(const char* path);

  void print_summary(std::ostream& os) const;
  //: Version number for I/O
  short version_no() const;
  void b_write(vsl_b_ostream& bfs) const;
  void b_read(vsl_b_istream& bfs);

  //: Test for equality
  bool operator==(const mbl_wt_histogram& s) const;
};

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_wt_histogram& histo);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_wt_histogram& histo);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os, const mbl_wt_histogram& histo);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os, const mbl_wt_histogram& histo);

#endif
