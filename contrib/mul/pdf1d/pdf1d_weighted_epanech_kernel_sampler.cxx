// This is mul/pdf1d/pdf1d_weighted_epanech_kernel_sampler.cxx
#include "pdf1d_weighted_epanech_kernel_sampler.h"
//:
// \file
// \brief Implements sampling for a weighted Epanechnikov kernel PDF
// \author Ian Scott

//=======================================================================

#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <pdf1d/pdf1d_weighted_epanech_kernel_pdf.h>
#include <pdf1d/pdf1d_epanech_kernel_pdf_sampler.h>

//=======================================================================

pdf1d_weighted_epanech_kernel_sampler::pdf1d_weighted_epanech_kernel_sampler():
  rng_(9667566ul)
{
}

//=======================================================================

pdf1d_weighted_epanech_kernel_sampler::~pdf1d_weighted_epanech_kernel_sampler()
{
}

//=======================================================================

const pdf1d_weighted_epanech_kernel_pdf& pdf1d_weighted_epanech_kernel_sampler::weighted_epanech_kernel_pdf() const
{
  return static_cast<const pdf1d_weighted_epanech_kernel_pdf&>(model());
}


// ====================================================================

// For generating plausible examples:
double pdf1d_weighted_epanech_kernel_sampler::sample()
{
// Need to deal with weights.

  const pdf1d_weighted_epanech_kernel_pdf& kpdf = weighted_epanech_kernel_pdf();

  int n = kpdf.centre().size();


  // Randomly choose a component according to the weights (assumed to sum to 1)
  double r = rng_.drand32(0.0, kpdf.sum_weights_);
  int i=0;
  r-=kpdf.weight()[i];
  while (r>0 && (i<n))
  {
    i++;
    r-=kpdf.weight()[i];
  }

  double x = pdf1d_epanech_kernel_pdf_sampler::epan_transform(rng_.drand32());

  x*=kpdf.width()[i];
  x+=kpdf.centre()[i];

  return x;
}

//: Fill x with samples possibly chosen so as to represent the distribution
//  Samples equal numbers from each kernel.
void pdf1d_weighted_epanech_kernel_sampler::regular_samples(vnl_vector<double>& x)
{
// Need to deal with weights.

  const pdf1d_weighted_epanech_kernel_pdf& kpdf = weighted_epanech_kernel_pdf();
  const unsigned n = x.size();
  double* x_data = x.data_block();
  const unsigned nk = kpdf.centre().size();
  int n_per_k = n/nk;
  double lim = (n_per_k-1)/2.0;

  const double* c = kpdf.centre().data_block();
  const double* w = kpdf.width().data_block();
  const double* s = kpdf.weight().data_block();

  double dist = 0; // This marks the distance (in number of samples)
                   // to the end of the next kernel.
  // In order to get a regular distribution consistent with the weights,
  // we use dist to measure our way through the kernels.
  // Think of dist as being split up in proportion to the weight values.

  unsigned int i=0;
  for (unsigned int kernel = 0; kernel<nk; ++kernel)
  {
    // n_this_k is approximately the number of samples for this kernel.
    // the exact number depends on the non-integer part of dist.
    // If we didn't do this, we would have a problem with accumulation of rounding errors,
    // causing us to find too many or too few samples when we reach the end of dist.
    double n_this_k = n * s[kernel] /  kpdf.sum_weights_;

    dist += n_this_k;

    if (n_this_k>5.5) // randomly sample
    {
      while (dist > double(i))
      {
        x_data[i] = c[kernel]+ w[kernel]*
          pdf1d_epanech_kernel_pdf_sampler::epan_transform(rng_.drand32());
        ++i;
      }
    }
    else
    {
      // Spread points about
      // Note that this isn't quite right - should be equally spaced in CDF space
      unsigned i_this_k = 0;
      while (dist > double(i))
      {
        double f = (double(i_this_k) - dist + n_this_k)/(n_this_k);  // in [0,1]
        assert(0<=f && f<=1);
        x_data[i] = c[kernel] + lim*(2*f-1)*w[kernel];
      ++i; ++i_this_k;
      }
    }
  }
  assert (vnl_math_abs(dist - double(n)) < 1.0e-10 * dist);
  assert (i == n);
}

//=======================================================================

  //: Reseeds the static random number generator (one per derived class)
void pdf1d_weighted_epanech_kernel_sampler::reseed(unsigned long seed)
{
  rng_.reseed(seed);
}

//=======================================================================

vcl_string pdf1d_weighted_epanech_kernel_sampler::is_a() const
{
  return vcl_string("pdf1d_weighted_epanech_kernel_sampler");
}

//=======================================================================

bool pdf1d_weighted_epanech_kernel_sampler::is_class(vcl_string const& s) const
{
  return pdf1d_sampler::is_class(s) || s==pdf1d_weighted_epanech_kernel_sampler::is_a();
}

//=======================================================================

short pdf1d_weighted_epanech_kernel_sampler::version_no() const
{
  return 1;
}

//=======================================================================

pdf1d_sampler* pdf1d_weighted_epanech_kernel_sampler::clone() const
{
  return new pdf1d_weighted_epanech_kernel_sampler(*this);
}

