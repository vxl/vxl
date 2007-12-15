// This is brl/bbas/bsta/bsta_int_histogram_1d.h
#ifndef BSTA_INT_HISTOGRAM_1D_H_
#define BSTA_INT_HISTOGRAM_1D_H_

//-----------------------------------------------------------------------------
//:
// \file
// \brief 1D integer Histograms with bucket width = 1
//
//  This simple histogram class is an integer version of gel/vifa/vifa_histogram.h
//    which was a port from Targetjr.  Also borrowed from bsta_histogram.  Only the
//    features needed for integer histograms of images with bucket_width = 1 are included.
//    The class is defined to create a histogram with long int buckets (4 bytes).
//  The original Targetjr class had methods like ::scale(...) which were needed
//    (for example) for the edgel strength algorithm in which bucket widths needed to
//    be rescaled, etc, so all floats were used.  Here we just use integers.  Also
//    included are some useful methods for finding properties of these histogrems, like
//    the min and max value, largest value, locations of these, etc.
//  It is initally placed in contrib/jgl/bsta for now.  If others think it useful, it can be
//    moved to a permanent home later.
//
// \author James E. Green
// \date   23 April, 2007
//
// \verbatim
//  Modifications
//   2007/04/23 Initial Version
//
//
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_vector.h>

class bsta_int_histogram_1d
{
 private:
  unsigned int nbins_;
  vcl_vector<long int> counts_;

 public:

 // default constructor that assumes all data values are positive
  // constructor parameters:
  //   nbins_      # bins to create in this histogram.  Note that for 2-byte data the number
  //                 could be 0-65,535, it is usually much smaller,  1-byte data ranges
  //                 0-255, but may be less. (so it's an unsigned int)

  bsta_int_histogram_1d(const unsigned int bins);

  // destructor
  ~bsta_int_histogram_1d();

  // The number of bins in the histogram
  unsigned int get_nbins() { return nbins_;}

  //: min,max of total range
  unsigned int get_min_bin();
  unsigned int get_max_bin();

  //: Get total area under the histogram = total counts in histogram
  unsigned long int get_area();

  //: Get the count in a given bin
  long int get_count(unsigned int bin); // const??

  //: Set the count for a given bin
  void set_count(const unsigned int bin, const long int val);

  // Get highest value in histogram; returns max value; index of max is available in imax
  unsigned long int get_max_val(unsigned int &imax);

  // Trim off a fraction of the histogram at top and bottom ends.  Note that fractions
  //    should be less than 0.5, usually considerably less, eg usually 0.01 or less.
  void trim(float low_fract, float high_fract);

  // Zero out bottom n bins.
  void zero_low(unsigned n);

  // Zero out top n bins.
  void zero_high(unsigned n);

  //: Smooth the histogram with a Parzen window of sigma
  void parzen(const float sigma);

  // Find the "significant peaks & vallwys in a histogram.  Here "significant" means there is
  //   a specified difference in height between the peak and the previous valley, or vice versa.
  bool find_peaks( float perct, int &n_peaks, vcl_vector<unsigned int> &peaks);
};

#endif // BSTA_INT_HISTOGRAM_1D_H_
