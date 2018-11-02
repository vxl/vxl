#include <fstream>
#include <cmath>
#include <iostream>
#include <algorithm>
#include "brad_synoptic_function_1d.h"
//
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_inverse.h>
#include <vnl/algo/vnl_svd.h>
#include <bsta/bsta_gauss_sd1.h>

static double variance_multiplier(double n_obs)
{
  if (n_obs<2)
    return 100.0;//shouldn't happen
  if (n_obs==2)
    return 79.79;
  double noff = n_obs-1.5;
  double npow = std::pow(noff, 1.55);
  return 1.28 + 20.0/(1.0+npow);
}

bool brad_synoptic_function_1d::load_samples(std::string const& path)
{
  std::ifstream is(path.c_str());
  if (!is.is_open())
    return false;
  unsigned npts;
  is >> npts;
  if (npts==0)
    return false;
  double x, y, z;
  is >> x >> y >> z;//skip point for now
  std::string img_name;
  for (unsigned i = 0; i<npts; ++i) {
    double inten ,vis , elev , azimuth;
    is >> img_name >> inten >> vis >> elev >> azimuth;
    if (inten<0) continue;
    intensity_.push_back(inten);
    vis_.push_back(vis);
    elev_.push_back(elev);
    azimuth_.push_back(azimuth);
  }
  this->fit_intensity_cubic();
  return true;
}

double brad_synoptic_function_1d::
angle(double elev0, double az0, double elev1, double az1)
{
  double se0 = std::sin(elev0), ce0 = std::cos(elev0);
  double sa0 = std::sin(az0), ca0 = std::cos(az0);
  double se1 = std::sin(elev1), ce1 = std::cos(elev1);
  double sa1 = std::sin(az1), ca1 = std::cos(az1);
  vnl_double_3 v0(se0*ca0,se0*sa0,ce0), v1(se1*ca1,se1*sa1,ce1);
  double dp = dot_product(v0, v1);
  return std::acos(dp);
}


double brad_synoptic_function_1d::arc_length(unsigned index)
{
    return azimuth_[index];
}

void brad_synoptic_function_1d::fit_intensity_cubic()
{
  unsigned n = this->size();
  // form X, W matrices, and y vector
  vnl_matrix<double> X(n,4, 1.0);
  vnl_matrix<double> W(n,n, 0.0);
  vnl_vector<double> y(n);

  double min_s = 1e99;
  double max_s = -1e99;

#ifdef USE_MIN_MAX_VIS
  double min_vis=0.0;
  double max_vis=0.0;
#endif

  for (unsigned r = 0; r<n;++r)
  {
    double s = this->arc_length(r);
    if (min_s > s)
    {
      min_s = s;
#ifdef USE_MIN_MAX_VIS
      min_vis = vis_[r];
#endif
    }
    if ( max_s < s)
    {
      max_s =s;
#ifdef USE_MIN_MAX_VIS
      max_vis = vis_[r];
#endif
    }
  }
  for (unsigned r = 0; r<n;++r) {
    double s = this->arc_length(r);
    X[r][1]=s; X[r][2]=s*s; X[r][3]=X[r][2]*s;
    W[r][r] = vis_[r]; y[r]=intensity_[r];
  }

  vnl_double_4 l;
  l[0] = 0;
  l[1] = min_s - max_s ;
  l[2] = min_s*min_s - max_s*max_s ;
  l[3] = min_s*min_s*min_s - max_s*max_s*max_s ;

#ifdef USE_MIN_MAX_VIS
  vnl_double_4x4 ll= std::min(min_vis, max_vis);
#else
  vnl_double_4x4 ll= outer_product<double>(l,l);
#endif
  //compute cubic coefficients
  vnl_matrix<double> Xt = X.transpose();
  vnl_matrix<double> Xtw = Xt*W;
  vnl_double_4x4 M = Xtw*X + ll;
  vnl_double_4 q = Xtw*y;
  vnl_double_4x4 Minv = vnl_inverse(M);
  cubic_coef_int_ = Minv*q;
  vnl_vector<double> error = W*(y-X*cubic_coef_int_);
  cubic_fit_sigma_ = std::sqrt(error.squared_magnitude());
  vnl_vector<double> diag = W.get_diagonal();
  effective_n_obs_ = diag.sum();
  if (effective_n_obs_ <2)
    cubic_fit_sigma_ = -1.0;
  else {
    double var = error.squared_magnitude();
    var *= variance_multiplier(effective_n_obs_);
    cubic_fit_sigma_ = std::sqrt(var/(effective_n_obs_-1));
  }
}


double brad_synoptic_function_1d::cubic_interp_inten(double arc_length)
{
  double s = arc_length;
  double s2 = s*s, s3 = s2*s;
  return  cubic_coef_int_[0]
        + cubic_coef_int_[1]*s
        + cubic_coef_int_[2]*s2
        + cubic_coef_int_[3]*s3;
}

double brad_synoptic_function_1d::linear_interp_sigma()
{
  unsigned n=this->size();
  if (n<3) return -1.0;
  double var = 0.0;
  double total_vis = 0.0;
  for (unsigned i = 1; i<n-1; ++i) {
    //note can cache these arc lengths for lower computation
    //but not bothering now
    double acm1 = this->arc_length(i-1);
    double acp1 = this->arc_length(i+1);
    double ac = this->arc_length(i);
    double delac = acp1-acm1;
    if (delac==0.0) return -1.0;
    double intm1 = intensity_[i-1];
    double intp1 = intensity_[i+1];
    double inti = intensity_[i];
    //linear interpolation
    double pred_int = (intp1-intm1)*(ac-acm1)/delac + intm1;
    //prediction error
    double pred_sq_error = (inti-pred_int)*(inti-pred_int);
    // minimum visibility
    double min_vis = vis_[i-1];
    if (vis_[i]<min_vis) min_vis = vis_[i];
    if (vis_[i+1]<min_vis) min_vis = vis_[i+1];
    var += min_vis*pred_sq_error;
    total_vis += min_vis;
  }
  if (total_vis == 0.0) return -1.0;
  else return std::sqrt(var/total_vis);
}

double brad_synoptic_function_1d::cubic_fit_prob_density()
{
  bsta_gauss_sd1 gauss(0.0, inherent_sigma_*inherent_sigma_);
  return gauss.prob_density(cubic_fit_sigma_);
}

void brad_synoptic_function_1d::compute_auto_correlation()
{
  // compute mean and standard deviation of the intensities
  unsigned int n = static_cast<int>(this->size());
  if (n<3) return;
  double avg = 0.0, intsq = 0.0, vissum = 0.0;
  for (unsigned i = 0; i<n; ++i) {
    double intv = intensity_[i], intvsq = intv*intv;
    avg += vis_[i]*intv;
    intsq += vis_[i]*intvsq;
    vissum += vis_[i];
  }
  //if (vissum <2.0) return;
  double mean = avg/vissum;
  double vsq = intsq-(mean*mean*vissum);
  double var = vsq/vissum;
  //do the autocorrelation, scan over 1/2 the sample size
    for (unsigned int k = 0; k<=n/2; ++k) {
      double cor =0.0, vsum = 0.0;
      for (unsigned int t = k; t<n; ++t) {
        double vis_min = vis_[t];
        if (vis_[t-k]<vis_min)
          vis_min = vis_[t-k];
        vsum+=vis_min;
        cor += (vis_min*(intensity_[t]-mean)*(intensity_[t-k]-mean));
      }
      if (vsum>0.0) {
        cor/=vsum;
        auto_corr_.push_back(cor/var);
      }
      else auto_corr_.push_back(0.0);
    }
}

void brad_synoptic_function_1d::fit_linear_const()
{
  unsigned n = this->size();
  // fit linear segment
  double tau = 0.0;
  unsigned i = 0;
  double sumt =0.0, sumtsq = 0.0;
  while (tau<=tau_s_) {
    tau = arc_length(i);
    double acor = auto_corr_[i];
    sumt += tau*(1.0-acor);
    sumtsq += tau*tau;
    ++i;
  }
  if (sumtsq==0.0) alpha_ = 0.0;
  else alpha_ = sumt/sumtsq;
  // fit constant segment starting at tau_s+
  double sumc = 0.0;
  double T = 0.0;
  for (; i<=n/2; ++i) {
    T+= 1.0;
    sumc += auto_corr_[i];
  }
  if (T==0.0)
    mu_ = 0.0;
  mu_ = sumc/T;
  // compute fit sigma
  double esq = 0.0, nd = 0.0;
  for (unsigned i = 0; i<=n/2; ++i) {
    double acl = arc_length(i);
    double acor =  auto_corr_[i];
    double iacor = interp_linear_const(acl);
    esq += (acor-iacor)*(acor-iacor);
    nd += 1.0;
  }
  if (nd<2.0) lin_const_sigma_ = -1.0;
  else {
    double mult = variance_multiplier(nd);
    double var = (mult*esq)/(nd-1.0);
    lin_const_sigma_ = std::sqrt(var);
  }
}

double brad_synoptic_function_1d::interp_linear_const(double arc_length)
{
  if (arc_length<=tau_s_)
    return 1.0-alpha_*arc_length;
  else
    return mu_;
}


void brad_synoptic_function_1d::
auto_corr_freq_amplitudes(std::vector<double>& freq_amplitudes)
{
  unsigned n = this->size();
  double temp = n/2;
  double norm = std::sqrt(1/temp);

  freq_amplitudes.clear();
  max_freq_amplitude_ = 0.0;
    // max frequency, half the number of samples in the autocorrelation function
  for (unsigned int k = 0; k<=n/4; ++k) {
    double ac = 0, as =0;//fourier coefficients
      for (unsigned int i = 0; i<=n/2; ++i) {
      double x = this->arc_length(i);
          double arg = x*k;
      ac += std::cos(arg)*auto_corr_[i];
      as += std::sin(arg)*auto_corr_[i];
    }

    // frequency amplitude
    double amp = norm*std::sqrt(ac*ac + as*as);
    if (amp>max_freq_amplitude_)
      max_freq_amplitude_ = amp;
    freq_amplitudes.push_back(amp);
  }
}

double brad_synoptic_function_1d::lin_const_fit_prob_density()
{
  bsta_gauss_sd1 gauss(0.0, inherent_sigma_*inherent_sigma_);
  return gauss.prob_density(lin_const_sigma_);
}

double brad_synoptic_function_1d::max_frequency_prob_density()
{
  bsta_gauss_sd1 gauss(max_freq_mean_, max_freq_sigma_*max_freq_sigma_);
  return gauss.prob_density(max_freq_amplitude_);
}
