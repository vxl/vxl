// This is contrib/brl/bseg/brip/brip_mutual_info.h
#ifndef brip_mutual_info_h_
#define brip_mutual_info_h_
//:
// \file
// \brief Calculate the mutual information between images
// \author Matt Leotta
//

#include <vil/vil_image_view.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>

#define LN_2 0.69314718056


//: Calculate the Mutual Information between the images.
template<class T>
double brip_mutual_info(const vil_image_view<T>& image1,
                        const vil_image_view<T>& image2,
                        double min, double max, unsigned n_bins);


//: Calculate the entropy of a histogram
inline
double brip_hist_entropy(const vcl_vector<double>& histogram, double mag)
{
  double entropy = 0.0;
  for( vcl_vector<double>::const_iterator h_itr = histogram.begin();
       h_itr != histogram.end(); ++h_itr ){
    double prob = (*h_itr)/mag;
    entropy += -(prob?prob*vcl_log(prob):0); // if prob=0 this value is defined as 0
  }
  return entropy/LN_2; // divide by ln(2) to convert this measure to base 2
}


//: Calculate the entropy of a joint histogram
inline
double brip_hist_entropy(const vcl_vector<vcl_vector<double> >& histogram, double mag)
{
  double entropy = 0.0;
  for( vcl_vector<vcl_vector<double> >::const_iterator h_itr = histogram.begin();
       h_itr != histogram.end(); ++h_itr ){
    entropy += brip_hist_entropy(*h_itr,mag);
  }
  return entropy;
}

#endif // brip_mutual_info_h_