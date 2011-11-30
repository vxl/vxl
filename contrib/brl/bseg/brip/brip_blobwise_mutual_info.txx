// This is brl/bseg/brip/brip_blobwise_mutual_info.txx
#ifndef brip_blobwise_mutual_info_txx_
#define brip_blobwise_mutual_info_txx_
//:
// \file
// \brief Calculate the mutual information between the images.
// \author Matt Leotta

#include "brip_blobwise_mutual_info.h"
#include "brip_histogram.h"
#include "brip_mutual_info.h"
#include <bil/algo/bil_blob_finder.h>
#include <vil/algo/vil_binary_dilate.h>
#include <vil/algo/vil_binary_erode.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_algorithm.h>

//: Calculate the Mutual Information between the images.
template<class T>
void brip_blobwise_mutual_info (const vil_image_view<T>& img1,
                                const vil_image_view<T>& img2,
                                const vil_image_view<bool>& mask,
                                      vil_image_view<T>& mi_img )
{
  
   //blob region is just a vector of vil_chords (rows in image)
  bil_blob_finder finder(mask);
  vcl_vector<vil_chord> region;
  while (finder.next_4con_region(region))
  {
    //gather vector of samples
    vcl_vector<T> x_samps; 
    vcl_vector<T> y_samps; 
    vcl_vector<vil_chord>::iterator iter;
    for (iter=region.begin(); iter!=region.end(); ++iter) {
      //add each pixel in this row to blob
      for (unsigned i=iter->ilo; i<iter->ihi+1; ++i) {
        x_samps.push_back( img1(i, iter->j) ); 
        y_samps.push_back( img2(i, iter->j) ); 
      }
    }
    
    //calculate mutual informaiton between these two sets of samples
    vil_image_view<T> x(1, x_samps.size()); 
    vil_image_view<T> y(1, x_samps.size()); 
    for(int i=0; i<x.size(); ++i) {
      x(0, i) = x_samps[i]; 
      y(0, i) = y_samps[i]; 
    }
    vcl_vector<double> x_hist, y_hist;
    vcl_vector<vcl_vector<double> > joint_hist;

    unsigned n_bins = 32; 
    double min=0.0, max=1.0;
    double mag1, mag2, mag3;
    mag1 = brip_histogram(x, x_hist, min, max, n_bins);
    mag2 = brip_histogram(y, y_hist, min, max, n_bins);
    mag3 = brip_joint_histogram(x, y, joint_hist, min, max, n_bins);

    T H1, H2, H3, MI;
    H1 = brip_hist_entropy(x_hist, mag1);
    H2 = brip_hist_entropy(y_hist, mag2);
    H3 = brip_hist_entropy(joint_hist, mag3);
    MI = H1 + H2 - H3;
    
    //store mutual information value back in MI img
    for (iter=region.begin(); iter!=region.end(); ++iter) 
      for (unsigned i=iter->ilo; i<iter->ihi+1; ++i) 
        mi_img(i, iter->j) =  MI;

    
  } //end blob while
  
}


// Macro to perform manual instantiations
#define BRIP_BLOBWISE_MUTUAL_INFO_INSTANTIATE(T) \
  template \
  void brip_blobwise_mutual_info (const vil_image_view<T>& img1, \
                                  const vil_image_view<T>& img2, \
                                  const vil_image_view<bool>& mask, \
                                        vil_image_view<T>& mi_img )

#endif // brip_blobwise_mutual_info_txx_
