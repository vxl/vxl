// This is contrib/brl/bseg/brip/brip_mutual_info.txx
#ifndef brip_mutual_info_txx_
#define brip_mutual_info_txx_
//:
// \file
// \brief Calculate the mutual information between the images.
// \author Matt Leotta
//

#include <brip/brip_mutual_info.h>
#include <brip/brip_histogram.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vxl_config.h>
#include <vcl_algorithm.h>


//: Calculate the Mutual Information between the images.
template<class T>
double brip_mutual_info(const vil_image_view<T>& image1,
                        const vil_image_view<T>& image2,
                        double min, double max, unsigned n_bins)
{
  assert( (image1.ni() == image2.ni()) &&
          (image1.nj() == image2.nj()) &&
          (image1.nplanes() == image2.nplanes()) );
  vcl_vector<double> histogram1, histogram2;
  vcl_vector<vcl_vector<double> > joint_histogram;

  double mag1, mag2, mag3;

  mag1 = brip_histogram(image1, histogram1, min, max, n_bins);
  mag2 = brip_histogram(image2, histogram2, min, max, n_bins);
  mag3 = brip_joint_histogram(image1, image2, joint_histogram, min, max, n_bins);

  double H1, H2, H3;
  H1 = brip_hist_entropy(histogram1, mag1);
  H2 = brip_hist_entropy(histogram2, mag2);
  H3 = brip_hist_entropy(joint_histogram, mag3);

  return H1 + H2 - H3;
}



// Macro to perform manual instantiations
#define BRIP_MUTUAL_INFO_INSTANTIATE(T) \
  template \
  double brip_mutual_info(const vil_image_view<T>& image1, \
                          const vil_image_view<T>& image2, \
                          double min, double max, unsigned n_bins); 


#endif // brip_mutual_info_txx_