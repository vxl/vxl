#ifndef mbl_histogram_h_
#define mbl_histogram_h_

//:
// \file
// \brief Simple object to build histogram from supplied data.
// \author Tim Cootes

#include <vcl_iosfwd.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_vector.h>

//: Simple object to build histogram from supplied data.
//  Assumes all 1D data can be cast to doubles
//  One could perhaps re-write this, templated by object type, if really necessary.
class mbl_histogram
{
  //: Position of limits of each bin.
  //  Bin i is [bins_[i],bins_[i+1])
  vcl_vector<double> bins_;

  //: Number in each bin
  vcl_vector<int> freq_;

  //: Number below lowest bin
  int n_below_;

  //: Number above highest bin
  int n_above_;

  //: Total number of examples supplied
  int n_obs_;
 public:
  //: Construct with no bins
  mbl_histogram();

  //: Construct with given number of bins over given range
  mbl_histogram(double x_lo, double x_hi, int n_bins);

  //: Define number and size of bins
  void set_bins(double x_lo, double x_hi, int n_bins);

  //: Remove all data
  void clear();

  //: Add given observation
  void obs(double v);

  //: Number of bins
  int n_bins() const { return freq_.size(); }

  //: Position of limits of each bin.
  //  Bin i is [bins_[i],bins_[i+1])
  const vcl_vector<double>& bins() const { return bins_; }

  //: Number of observations
  int n_obs() const { return n_obs_;}

  //: Number in each bin
  const vcl_vector<int>& frequency() const { return freq_; }

  //: Number below lowest bin
  int n_below() const { return n_below_; }

  //: Number above highest bin
  int n_above() const { return n_above_; }

  //: Write out probabilities (freq/n_obs) to a named file
  //  Can then be plotted by your favorite tool
  //
  //  Format: (bin-centre) prob     (one per line)
  // \return true if successful
  bool write_probabilities(const char* path);

  void print_summary(vcl_ostream& os) const;
  //: Version number for I/O
  short version_no() const;
  void b_write(vsl_b_ostream& bfs) const;
  void b_read(vsl_b_istream& bfs);

  //: Test for equality
  bool operator==(const mbl_histogram& s) const;
};

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_histogram& histo);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_histogram& histo);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os, const mbl_histogram& histo);

//: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os, const mbl_histogram& histo);

#endif
