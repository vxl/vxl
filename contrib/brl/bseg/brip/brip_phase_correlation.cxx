#include <iostream>
#include <cmath>
#include "brip_phase_correlation.h"
#include "brip_vil_float_ops.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vil/vil_math.h>
#include <vnl/vnl_random.h>
#include <vil/vil_resample_bicub.h>
#include <bsta/bsta_histogram.h>
#include <bsta/bsta_otsu_threshold.h>
static bool peak_greater(const peak& pa, const peak& pb){
  return pa.score_ > pb.score_;
}

brip_phase_correlation::
brip_phase_correlation(vil_image_view<float> const&img0,
                       vil_image_view<float> const&img1,
                       float gauss_sigma,
                       int peak_radius,
                       float alpha):
  alpha_(alpha),
  peak_radius_(peak_radius),
  gauss_sigma_(gauss_sigma)
{
  float fill_value = 0.0f;
  int ni0 = img0.ni(), nj0 = img0.nj();
  int ni1 = img1.ni(), nj1 = img1.nj();
  //find smallest image colum
  int ni = ni0;
  if(ni>ni1) ni = ni1;
  int nj = nj0;
  if(nj>nj1) nj = nj1;

  //reduce to a power of two in each dimension, required by the FFT algorithm
  double p2i = std::floor(std::log(static_cast<double>(ni))/vnl_math::ln2 + 0.5);
  double p2j = std::floor(std::log(static_cast<double>(nj))/vnl_math::ln2 + 0.5);
  int nip2 = static_cast<unsigned>(std::pow(2.0, p2i));
  int njp2 = static_cast<unsigned>(std::pow(2.0, p2j));
  nip2_margin0_ = (ni0-nip2)/2; njp2_margin0_ = (nj0-njp2)/2;
  nip2_margin1_ = (ni1-nip2)/2; njp2_margin1_ = (nj1-njp2)/2;
  // if source image is smaller than target, fill borders with the fill_value
  img0_.set_size(nip2, njp2);
  img1_.set_size(nip2, njp2);
  for(int j = 0; j<njp2; ++j)
    for(int i = 0; i<nip2; ++i){
      int di0 = nip2_margin0_ + i, dj0 = njp2_margin0_ + j;
      int di1 = nip2_margin1_ + i, dj1 = njp2_margin1_ + j;
      if(di0<0||dj0<0||di0>=ni0||dj0>=nj0)
        img0_(i,j) = fill_value;
      else
        img0_(i,j) = img0(di0, dj0);

      if(di1<0||dj1<0||di1>=ni1||dj1>=nj1)
        img1_(i,j) = fill_value;
      else
        img1_(i,j) = img1(di1, dj1);
    }

}

bool brip_phase_correlation::compute_ffts(){
  bool good = brip_vil_float_ops::fourier_transform(img0_, mag0_, phase0_);
  if(!good) return false;
  good =  brip_vil_float_ops::fourier_transform(img1_, mag1_, phase1_);
  return good;
}

bool brip_phase_correlation::compute_correlation_array(){
  unsigned ni0 = phase0_.ni(), nj0 = phase0_.nj();
  unsigned ni1 = phase1_.ni(), nj1 = phase1_.nj();
  if(ni0 != ni1 || nj0 != nj1) return false;
  // form complex conjugate product of phase terms, magnitude is set to 1
  vil_image_view<float>  prod_mag(ni0, nj0), prod_phase(ni0, nj0);
  prod_mag.fill(1.0f);
  for(unsigned j = 0; j<nj0; ++j)
    for(unsigned i = 0; i<ni0; ++i)
      prod_phase(i,j)=  phase0_(i,j) - phase1_(i,j);
  //                                 ^---------------------complex conjugate (minus sign)
  bool good = brip_vil_float_ops::inverse_fourier_transform(prod_mag, prod_phase, corr_);
  if(good){
    for(unsigned j = 0; j<corr_.nj(); ++j)
      for(unsigned i = 0; i<corr_.ni(); ++i)
        corr_(i,j) = std::fabs(corr_(i,j));
  }
  thresh_ = compute_threshold(corr_);
  return good;
}
float brip_phase_correlation::compute_threshold(vil_image_view<float> const& img) const{
  float min0, max0;
  vil_math_value_range(corr_, min0, max0);
  bsta_histogram<float> h(min0, max0, 50);
  int ni = img.ni(), nj = img.nj();
  for(int j = 0; j<nj; ++j)
    for(int i = 0; i<ni; ++i)
      h.upcount(img(i,j), 1.0f);
  bsta_otsu_threshold<float> ott(h);
  float thresh = ott.threshold();
  return thresh;
}

bool brip_phase_correlation::extract_correlation_peaks(){
  peaks_.clear();
  int ni = corr_.ni(), nj = corr_.nj();
  //eliminate correlation values below the Otsu threshold
  vil_image_view<float> temp(ni, nj), temp1;
  temp.fill(0.0f);
  for(int j = 0; j<nj; ++j)
    for(int i = 0; i<ni; ++i)
      if(corr_(i,j)>thresh_)
        temp(i,j) = corr_(i,j);

  //smooth remaining peaks with a gaussian
  temp1 = brip_vil_float_ops::gaussian(temp, gauss_sigma_, "periodic");


  // determine local maxima by iterating through the smoothed correlation image
   corr_peaks_.set_size(ni, nj);
   corr_peaks_.fill(0.0f);
   float avg_sum = 0.0f, count = 0.0f;
   for(int j = 0; j<nj; ++j)
     for(int i = 0; i<ni; ++i){
       // center value
       float cv = temp1(i,j);
       if(cv ==0.0f)
         continue;
       avg_sum += cv;
       count += 1.0f;
       //accumulate centroid sums
       float sumu = 0.0f, sumv = 0.0f;
       float sumg = 0.0f;
       bool is_peak = true;
       for(int jj = -peak_radius_; jj<=peak_radius_; ++jj)
         for(int ii = -peak_radius_; ii<=peak_radius_; ++ii){
           int is = i+ii, js = j+jj;
           if(is<0||is>=ni||js<0||js>=nj)
             continue;
           float g = temp1(is, js);
           sumg +=g;
           // weight centroid by correlation values
           sumu += is*g;
           sumv += js*g;
           //skip the center location
           if(ii==jj)
             continue;
           // if any neighbor is larger then the
           // center can't be a peak
           if(is_peak&&temp1(is, js)>cv){
             cv = 0.0f;
             is_peak = false;
           }
         }
       corr_peaks_(i,j) = cv;

       if(is_peak){
                   //don't include peak in background sum
         avg_sum -= cv;
         count -= 1.0f;
          if(sumg <=0.0)
            continue;
         // estimated correlation peak location
         sumu /= sumg;
         sumv /= sumg;
         // fill the peak struct
         peak pk;
         if(sumu<(ni/2)) // positive tu is in the right half of the inverse transform
           pk.u_ = -sumu;
         else
           pk.u_ = (ni-1)-sumu;
         if(sumv<(nj/2)) // positive tv is in the lower half of the inverse transform
           pk.v_ = -sumv;
         else
           pk.v_ = (nj-1)-sumv;
         pk.score_ = cv;
         peaks_.push_back(pk);
       }
     }
   // sort the scores so that peaks_[0] is the largest
   std::sort(peaks_.begin(), peaks_.end(), peak_greater);

  //null hypothesis to use in case of a single peak
   gauss_avg_ = avg_sum/count;

   return true;
}
bool brip_phase_correlation::compute(){
  bool good = this->compute_ffts();
  if(!good){
    std::cout << "computation of fast fourier transforms failed\n";
    return false;
  }
  good = this->compute_correlation_array();
  if(!good){
    std::cout << "computation of correlation array failed\n";
    return false;
  }
  good =  this->extract_correlation_peaks();
  if(!good){
    std::cout << "extract correlation peaks failed\n";
    return false;
  }
  return true;
}
bool brip_phase_correlation::translation(float& tu, float& tv, float& confidence) const{
  confidence = 0.0f;
  int npeaks = static_cast<int>(peaks_.size());
  if(npeaks == 0){
    std::cout << "translation failed - no local correlation maxima\n";
    return false;
  }
#if 0 // for debug
  int limit = npeaks;
  if(limit>5) limit = 5;
  std::cout << "scores \n";
  for(int i=0; i<limit; ++i)
    std::cout << peaks_[i].u_ << ' ' << peaks_[i].v_ << ' ' << peaks_[i].score_ << '\n';
#endif
  const peak& pk = peaks_[0];
  tu = pk.u_; tv = pk.v_;
  float score = pk.score_, ratio = 1.0;
  //  if only one peak, use background average as normalizer
  if(npeaks==1)
    ratio = score/gauss_avg_;
  // otherwise use the next lower peak score as normalizer
  else if(npeaks >=2){
    float s1 = peaks_[1].score_;
    if(s1<=0.0f)
      s1 = gauss_avg_;
    ratio  = score/s1;
  }
  // map the score ratio to [0, 1]
  // alpha_ is a scale factor to define ratio significance
  // (should adjust using a large training test set, default value for now)
  confidence =std::tanh(alpha_*(ratio-1.0f));
  return true;
}
