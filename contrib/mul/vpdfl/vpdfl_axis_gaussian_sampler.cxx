// This is mul/vpdfl/vpdfl_axis_gaussian_sampler.cxx
//:
// \file
// \author Ian Scott
// \date 12-Apr-2001
// \brief Sampler class for Multi-Variate axis aligned Gaussian classes.

#include "vpdfl_axis_gaussian_sampler.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpdfl/vpdfl_axis_gaussian.h>

//=======================================================================
// Dflt ctor
//=======================================================================

vpdfl_axis_gaussian_sampler::vpdfl_axis_gaussian_sampler():
 rng_(9667566ul)
{
}

//=======================================================================
// Destructor
//=======================================================================

vpdfl_axis_gaussian_sampler::~vpdfl_axis_gaussian_sampler() = default;


//=======================================================================
// Method: is_a
//=======================================================================

std::string vpdfl_axis_gaussian_sampler::is_a() const
{
  return std::string("vpdfl_axis_gaussian_sampler");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool vpdfl_axis_gaussian_sampler::is_class(std::string const& s) const
{
  return vpdfl_sampler_base::is_class(s) || s==vpdfl_axis_gaussian_sampler::is_a();
}

//=======================================================================
// Method: clone
//=======================================================================

vpdfl_sampler_base* vpdfl_axis_gaussian_sampler::clone() const
{
  return new vpdfl_axis_gaussian_sampler(*this);
}
//=======================================================================

void vpdfl_axis_gaussian_sampler::reseed(unsigned long seed)
{
  rng_.reseed(seed);
}
//=======================================================================

//: Set model for which this is an instance
// Error check that it is an axis gaussian.
void vpdfl_axis_gaussian_sampler::set_model(const vpdfl_pdf_base& model)
{
  assert(model.is_class("vpdfl_axis_gaussian"));
  // cannot use dynamic_cast<> without rtti - PVr
  // rtti currently turned off
  vpdfl_sampler_base::set_model(model);
}

//=======================================================================

void vpdfl_axis_gaussian_sampler::sample(vnl_vector<double>& x)
{
  const auto & gauss = static_cast<const vpdfl_axis_gaussian &>( model());
  const double *s = gauss.sd().data_block();
  const double *m = gauss.mean().data_block();
  int n = gauss.n_dims();

  x.set_size(n);

  double* x_data = x.data_block();
  for (int i=0;i<n;++i)
    x_data[i] = m[i] + s[i]*rng_.normal();
}

//=======================================================================


//: Return a reference to the pdf model
// This is properly cast.
const vpdfl_axis_gaussian& vpdfl_axis_gaussian_sampler::axis_gaussian() const
{
  return static_cast<const vpdfl_axis_gaussian&>(model());
}
