// This is mul/vpdfl/vpdfl_mixture_sampler.cxx
//=======================================================================
//
//  Copyright: (C) 1998 Victoria University of Manchester
//
//=======================================================================
#include "vpdfl_mixture_sampler.h"
//:
// \file
// \brief Implements a mixture model pdf sampler
// \author Tim Cootes
// \date 2-Feb-2000
//
// Modifications
// \verbatim
//    IMS   Converted to VXL 12 May 2000
// \endverbatim

//=======================================================================

#include <vcl_string.h>
#include <vpdfl/vpdfl_sampler_base.h>

//=======================================================================

void vpdfl_mixture_sampler::init()
{
}

vpdfl_mixture_sampler::vpdfl_mixture_sampler():
  rng_(9667566ul)
{
  init();
}

vpdfl_mixture_sampler::vpdfl_mixture_sampler(const vpdfl_mixture_sampler& m):
  vpdfl_sampler_base()
{
  init();
  *this = m;
}

vpdfl_mixture_sampler& vpdfl_mixture_sampler::operator=(const vpdfl_mixture_sampler& m)
{
  if (this==&m) return *this;

  delete_stuff();

  int n = m.inst_.size();
  inst_.resize(n);
  for (int i=0;i<n;++i)
    inst_[i] = m.inst_[i]->clone();

  return *this;
}

//=======================================================================

void vpdfl_mixture_sampler::delete_stuff()
{
  int n = inst_.size();
  for (int i=0;i<n;++i)
    delete inst_[i];
  inst_.resize(0);
}

vpdfl_mixture_sampler::~vpdfl_mixture_sampler()
{
  delete_stuff();
}

//=======================================================================

const vpdfl_mixture& vpdfl_mixture_sampler::mixture() const
{
  return static_cast<const vpdfl_mixture&>(model());
}

// ====================================================================

//: Set model for which this is an instance
void vpdfl_mixture_sampler::set_model(const vpdfl_pdf_base& m)
{
  vpdfl_sampler_base::set_model(m);

  const vpdfl_mixture& mix = mixture();

  int n = mix.n_components();
  delete_stuff();
  inst_.resize(n);
  for (int i=0;i<n;++i)
    inst_[i]=mix.components()[i]->new_sampler();
}


// ====================================================================

  // For generating plausible examples:
void vpdfl_mixture_sampler::sample(vnl_vector<double>& x)
{
  const vpdfl_mixture& mix = mixture();
  int max_comp = mix.n_components()-1;

  // Randomly choose a component according to the weights (assumed to sum to 1)
  double r = rng_.drand32(0.0, 1.0); // in [0,1]
  int i=0;
  r-=mix.weights()[i];
  while (r>0 && (i<max_comp))
  {
    i++;
    r-=mix.weights()[i];
  }

  inst_[i]->sample(x);
}

//=======================================================================

  //: Reseeds the static random number generator (one per derived class)
void vpdfl_mixture_sampler::reseed(unsigned long seed)
{
  rng_.reseed(seed);
  for (unsigned int i=0; i<inst_.size(); ++i)
    inst_[i]->reseed(rng_.lrand32());
}


//=======================================================================

vcl_string vpdfl_mixture_sampler::is_a() const
{
  return vcl_string("vpdfl_mixture_sampler");
}

//=======================================================================

bool vpdfl_mixture_sampler::is_class(vcl_string const& s) const
{
  return vpdfl_sampler_base::is_class(s) || s==vpdfl_mixture_sampler::is_a();
}

//=======================================================================

short vpdfl_mixture_sampler::version_no() const
{
  return 1;
}

//=======================================================================

vpdfl_sampler_base* vpdfl_mixture_sampler::clone() const
{
  return new vpdfl_mixture_sampler(*this);
}


//==================< end of file: vpdfl_mixture_sampler.cxx >====================
