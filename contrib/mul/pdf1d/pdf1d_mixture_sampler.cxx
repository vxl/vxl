// This is mul/pdf1d/pdf1d_mixture_sampler.cxx
#include "pdf1d_sampler.h"
//:
// \file
// \brief Implements sampling for a mixture model pdf sampler
// \author Tim Cootes and Ian Scott

//=======================================================================

#include <vcl_string.h>
#include <pdf1d/pdf1d_mixture.h>
#include <pdf1d/pdf1d_mixture_sampler.h>

//=======================================================================

void pdf1d_mixture_sampler::init()
{
}

pdf1d_mixture_sampler::pdf1d_mixture_sampler():
  rng_(9667566ul)
{
  init();
}

pdf1d_mixture_sampler::pdf1d_mixture_sampler(const pdf1d_mixture_sampler& m):
  pdf1d_sampler()
{
  init();
  *this = m;
}

pdf1d_mixture_sampler& pdf1d_mixture_sampler::operator=(const pdf1d_mixture_sampler& m)
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

void pdf1d_mixture_sampler::delete_stuff()
{
  int n = inst_.size();
  for (int i=0;i<n;++i)
    delete inst_[i];
  inst_.resize(0);
}

pdf1d_mixture_sampler::~pdf1d_mixture_sampler()
{
  delete_stuff();
}

//=======================================================================

const pdf1d_mixture& pdf1d_mixture_sampler::mixture() const
{
  return static_cast<const pdf1d_mixture&>(model());
}

// ====================================================================

//: Set model for which this is an instance
void pdf1d_mixture_sampler::set_model(const pdf1d_pdf& m)
{
  pdf1d_sampler::set_model(m);

  const pdf1d_mixture& mix = mixture();

  int n = mix.n_components();
  delete_stuff();
  inst_.resize(n);
  for (int i=0;i<n;++i)
    inst_[i]=mix.components()[i]->new_sampler();
}


// ====================================================================

// For generating plausible examples:
double pdf1d_mixture_sampler::sample()
{
  const pdf1d_mixture& mix = mixture();
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

  return inst_[i]->sample();
}

//=======================================================================

  //: Reseeds the static random number generator (one per derived class)
void pdf1d_mixture_sampler::reseed(unsigned long seed)
{
  rng_.reseed(seed);
  for (unsigned int i=0; i<inst_.size(); ++i)
    inst_[i]->reseed(rng_.lrand32());
}


//=======================================================================

vcl_string pdf1d_mixture_sampler::is_a() const
{
  return vcl_string("pdf1d_mixture_sampler");
}

//=======================================================================

bool pdf1d_mixture_sampler::is_class(vcl_string const& s) const
{
  return pdf1d_sampler::is_class(s) || s==pdf1d_mixture_sampler::is_a();
}

//=======================================================================

short pdf1d_mixture_sampler::version_no() const
{
  return 1;
}

//=======================================================================

pdf1d_sampler* pdf1d_mixture_sampler::clone() const
{
  return new pdf1d_mixture_sampler(*this);
}


//==================< end of file: pdf1d_mixture_sampler.cxx >====================
