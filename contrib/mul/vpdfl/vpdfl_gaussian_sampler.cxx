// This is mul/vpdfl/vpdfl_gaussian_sampler.cxx
//:
// \file
// \author Ian Scott
// \date 12-Apr-2001
// \brief Sampler class for Multi-Variate Gaussian classes.

#include <iostream>
#include <cmath>
#include "vpdfl_gaussian_sampler.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpdfl/vpdfl_gaussian.h>
#include <mbl/mbl_matxvec.h>

//=======================================================================
// Dflt ctor
//=======================================================================

vpdfl_gaussian_sampler::vpdfl_gaussian_sampler():
 rng_(9667566ul)
{
}

//=======================================================================
// Destructor
//=======================================================================

vpdfl_gaussian_sampler::~vpdfl_gaussian_sampler() = default;


//=======================================================================
// Method: is_a
//=======================================================================

std::string vpdfl_gaussian_sampler::is_a() const
{
  return std::string("vpdfl_gaussian_sampler");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool vpdfl_gaussian_sampler::is_class(std::string const& s) const
{
  return vpdfl_sampler_base::is_class(s) || s==vpdfl_gaussian_sampler::is_a();
}

//=======================================================================


void vpdfl_gaussian_sampler::reseed(unsigned long seed)
{
  rng_.reseed(seed);
}

//=======================================================================


//: Return a reference to the pdf model
// This is properly cast.
const vpdfl_gaussian& vpdfl_gaussian_sampler::gaussian() const
{
  return static_cast<const vpdfl_gaussian&>(model());
}

//=======================================================================

void vpdfl_gaussian_sampler::sample(vnl_vector<double>& x)
{
  const vpdfl_gaussian& gauss = gaussian();

  const double *evals = gauss.eigenvals().data_block();

  int n = gauss.n_dims();

  // Generate random sample in coordinate frame of PCA
  b_.set_size(n);

  double* b_data = b_.data_block();
  for (int i=0;i<n;++i)
    b_data[i] = std::sqrt(evals[i])*rng_.normal();

  // Transform sample into x space
  mbl_matxvec_prod_mv(gauss.eigenvecs(),b_,x);
  x+=gauss.mean();
}

//=======================================================================
// Method: clone
//=======================================================================

vpdfl_sampler_base* vpdfl_gaussian_sampler::clone() const
{
  return new vpdfl_gaussian_sampler(*this);
}
