// This is brl/bbas/bsta/bsta_histogram.h
#ifndef bsta_histogram_h_
#define bsta_histogram_h_
//:
// \file
// \brief A simple histogram class
// \author Joseph Mundy
// \date 5/19/04
//
// A templated histogram class.  Supports entropy calculations
//
// \verbatim
//  J.L. Mundy added min,max, percentile methods
// \endverbatim

#include <vcl_vector.h>
#include <vcl_cassert.h>
template <class T> class bsta_histogram
{
 public:
  //:Simple default constructor that assumes all data values are positive
  bsta_histogram(const T range, const unsigned int nbins,
                 const T min_prob = 0.0);

  //:More general constructor defining a signed value range
  bsta_histogram(const T min, const T max, const unsigned int nbins,
                 const T min_prob = 0.0);

  //:construct from other histogram data
  bsta_histogram(const T min, const T max, vcl_vector<T> const& data,
                 const T min_prob = 0.0);


 ~bsta_histogram() {}

 // The number of bins in the histogram
  unsigned int nbins() const { return nbins_; }

  //: min,max of total range
  T min() const {return min_;}
  T max() const {return max_;}

  //: The value range for a bin
  void value_range(const unsigned int bin, T& vmin, T& vmax)
    {assert(bin<nbins_); vmin = bin*delta_+min_; vmax = (bin+1)*delta_+min_;}
  
  //: The average value for a bin
  T avg_bin_value(const unsigned int bin)
    {assert(bin<nbins_); return (min_ + bin*delta_ + delta_/2);}

  //: The counts in a given bin
  T counts(const unsigned int bin) const
    {assert(bin<nbins_); return counts_[bin];}
  
  //: probability of a given bin
  T p(const unsigned int bin) const;

  //: probability of a value in the range
  T p(const T value) const;

  //: Total area under the histogram
  T area() const;

  //: Mean of distribution
  T mean() const;

  //: Mean of distribution between bin indices
  T mean(const unsigned int lowbin, const unsigned int highbin) const;

  //: Variance of distribution
  T variance() const;

  //: Variance of distribution between bin indices
  T variance(const unsigned int lowbin, const unsigned int highbin) const;

  
  //: Fraction of area less than val
  T fraction_below(const T value) const;

  //: Fraction of area greater than val
  T fraction_above(const T value) const;

  //: Value for area fraction below value
  T value_with_area_below(const T area_fraction) const;

  //: Value for area fraction below value
  T value_with_area_above(const T area_fraction) const;

  //: Entropy of p(x)
  T entropy() const;

  //: Renyi alpha = 2 entropy of p(x)
  T renyi_entropy() const;

 //: Increase the count of the bin corresponding to val by mag
  void upcount(T val, T mag);

 //: Smooth the histogram with a Parzen window of sigma
  void parzen(const T sigma);


  void print() const;
 private:
  void compute_area() const; // mutable const
  mutable bool area_valid_;
  mutable T area_;
  unsigned int nbins_;
  T range_;
  T delta_;
  T min_prob_;
  T min_;
  T max_;
  vcl_vector<T> counts_;
};

#define BSTA_HISTOGRAM_INSTANTIATE(T) extern "Please #include <bsta/bsta_histogram.txx>"

#endif // bsta_histogram_h_
