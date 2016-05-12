#ifndef bsta_otsu_threshold_h_
#define bsta_otsu_threshold_h_
//:
// \file
// \brief Implements Otsu's threshold method for 1D distribution and 2 classes
// \author Vishal Jain, (vj@lems.brown.edu)
// \date April 09, 2005
//
//  The Otsu thresholding method maximizes the between class variance by
//  an exhaustive linear scan over the histogram bin indices
//
// \verbatim
//  Modifications
//   Templated and moved up to bsta - J.L. Mundy June 29, 2005
// \endverbatim

#include <vector>
#include<bsta/bsta_histogram.h>

template <class T> class bsta_otsu_threshold
{
 public:
  // constructor
  bsta_otsu_threshold(bsta_histogram<T> const& hist);
  //: takes vector of data as input
  bsta_otsu_threshold(std::vector<T> data, T low , T high);
  // destructor
  ~bsta_otsu_threshold();

  unsigned bin_threshold() const;

  T threshold() const {return distribution_1d_.avg_bin_value(this->bin_threshold());}

 protected:
  //: default constructor doesn't make sense
  bsta_otsu_threshold() : distribution_1d_(bsta_histogram<T>(0,0)){}

  bsta_histogram<T> distribution_1d_;
};

#define BSTA_OTSU_THRESHOLD_INSTANTIATE(T) extern "Please #include <bsta/bsta_otsu_threshold.hxx>"

#endif // bsta_otsu_threshold_h_
