// This is mul/pdf1d/pdf1d_flat_sampler.cxx

//:
// \file
// \author Tim Cootes
// \brief Sampler class for Univariate flat distributions

#include "pdf1d_flat_sampler.h"

#include <vcl_cassert.h>
#include <pdf1d/pdf1d_flat.h>

//=======================================================================
// Dflt ctor
//=======================================================================

pdf1d_flat_sampler::pdf1d_flat_sampler():
 rng_(9667566ul)
{
}

//=======================================================================
// Destructor
//=======================================================================

pdf1d_flat_sampler::~pdf1d_flat_sampler()
{
}


//=======================================================================
// Method: is_a
//=======================================================================

vcl_string pdf1d_flat_sampler::is_a() const
{
  return vcl_string("pdf1d_flat_sampler");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool pdf1d_flat_sampler::is_class(vcl_string const& s) const
{
  return pdf1d_sampler::is_class(s) || s==pdf1d_flat_sampler::is_a();
}

//=======================================================================
// Method: clone
//=======================================================================

pdf1d_sampler* pdf1d_flat_sampler::clone() const
{
  return new pdf1d_flat_sampler(*this);
}
//=======================================================================

void pdf1d_flat_sampler::reseed(unsigned long seed)
{
  rng_.reseed(seed);
}
//=======================================================================

//: Set model for which this is an instance
// Error check that it is an axis flat.
void pdf1d_flat_sampler::set_model(const pdf1d_pdf& model)
{
  assert(model.is_class("pdf1d_flat"));
  // cannot use dynamic_cast<> without rtti - PVr
  // rtti currently turned off
  pdf1d_sampler::set_model(model);
}

//=======================================================================

double pdf1d_flat_sampler::sample()
{
  const pdf1d_flat & flat = static_cast<const pdf1d_flat &>(model());
  return  rng_.drand64(flat.lo(),flat.hi());
}

//: Fill x with samples possibly chosen so as to represent the distribution
//  5 or fewer samples requested, they are spaced out equally.
void pdf1d_flat_sampler::regular_samples(vnl_vector<double>& x)
{
  int n = x.size();
  const pdf1d_flat & flat = static_cast<const pdf1d_flat &>(model());
  double lo = flat.lo();
  double w = flat.hi()-lo;

  double f = w/(n+1);
  lo+=f;
  for (int i=0;i<n;++i)
    x[i] = lo+f*i;
}


//=======================================================================


//: Return a reference to the pdf model
// This is properly cast.
const pdf1d_flat& pdf1d_flat_sampler::flat() const
{
  return static_cast<const pdf1d_flat&>(model());
}

