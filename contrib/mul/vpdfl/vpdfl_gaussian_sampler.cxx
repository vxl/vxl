// This is mul/vpdfl/vpdfl_gaussian_sampler.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author IAn Scott
// \date 12-Apr-2001
// \brief Sampler class for Multi-Variate Gaussian classes.

#include "vpdfl_gaussian_sampler.h"

#include <vcl_cmath.h>
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

vpdfl_gaussian_sampler::~vpdfl_gaussian_sampler()
{
}


//=======================================================================
// Method: is_a
//=======================================================================

vcl_string vpdfl_gaussian_sampler::is_a() const
{
  return vcl_string("vpdfl_gaussian_sampler");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool vpdfl_gaussian_sampler::is_class(vcl_string const& s) const
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
  return (const vpdfl_gaussian&) model();
}

//=======================================================================

void vpdfl_gaussian_sampler::sample(vnl_vector<double>& x)
{
  const vpdfl_gaussian& gauss = gaussian();

  const double *evals = gauss.eigenvals().data_block();

  int n = gauss.n_dims();

  // Generate random sample in co-ord frame of PCA
  b_.resize(n);

  double* b_data = b_.data_block();
  for (int i=0;i<n;++i)
    b_data[i] = vcl_sqrt(evals[i])*rng_.normal();

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
