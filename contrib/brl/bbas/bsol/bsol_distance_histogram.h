#ifndef bsol_distance_histogram_h_
#define bsol_distance_histogram_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief A distance histogram for vsol geometry (currently lines only)
//
//  This histogram is for detecting distance patterns in vsol geometry.
//  At present it implements only vsol_line_2d machinery.
//
// \verbatim
//  Modifications
//   Initial version May 15, 2002
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <iosfwd>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsol/vsol_line_2d_sptr.h>

class bsol_distance_histogram
{
 public:
  bsol_distance_histogram();
  bsol_distance_histogram(int nbins, double max_val);
  bsol_distance_histogram(int nbins,
                          std::vector<vsol_line_2d_sptr> const& lines);
  ~bsol_distance_histogram();
  // === accessors ===
  int n_bins() const {return bin_counts_.size();}
  std::vector<double>& counts() {return bin_counts_;}
  std::vector<double>& values() {return bin_values_;}
  std::vector<double>& weights() {return weights_;}
  std::vector<double> const& counts() const {return bin_counts_;}
  std::vector<double> const& values() const {return bin_values_;}
  std::vector<double> const& weights() const {return weights_;}

  //: update the histogram
  void up_count(const double value, const double count, const double weight = 1.0);

  //: find the first two (non-zero) distance peaks
  bool distance_peaks(double& peak1, double& peak2,
                      double min_peak_height_ratio = 0.5);


  //: utility functions
  double min_val() const;
  double max_val() const;
  double min_count() const;
  double max_count() const;

  friend
    std::ostream& operator<<(std::ostream& os, const bsol_distance_histogram& h);

 private:
  //: normalize the distance values which were weighted by the line length
  void normalize_distance();
  //: perform a parabolic interpolation using adjacent bins.
  double interpolate_peak(int initial_peak);
  double delta_; //!< bin value interval
  std::vector<double> bin_counts_;//!< histogram counts
  std::vector<double> bin_values_;//!< histogram values
  std::vector<double> weights_;//!< value weights
};

#endif
