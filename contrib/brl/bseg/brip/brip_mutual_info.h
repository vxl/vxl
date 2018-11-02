// This is brl/bseg/brip/brip_mutual_info.h
#ifndef brip_mutual_info_h_
#define brip_mutual_info_h_
//:
// \file
// \brief Calculate the mutual information between images
// \author Matt Leotta
//

#include <vector>
#include <iostream>
#include <cmath>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>


//: Calculate the Mutual Information between the images.
template<class T>
double brip_mutual_info(const vil_image_view<T>& image1,
                        const vil_image_view<T>& image2,
                        double min, double max, unsigned n_bins);


//: Calculate the entropy of a histogram
inline
double brip_hist_entropy(const std::vector<double>& histogram, double mag)
{
  double entropy = 0.0;
  for (double h_itr : histogram){
    double prob = h_itr/mag;
    entropy += -(prob?prob*std::log(prob):0); // if prob=0 this value is defined as 0
  }
  return entropy/vnl_math::ln2; // divide by ln(2) to convert this measure to base 2
}


//: Calculate the entropy of a joint histogram
inline
double brip_hist_entropy(const std::vector<std::vector<double> >& histogram, double mag)
{
  double entropy = 0.0;
  for (const auto & h_itr : histogram){
    entropy += brip_hist_entropy(h_itr,mag);
  }
  return entropy;
}

#endif // brip_mutual_info_h_
