#ifndef pdf1d_bhat_overlap_h
#define pdf1d_bhat_overlap_h
//:
// \file
// \author Tim Cootes
// \brief Functions to calculate Bhattacharyya overlap.

#include <pdf1d/pdf1d_pdf.h>
// not used? #include <pdf1d/pdf1d_gaussian.h>

//: Estimate Bhattacharyya overlap between two pdfs
//  If use_analytic is true and an analytic form exists, it will be used.
//  Otherwise n_samples are drawn from pdf1 and used to estimate the overlap
double pdf1d_bhat_overlap(const pdf1d_pdf& pdf1, const pdf1d_pdf& pdf2,
                          int n_samples=0, bool use_analytic=true);

// Bhat. overlap between a pdf and a sampled distribution.
// Second distribution is known to have pdf of p[i] when evaluated at x[i]
// x[i] must be representative samples from the pdf (i.e. randomly sampled
// from it, or selected so as to be equally spread in cum.prob. space).
double pdf1d_bhat_overlap(const pdf1d_pdf& pdf,
                          const double* x,
                          const double* p, int n);

// Bhat. overlap between a Gaussian and a sampled distribution.
// Second distribution is known to have pdf of p[i] when evaluated at x[i]
// x[i] must be representative samples from the pdf (i.e. randomly sampled
// from it, or selected so as to be equally spread in cum.prob. space).
double pdf1d_bhat_overlap_gaussian(double mean, double variance,
                          const double* x,
                          const double* p, int n);

//: Bhat. overlap between two 1D Gaussians
double pdf1d_bhat_overlap_gaussians(double mean1, double var1, double mean2, double var2);

//: Bhat. overlap between two 1D Gaussians
double pdf1d_bhat_overlap_gaussians(const pdf1d_pdf& g1, const pdf1d_pdf& g2);

//: Bhat. overlap between Gaussian and arbitrary pdf (estimate by sampling at n points)
double pdf1d_bhat_overlap_gaussian_with_pdf(double mean, double var, const pdf1d_pdf& pdf, int n=100);

//: Bhat. overlap between Gaussian and arbitrary pdf (estimate by sampling at n points)
double pdf1d_bhat_overlap_gaussian_with_pdf(const pdf1d_pdf& gauss, const pdf1d_pdf& pdf, int n=100);

#endif // pdf1d_bhat_overlap_h
