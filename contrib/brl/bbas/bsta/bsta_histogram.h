// This is brl/bbas/bsta/bsta_histogram.h
#ifndef bsta_histogram_h_
#define bsta_histogram_h_
//:
// \file
// \brief A simple histogram class
// \author Joseph Mundy
// \date May 19, 2004
//
// A templated histogram class.  Supports entropy calculations
//
// \verbatim
//  J.L. Mundy added min,max, percentile methods
//  B.A. Mayer add clear() method so a single instance can revert
//             to the default constructor to be reused.
// \endverbatim

#include <vector>
#include <iostream>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bsta/bsta_histogram_base.h>

template <class T> class bsta_histogram : public bsta_histogram_base
{
 public:
  //: Default constructor
  bsta_histogram();

  //:Simple constructor that assumes all data values are positive
  bsta_histogram(const T range, const unsigned int nbins,
                 const T min_prob = 0.0);

  //:More general constructor defining a signed value range
  bsta_histogram(const T min, const T max, const unsigned int nbins,
                 const T min_prob = 0.0);

  //:More general constructor specifying bin interval, delta
  bsta_histogram(const unsigned int nbins, const T min, const T delta,
                 const T min_prob = 0.0);

  //:construct from other histogram data
  bsta_histogram(const T min, const T max, std::vector<T> const& data,
                 const T min_prob = 0.0);

  ~bsta_histogram() override = default;

  // The number of bins in the histogram
  unsigned int nbins() const { return nbins_; }

  //: min,max of total range
  T min() const {return min_;}
  T max() const {return max_;}

  //: bin interval
  T delta() const {return delta_;}

  //: minimum probability
  T min_prob() const {return min_prob_;}

  //: The value range for a bin
  void value_range(const unsigned int bin, T& vmin, T& vmax) const
  { assert(bin<nbins_); vmin = bin*delta_+min_; vmax = (bin+1)*delta_+min_; }

  //: The average value for a bin
  T avg_bin_value(const unsigned int bin) const
  { assert(bin<nbins_); return min_ + bin*delta_ + delta_/2; }

  //: The counts in a given bin
  T counts(const unsigned int bin) const
  { assert(bin<nbins_); return counts_[bin]; }

  //: probability of a given bin
  T p(const unsigned int bin) const;

  //: probability of a value in the range
  T p(const T value) const;

  //: Total area under the histogram
  T area() const;

  //: The area under the histogram up to (excluding) the given bin
  T cumulative_area(unsigned bin) const;

  //: Mean of distribution
  T mean() const;

  //: Mean of distribution between bin indices
  T mean(const unsigned int lowbin, const unsigned int highbin) const;

  //: Mean of distribution between values
  T mean_vals(const T low, const T high) const;

  //: Variance of distribution
  T variance() const;

  //: Variance of distribution between bin indices
  T variance(const unsigned int lowbin, const unsigned int highbin) const;

  //: Variance of distribution between values
  T variance_vals(const T low, const T high) const;

  //: First non-zero bin from below
  unsigned low_bin();

  //: First non-zero bin from above
  unsigned high_bin();

  //: Fraction of area less than val
  T fraction_below(const T value) const;

  //: Fraction of area greater than val
  T fraction_above(const T value) const;

  //: Value for area fraction below value
  T value_with_area_below(const T area_fraction) const;

  //: Value for area fraction above value
  T value_with_area_above(const T area_fraction) const;

  //: Entropy of p(x)
  T entropy() const;

  //: Renyi alpha = 2 entropy of p(x)
  T renyi_entropy() const;

 //: Increase the count of the bin corresponding to val by mag
  void upcount(T val, T mag);

  //: Return the bin this element would fall on - it doesn't modify the current count
  int bin_at_val(T val) const;

  //: set the count for a given bin
  void set_count(const unsigned bin, const T count)
  { if (bin<nbins_){ counts_[bin]=count; area_valid_ = false;}}

  //: array of bin values
  std::vector<T> value_array() const {
    std::vector<T> v(nbins_);
    for (unsigned b = 0; b<nbins_; ++b) v[b]=avg_bin_value(b); return v; }

  //: array of bin counts
  std::vector<T> count_array() const {
    std::vector<T> v(nbins_);
    for (unsigned b = 0; b<nbins_; ++b) v[b]=counts(b); return v; }

 //: Smooth the histogram with a Parzen window of sigma
  void parzen(const T sigma);

  //: Write to stream
  std::ostream& write(std::ostream&) const;

  //: Read from stream
  std::istream& read(std::istream&);

  void pretty_print(std::ostream& os = std::cout) const;

  void print(std::ostream& os = std::cout) const;

  //: print as a matlab plot command
  void print_to_m(std::ostream& os = std::cout) const;

  //: print x and y arrays
  void print_to_arrays(std::ostream& os = std::cout) const;

  //: print values and bin probability in full (even if counts ==0)
  void print_vals_prob(std::ostream& os = std::cout) const;

  //:restore default constructor state.
  void clear();

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
  std::vector<T> counts_;
};
// Histogram intersection (sum of min probabilies)
template <class T>
T hist_intersect(bsta_histogram<T> const& ha, bsta_histogram<T> const& hb);

// Bhattacharyya distance  -log(sum(sqrt(pa*pb)))
template <class T>
T bhatt_distance(bsta_histogram<T> const& ha, bsta_histogram<T> const& hb);

// Jensen-Shannon divergence
// 1/2 sum_i ( pa_i*log(2*pa_i/(pa_i+pb_i)+ pb_i*log(2*pb_i/(pa_i+pb_i)) )
template <class T>
T js_divergence(bsta_histogram<T> const& ha, bsta_histogram<T> const& hb);

// Bhattacharyya distance  -log(sum(sqrt(pa*pb)))
template <class T>
T bhatt_distance(bsta_histogram<T> const& ha, bsta_histogram<T> const& hb);

// Scale the range of the histogram
template <class T>
bsta_histogram<T> scale(bsta_histogram<T> const& h, T s);

// Find best scale to minimize Jensen-Shannon divergence scaling "from" to "to"
template <class T>
T minimum_js_divergence_scale(bsta_histogram<T> const& h_from, bsta_histogram<T> const& h_to,
                              T min_scale = T(1)/T(4));
// Histogram intersection (sum of min probabilies)
template <class T>
bool merge_hists(bsta_histogram<T> const& ha, bsta_histogram<T> const& hb, bsta_histogram<T>& h_merged);

//: Write histogram to stream
// \relatesalso bsta_histogram
template <class T>
std::ostream&  operator<<(std::ostream& s, bsta_histogram<T> const& h);

//: Read histogram from stream
// \relatesalso bsta_histogram
template <class T>
std::istream&  operator>>(std::istream& is,  bsta_histogram<T>& h);

//: Forward declaration of specialization
template <>
void bsta_histogram<char>::pretty_print(std::ostream& os) const;
#include <bsta/bsta_histogram_sptr.h>
#define BSTA_HISTOGRAM_INSTANTIATE(T) extern "Please #include <bsta/bsta_histogram.hxx>"

#endif // bsta_histogram_h_
