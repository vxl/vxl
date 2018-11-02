// This is brl/bseg/brip/brip_phase_correlation.h
#ifndef brip_phase_correlation_h
#define brip_phase_correlation_h
//:
// \file
// \brief Find the translation between two images using phase correlation
// \author J.L. Mundy
// \date 4 May 2015
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
// Suppose two images are related by I1(u,v) = I0(u-tu, v-tv)
// The Fourier transforms of the images are related by the shift theorem
//
//       F1(wu, wv) = F0(wu,wv) exp(i(wu*tu + wv*tv))
//
// A Fourier transform can be represented by
//
//      F0 = mag(wu, wv)exp(i phi(wu, wv))   magnitude phase representation
//
// Intensity variations due to illumination and reflectance changes are mainly
// expressed in the magnitude term and so can be attenuated by setting mag(wu, wv) == 1.0;
//
// Next, the quantity F0 o ~F1 is formed where ~ denotes the complex conjugate and o is the
// element by element product of the two Fourier transform matrices. Thus,
//
//  F0 o ~F1 = exp(-i(wu*tu + wv*tv))
//
// The inverse transform, Finv[(F0 o ~F1)] = delta( (u-tu), (v-tv) )
//
// i.e., a delta function at the location (tu, tv), and equivalent
// to the cross correlation of the two images
//
// There are several parameters involved in the computation of the translation
//
//  gauss_sigma - a smoothing kernel for interpolating the correlation peak location
//  peak_radius - the neighborhood radius around a peak for interpolation
//                It is possible to relate the radius to gauss_sigma, but
//                then a threshold on Gauss kernel weight is required. The
//                radius is easier to interpret.
//  alpha - determines the rate that confidence approaches 1 with
//          increasing ratio = (s0/s1-1.0), where s0 is the top peak score
//          and s1 is the second highest correlation peak score.
//          For the current setting of 0.5 a ratio of 3:1 corresponds to a
//          confidence of 0.9.
//
//
#include <iostream>
#include <vector>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// a struct for holding correlation peak info
struct peak{
  float u_;
  float v_;
  float score_;
};

class brip_phase_correlation
{

 public:
  brip_phase_correlation(vil_image_view<float> const&img0, vil_image_view<float> const&img1,
                         float gauss_sigma= 1.0f, int peak_radius = 2, float alpha = 0.5f);

  ~brip_phase_correlation()= default;
  bool compute();
  bool translation(float& tu, float& tv, float& confidence) const;

  // for debug purposes
  bool compute_ffts();
  bool compute_correlation_array();
  bool extract_correlation_peaks();
  vil_image_view<float> img0() const {return img0_;}
  vil_image_view<float> img1() const {return img1_;}
  vil_image_view<float> mag0() const {return mag0_;}
  vil_image_view<float> phase0() const {return phase0_;}
  vil_image_view<float> mag1() const {return mag0_;}
  vil_image_view<float> phase1() const {return phase0_;}
  vil_image_view<float> correlation_array() const {return corr_;}
  vil_image_view<float> corr_peaks() const {return corr_peaks_;}


 protected:
  //parameters
  float alpha_;
  int peak_radius_;
  float gauss_sigma_;

  brip_phase_correlation();//no default constructor
  // compute a threshold using the Otsu algorithm
  float compute_threshold(vil_image_view<float> const& img) const;

  // input images ( ni and j are adjusted to be a power of 2)
  vil_image_view<float> img0_;
  vil_image_view<float> img1_;
  // margins needed to pad input images to reach a power of 2
  int nip2_margin0_;
  int njp2_margin0_;
  int nip2_margin1_;
  int njp2_margin1_;

  // Fourier transform of img0_
  vil_image_view<float> mag0_;
  vil_image_view<float> phase0_;
  // Fourier transform of img1_
  vil_image_view<float> mag1_;
  vil_image_view<float> phase1_;
  // Inverse transform (correlation surface)
  vil_image_view<float> corr_;
  // Local maxima in the correlation surface
  vil_image_view<float> corr_peaks_;
  // threshold for correlation values
  float thresh_;
  // average of the Gauss smoothed correlation values
  float gauss_avg_;
  // the vector of local maxima
  std::vector<peak> peaks_;
};

#endif // brip_phase_correlation_h
