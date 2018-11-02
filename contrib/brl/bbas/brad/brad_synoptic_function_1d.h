#ifndef __BRAD_SYNOPTIC_FUNCTION_1D_H
#define __BRAD_SYNOPTIC_FUNCTION_1D_H
//-----------------------------------------------------------------------------
//:
// \file
// \brief A function for interpolating intensity with respect to viewpoint
//
//
// \author J. L. Mundy
// \date October 1, 2011
//
//----------------------------------------------------------------------------
#include <utility>
#include <vector>
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_double_4.h>
class brad_synoptic_function_1d
{
 public:


  brad_synoptic_function_1d(): inherent_sigma_(0.02), tau_s_(0.25),
    //    max_freq_mean_(0.487648), max_freq_sigma_(0.127436755) {}
    max_freq_mean_(0.487648), max_freq_sigma_(0.2) {}
  //:constructor from batch orbit data
  brad_synoptic_function_1d(std::vector<double>  elevation,
                            std::vector<double>  azimuth,
                            std::vector<double>  vis,
                            std::vector<double>  intensity):
    elev_(std::move(elevation)),azimuth_(std::move(azimuth)),vis_(std::move(vis)),intensity_(std::move(intensity)),
    inherent_sigma_(0.06), tau_s_(0.25), max_freq_mean_(0.487648),
    //    max_freq_sigma_(0.127436755){this->fit_intensity_cubic();}
    max_freq_sigma_(0.2){this->fit_intensity_cubic();}
  //: set members
  void set_elevation(std::vector<double> const& elevation){
    elev_ = elevation;}
  void set_azimuth(std::vector<double> const& azimuth){
   azimuth_ = azimuth;}
  void set_vis(std::vector<double> const& vis){
   vis_ = vis;}
  void set_intensity(std::vector<double> const& intensity){
   intensity_ = intensity;}
  void set_inherent_data_sigma(double sigma){inherent_sigma_=sigma;}

  //=  model for an intensity function that correlated to view dir.==

  //: fit a cubic to the intensity as function of arc length
  void fit_intensity_cubic();

  //:load batch orbit data from a file
  bool load_samples(std::string const& path);

  //: the number of observations on the orbit
  unsigned size(){return (unsigned)elev_.size();}

  //: member accessors
  double intensity(unsigned index){return intensity_[index];}
  double vis(unsigned index){return vis_[index];}
  double elev(unsigned index){return elev_[index];}
  double azimuth(unsigned index){return azimuth_[index];}
  vnl_double_4 cubic_coef_int(){return cubic_coef_int_;}

  //:spherical angle between two points on unit sphere
  static double angle(double elev0, double az0, double elev1, double az1);

  //:spherical arc length at sample index
  double arc_length(unsigned index);

  //: interpolated intensity based on the cubic approximation
  double cubic_interp_inten(double arc_length);

  //: standard deviation residual intensities with respect to the cubic fit
  double cubic_fit_sigma(){return cubic_fit_sigma_;}

  //: the expected number of observations weighted by occlusion prob.
  double effective_n_obs(){return effective_n_obs_;}

  //:inherent variance in intensity found by dense linear interpolation
  double linear_interp_sigma();

  //:compute Gaussian probability density for the cubic fit
  double cubic_fit_prob_density();

  //=  model for an intensity function that is uncorrelated with view dir.==

  //: compute the autocorrelation function with intensity weighted by vis.
  void compute_auto_correlation();

  //: autocorrelation function
  std::vector<double> auto_correlation(){return auto_corr_;}

  //: fit a linear/constant model to the autocorrelation function
  void fit_linear_const();

  //: switch point (arc length) between linear and const
  double tau_s(){return tau_s_;}

  //: linear coefficient
  double alpha(){return alpha_;}

  //: const coefficient
  double mu(){return mu_;}

  //: sigma of linear const fit to the autocorrelation function
  double lin_const_fit_sigma(){return lin_const_sigma_;}

  //: interpolate the autocorrelation function with the linear/const model
  double interp_linear_const(double arc_length);

  //: compute frequency components of autocorrelation function
  void auto_corr_freq_amplitudes(std::vector<double>& freq_amplitudes);

  //: probability density of the linear/const fit
  double lin_const_fit_prob_density();

  //: probability density for maximum Fourier amplitude of the autocorrelation
  double max_frequency_prob_density();

 private:
  std::vector<double> elev_;
  std::vector<double> azimuth_;
  std::vector<double> vis_;
  std::vector<double> intensity_;
  vnl_double_4 cubic_coef_int_;
  double cubic_fit_sigma_;
  double inherent_sigma_;
  double effective_n_obs_;
  std::vector<double> auto_corr_;
  double tau_s_;
  double alpha_;
  double mu_;
  double lin_const_sigma_;
  double max_freq_amplitude_;
  double max_freq_mean_;
  double max_freq_sigma_;
};

#endif
