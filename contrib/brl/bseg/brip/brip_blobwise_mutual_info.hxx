// This is brl/bseg/brip/brip_blobwise_mutual_info.hxx
#ifndef brip_blobwise_mutual_info_hxx_
#define brip_blobwise_mutual_info_hxx_
//:
// \file
// \brief Calculate the mutual information between the images.
// \author Matt Leotta

#include <vector>
#include <iostream>
#include <algorithm>
#include "brip_blobwise_mutual_info.h"
#include "brip_histogram.h"
#include "brip_mutual_info.h"
#include <bil/algo/bil_blob_finder.h>
#include <vil/algo/vil_binary_dilate.h>
#include <vil/algo/vil_binary_erode.h>
#include <vil/vil_math.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Calculate the Mutual Information between the images.
template<class T>
void brip_blobwise_mutual_info (const vil_image_view<T>& img1,
                                const vil_image_view<T>& img2,
                                const vil_image_view<T>& weights, //if weight is under a certain value, then dont use for MI
                                const vil_image_view<bool>& mask,
                                      vil_image_view<T>& mi_img )
{
#if 0
  //pixel gradient weight must be higher than this
  constexpr T minWeight = .1;
#endif
  //blob region is just a vector of vil_chords (rows in image)
  bil_blob_finder finder(mask);
  std::vector<vil_chord> region;
  while (finder.next_4con_region(region))
  {
    //gather vector of samples
    std::vector<T> x_samps;
    std::vector<T> y_samps;
    std::vector<double> weight_samps;
    std::vector<vil_chord>::iterator iter;
    for (iter=region.begin(); iter!=region.end(); ++iter) {
      //add each pixel in this row to blob
      for (unsigned i=iter->ilo; i<iter->ihi+1; ++i) {
#if 0
        if ( weights(i, iter->j) > minWeight )
#endif
        {
          x_samps.push_back( img1(i, iter->j) );
          y_samps.push_back( img2(i, iter->j) );
          weight_samps.push_back( weights(i, iter->j) );
        }
      }
    }

    //calculate mutual information between these two sets of samples
    vil_image_view<T> x(1, x_samps.size());
    vil_image_view<T> y(1, x_samps.size());
    vil_image_view<double> wImg(1, x_samps.size());
    for (unsigned int i=0; i<x.size(); ++i) {
      x(0, i) = x_samps[i];
      y(0, i) = y_samps[i];
      wImg(0,i) = weight_samps[i];
    }
    std::vector<double> x_hist, y_hist;
    std::vector<std::vector<double> > joint_hist;

    unsigned n_bins = 32;
    double min = -vnl_math::pi,
           max =  vnl_math::pi;
    double mag1, mag2, mag3;
    mag1 = brip_histogram(x, x_hist, min, max, n_bins);
    mag2 = brip_histogram(y, y_hist, min, max, n_bins);
    mag3 = brip_joint_histogram(x, y, joint_hist, min, max, n_bins);

#if 0
    factor in prior prob distribution
    for (unsigned int i=0; i<x_hist.size(); ++i) {
      x_hist[i]++;
      mag1++;
    }
    for (int i=0; i<y_hist.size(); ++i) {
      y_hist[i]++;
      mag2++;
    }
    for (int i=0; i<joint_hist.size(); ++i) {
      for (int j=0; j<joint_hist[i].size(); ++j) {
        joint_hist[i][j] += 1.0/n_bins;
        mag3 += 1.0/n_bins;
      }
    }
#endif // 0

    double H1, H2, H3, MI;
    H1 = brip_hist_entropy(x_hist, mag1);
    H2 = brip_hist_entropy(y_hist, mag2);
    H3 = brip_hist_entropy(joint_hist, mag3);
    MI = H1 + H2 - H3;

    //if it's empty, we should return max MI as default
    if (x_samps.size() == 0)
      MI = 5.0;

    if (MI != MI) {
      std::cout<<"mutual info is NAN\n"
              <<"  num samps = "<<x_samps.size()<<std::endl;
    }
    if ( std::fabs(MI-.6702) < .0001 ) {
      std::cout<<"MID BLOB samps:\n"
              <<"  num samps = "<<x_samps.size()<<std::endl;
    }
    if (x_samps.size() == 0) {
      std::cout<<"ZERO SAMPLE SET:\n"
              <<"   MI = "<<MI<<'\n'
              <<"   H1 = "<<H1<<'\n'
              <<"   H2 = "<<H2<<'\n'
              <<"   Hxy= "<<H3<<std::endl;
    }


    //store mutual information value back in MI img
    for (iter=region.begin(); iter!=region.end(); ++iter)
      for (unsigned i=iter->ilo; i<iter->ihi+1; ++i)
        mi_img(i, iter->j) = (T)MI;
  } //end blob while
}


template<class T>
void brip_blobwise_kl_div( const vil_image_view<T>& img1,
                           const vil_image_view<T>& img2,
                           const vil_image_view<bool>& mask,
                                 vil_image_view<T>& mi_img )
{
  //------------------------------------------------
  // iterate over each blob,
  //   compute KL div for blob distribution in img1 to img2
  //------------------------------------------------
  bil_blob_finder finder(mask);
  std::vector<vil_chord> region;
  while (finder.next_4con_region(region))
  {
    //gather vector of samples
    std::vector<T> x_samps;
    std::vector<T> y_samps;
    std::vector<vil_chord>::iterator iter;
    for (iter=region.begin(); iter!=region.end(); ++iter) {
      //add each pixel in this row to blob
      for (unsigned i=iter->ilo; i<iter->ihi+1; ++i) {
        x_samps.push_back( img1(i, iter->j) );
        y_samps.push_back( img2(i, iter->j) );
      }
    }

    //move samples into a 1-d image for KL div calc
    vil_image_view<T> x(1, x_samps.size());
    vil_image_view<T> y(1, x_samps.size());
    for (unsigned int i=0; i<x.size(); ++i) {
      x(0, i) = x_samps[i];
      y(0, i) = y_samps[i];
    }

    //---find global min/max values
    T min1, max1, min2, max2;
    vil_math_value_range(x, min1, max1);
    vil_math_value_range(y, min2, max2);
    double min = std::min(min1, min2);
    double max = std::max(max1, max2);

    //compute the number of bins as a function of num samps
    unsigned n_bins = std::min((int) x_samps.size()/2, 32);

    //bin the two blob samples
    std::vector<double> x_hist, y_hist;
    std::vector<std::vector<double> > joint_hist;
    double magX = brip_histogram(x, x_hist, min, max, n_bins);
    double magY = brip_histogram(y, y_hist, min, max, n_bins);

    //factor in prior prob distribution
    for (unsigned int i=0; i<x_hist.size(); ++i) {
      x_hist[i] += .01/n_bins;
      magX      += .01/n_bins;
      y_hist[i] += .01/n_bins;
      magY      += .01/n_bins;
    }
    T KL = (T)brip_hist_kl_div(x_hist, magX, y_hist, magY);

    //store mutual information value back in MI img
    for (iter=region.begin(); iter!=region.end(); ++iter)
      for (unsigned i=iter->ilo; i<iter->ihi+1; ++i)
        mi_img(i, iter->j) = KL;

#if 0
    //print out debug info
    iter = region.begin();
    int startI = iter->ilo,
        startJ = iter->j;
    if (startI > 422 && startI < 460 && startJ>280 && startJ<310) {
      std::cout<<"BLOB with size "<<x_samps.size()<<'\n'
              <<"  histX (inimg) = ";
      for (int i=0; i<x_hist.size(); ++i) std::cout<<x_hist[i]/magX<<' ';
      std::cout<<'\n'
              <<"  histY (expimg)= ";
      for (int i=0; i<y_hist.size(); ++i) std::cout<<y_hist[i]/magY<<' ';
      std::cout<<'\n'
              <<"  KL div = "<<KL<<std::endl;
    }
#endif
  } //end blob while
}


// Macro to perform manual instantiations
#define BRIP_BLOBWISE_MUTUAL_INFO_INSTANTIATE(T) \
  template \
  void brip_blobwise_mutual_info (const vil_image_view<T >& img1, \
                                  const vil_image_view<T >& img2, \
                                  const vil_image_view<T >& weights, \
                                  const vil_image_view<bool>& mask, \
                                        vil_image_view<T >& mi_img ); \
  template \
  void brip_blobwise_kl_div( const vil_image_view<T >& img1, \
                             const vil_image_view<T >& img2, \
                             const vil_image_view<bool>& mask, \
                                   vil_image_view<T >& mi_img )

#endif // brip_blobwise_mutual_info_hxx_
