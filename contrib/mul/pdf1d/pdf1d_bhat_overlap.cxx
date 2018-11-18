// This is mul/pdf1d/pdf1d_bhat_overlap.cxx
#include <iostream>
#include <cmath>
#include "pdf1d_bhat_overlap.h"
//:
// \file
// \author Tim Cootes
// \brief Functions to calculate Bhattacharyya overlap.

#include <pdf1d/pdf1d_sampler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <vnl/vnl_math.h>

//: Estimate Bhattacharyya overlap between two pdfs
//  If use_analytic is true and an analytic form exists, it will be used.
//  Otherwise n_samples are drawn from pdf1 and used to estimate the overlap
double pdf1d_bhat_overlap(const pdf1d_pdf& pdf1, const pdf1d_pdf& pdf2,
                          int n_samples, bool use_analytic)
{
  if (use_analytic)
  {
    // Check for known analytic cases
    if (pdf1.is_class("pdf1d_gaussian") && pdf2.is_class("pdf1d_gaussian"))
      return pdf1d_bhat_overlap_gaussians(pdf1,pdf2);
  }

  // Otherwise sample from pdf1 and use this to estimate the overlap

  vnl_vector<double> x(n_samples),p(n_samples);

  pdf1d_sampler* sampler = pdf1.new_sampler();
  sampler->regular_samples_and_prob(x,p);
  delete sampler;

  return pdf1d_bhat_overlap(pdf2,x.data_block(),p.data_block(),n_samples);
}

// Bhat. overlap between a pdf and a sampled distribution.
// Second distribution is known to have pdf of p[i] when evaluated at x[i]
// x[i] must be representative samples from the pdf (i.e. randomly sampled
// from it, or selected so as to be equally spread in cum.prob. space).
double pdf1d_bhat_overlap(const pdf1d_pdf& pdf,
                          const double* x,
                          const double* p, int n)
{
  // Use more efficient calculation for Gaussian case
  if (pdf.is_class("pdf1d_gaussian"))
    return pdf1d_bhat_overlap_gaussian(pdf.mean(),pdf.variance(),x,p,n);

  // Otherwise use a general form
  double sum = 0;
  for (int i=0;i<n;++i)
  {
    sum += std::sqrt( pdf(x[i])/p[i] );
  }

  return sum/n;
}

// Bhat. overlap between 1D Gaussian and sampled distribution.
double pdf1d_bhat_overlap_gaussian(double m, double v,
                                   const double* x,
                                   const double* p, int n)
{
  double k = vnl_math::one_over_sqrt2pi/std::sqrt(v);

  double sum = 0;
  for (int i=0;i<n;++i)
  {
    double dx=x[i]-m;
    double pgi = k*std::exp(-0.5*dx*dx/v);
    sum += std::sqrt(pgi/p[i]);
  }
  return sum/n;
}

//: Bhat. overlap between two 1D Gaussians
double pdf1d_bhat_overlap_gaussians(double m1, double v1,
                                    double m2, double v2)
{
  double dm = m1-m2;
  double k = std::sqrt(2*std::sqrt(v1*v2))/std::sqrt(v1+v2);
  return k * std::exp(-0.25*dm*dm/(v1+v2));
}

//: Bhat. overlap between two 1D Gaussians
double pdf1d_bhat_overlap_gaussians(const pdf1d_pdf& g1, const pdf1d_pdf& g2)
{
  return pdf1d_bhat_overlap_gaussians(g1.mean(),g1.variance(),g2.mean(),g2.variance());
}

//: Bhat. overlap between Gaussian and arbitrary pdf (estimate by sampling at n points)
double pdf1d_bhat_overlap_gaussian_with_pdf(double m, double v, const pdf1d_pdf& pdf, int n)
{
  if (pdf.is_class("pdf1d_gaussian"))
    return pdf1d_bhat_overlap_gaussians(m,v,pdf.mean(),pdf.variance());

  double k = vnl_math::one_over_sqrt2pi/std::sqrt(v);
  double sd = std::sqrt(v);

  // Place n samples along range [-3,3]*sd
  double dx = 6.0/(n-1);
  double x = -0.5*dx*(n-1);
  double sum = 0.0;
  for (int i=0;i<n;++i)
  {
    double pgi = k*std::exp(-0.5*x*x);
    sum += std::sqrt(pgi*pdf(m+sd*x));
    x += dx;
  }

  // Width of each bin = sd*dx in original space

  return sum*dx*sd;
}

//: Bhat. overlap between Gaussian and arbitrary pdf (estimate by sampling at n points)
double pdf1d_bhat_overlap_gaussian_with_pdf(const pdf1d_pdf& g, const pdf1d_pdf& pdf, int n)
{
  return pdf1d_bhat_overlap_gaussian_with_pdf(g.mean(),g.variance(),pdf,n);
}
