// This is mul/pdf1d/pdf1d_gaussian_kernel_pdf_sampler.cxx
#include <iostream>
#include <string>
#include "pdf1d_gaussian_kernel_pdf_sampler.h"
//:
// \file
// \brief Implements sampling for a gaussian_kernel_pdf model pdf sampler
// \author Tim Cootes and Ian Scott
//
//=======================================================================

#include <vcl_compiler.h>
#include <vnl/vnl_vector.h>
#include <pdf1d/pdf1d_sampler.h>

//=======================================================================

pdf1d_gaussian_kernel_pdf_sampler::pdf1d_gaussian_kernel_pdf_sampler():
  rng_(9667566ul)
{
}

pdf1d_gaussian_kernel_pdf_sampler::~pdf1d_gaussian_kernel_pdf_sampler()
{
}

//=======================================================================

const pdf1d_gaussian_kernel_pdf& pdf1d_gaussian_kernel_pdf_sampler::gaussian_kernel_pdf() const
{
  return static_cast<const pdf1d_gaussian_kernel_pdf&>(model());
}


// ====================================================================

//: For generating plausible examples
double pdf1d_gaussian_kernel_pdf_sampler::sample()
{
  const pdf1d_gaussian_kernel_pdf& kpdf = gaussian_kernel_pdf();

  int n = kpdf.centre().size();

  int i = 0;
  if (n>1) i=rng_.lrand32(0,n-1);

  double x = rng_.normal();

  x*=kpdf.width()[i];
  x+=kpdf.centre()[i];

  return x;
}

//: Fill x with samples possibly chosen so as to represent the distribution
//  Samples equal numbers from each kernel.
void pdf1d_gaussian_kernel_pdf_sampler::regular_samples(vnl_vector<double>& x)
{
  const pdf1d_gaussian_kernel_pdf& kpdf = gaussian_kernel_pdf();
  int n = x.size();
  double* x_data = x.data_block();
  int nk = kpdf.centre().size();
  int n_per_k = n/nk;
  double lim = (n_per_k-1)/2.0;

  const double* c = kpdf.centre().data_block();
  const double* w = kpdf.width().data_block();

  for (int i=0;i<n;++i)
  {
    // Select components in order
    int j = i%nk;

    if (n_per_k>5)
    {
       x_data[i] = c[j]+rng_.normal()*w[j];
    }
    else
    {
      // Spread points about
      // Note that this isn't quite right - should be equally spaced in CDF space
      int a = j/nk;
      double f = double(a)/(n_per_k-1);  // in [0,1]
      x_data[i] = c[j] + lim*(2*f-1)*w[j];
    }
  }
}

//=======================================================================

//: Reseeds the static random number generator (one per derived class)
void pdf1d_gaussian_kernel_pdf_sampler::reseed(unsigned long seed)
{
  rng_.reseed(seed);
}


//=======================================================================

std::string pdf1d_gaussian_kernel_pdf_sampler::is_a() const
{
  return std::string("pdf1d_gaussian_kernel_pdf_sampler");
}

//=======================================================================

bool pdf1d_gaussian_kernel_pdf_sampler::is_class(std::string const& s) const
{
  return pdf1d_sampler::is_class(s) || s==pdf1d_gaussian_kernel_pdf_sampler::is_a();
}

//=======================================================================

short pdf1d_gaussian_kernel_pdf_sampler::version_no() const
{
  return 1;
}

//=======================================================================

pdf1d_sampler* pdf1d_gaussian_kernel_pdf_sampler::clone() const
{
  return new pdf1d_gaussian_kernel_pdf_sampler(*this);
}


