// This is brl/bseg/brip/brip_blobwise_mutual_info.h
#ifndef brip_blobwise_mutual_info_h_
#define brip_blobwise_mutual_info_h_
//:
// \file
// \brief Calculate patchwise mutual info between two images
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
void brip_blobwise_mutual_info ( const vil_image_view<T>& img1,
                                 const vil_image_view<T>& img2,
                                 const vil_image_view<T>& weights,
                                 const vil_image_view<bool>& mask,
                                       vil_image_view<T>& mi_img );


template<class T>
void brip_blobwise_kl_div( const vil_image_view<T>& img1,
                           const vil_image_view<T>& img2,
                           const vil_image_view<bool>& mask,
                                 vil_image_view<T>& mi_img );

//: calculates the kl divergeance D_kl ( P || Q );
inline
double brip_hist_kl_div(const std::vector<double>& P, double magP,
                        const std::vector<double>& Q, double magQ)
{
  double kl = 0.0;
  for (unsigned int i=0; i<P.size(); ++i) {
    double probP = P[i]/magP;
    double probQ = Q[i]/magQ;
    if (probP==0 || probQ==0) continue;

    kl += probP * std::log( probP / probQ );
  }
  return  kl/vnl_math::ln2; // divide by ln(2) to convert this measure to base 2
}

#endif // brip_blobwise_mutual_info_h_
