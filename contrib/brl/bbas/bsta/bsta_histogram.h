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
//
// \verbatim
//  Modifications
// \endverbatim

template <class T> class bsta_histogram
{
 public:
  bsta_histogram(const T range = 360, const unsigned int nbins = 8,
                 const T min_prob = 0.0);
 ~bsta_histogram() {}
  unsigned int nbins() const { return nbins_; }
  void upcount(T dir, T mag);
  void parzen(const T sigma);
  
  T p(unsigned int bin) const;
  T area() const;
  T entropy() const;
  T renyi_entropy() const;
  void print() const;
 private:
  void compute_area() const; // mutable const
  mutable bool area_valid_;
  mutable T area_;
  unsigned int nbins_;
  T range_;
  T delta_;
  T min_prob_;
  vcl_vector<T> counts_;
};

#define BSTA_HISTOGRAM_INSTANTIATE(T) extern "Please #include <bsta/bsta_histogram.txx>"

#endif // bsta_histogram_h_
