// This is mul/pdf1d/pdf1d_gaussian_sampler.cxx

//:
// \file
// \author Tim Cootes
// \brief Sampler class for univariate Gaussian classes.

#include "pdf1d_gaussian_sampler.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <pdf1d/pdf1d_gaussian.h>

//=======================================================================
// Dflt ctor
//=======================================================================

pdf1d_gaussian_sampler::pdf1d_gaussian_sampler():
 rng_(9667566ul)
{
}

//=======================================================================
// Destructor
//=======================================================================

pdf1d_gaussian_sampler::~pdf1d_gaussian_sampler() = default;


//=======================================================================
// Method: is_a
//=======================================================================

std::string pdf1d_gaussian_sampler::is_a() const
{
  return std::string("pdf1d_gaussian_sampler");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool pdf1d_gaussian_sampler::is_class(std::string const& s) const
{
  return pdf1d_sampler::is_class(s) || s==pdf1d_gaussian_sampler::is_a();
}

//=======================================================================
// Method: clone
//=======================================================================

pdf1d_sampler* pdf1d_gaussian_sampler::clone() const
{
  return new pdf1d_gaussian_sampler(*this);
}
//=======================================================================

void pdf1d_gaussian_sampler::reseed(unsigned long seed)
{
  rng_.reseed(seed);
}
//=======================================================================

//: Set model for which this is an instance
// Error check that it is an axis gaussian.
void pdf1d_gaussian_sampler::set_model(const pdf1d_pdf& model)
{
  assert(model.is_class("pdf1d_gaussian"));
  // cannot use dynamic_cast<> without rtti - PVr
  // rtti currently turned off
  pdf1d_sampler::set_model(model);
}

//=======================================================================

double pdf1d_gaussian_sampler::sample()
{
  const auto & gauss = static_cast<const pdf1d_gaussian &>(model());
  return  gauss.mean() + gauss.sd()*rng_.normal();
}

//: Fill x with samples possibly chosen so as to represent the distribution
//  5 or fewer samples requested, they are spaced out equally.
void pdf1d_gaussian_sampler::regular_samples(vnl_vector<double>& x)
{
  int n = x.size();
  if (n>5)
  {
    pdf1d_sampler::regular_samples(x);
    return;
  }

  // Strictly should select samples so that CDF(x) is equally spread in [0,1]

  const auto & gauss = static_cast<const pdf1d_gaussian &>( model());
  double lim = gauss.sd()*(n-1)/2.0;
  double mean = gauss.mean();

  for (int i=0;i<n;++i)
    x[i] = mean+lim * (2*(double(i)/(n-1))-1);
}


//=======================================================================


//: Return a reference to the pdf model
// This is properly cast.
const pdf1d_gaussian& pdf1d_gaussian_sampler::gaussian() const
{
  return static_cast<const pdf1d_gaussian&>( model());
}
