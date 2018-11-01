// This is oxl/osl/osl_edgel_chain.cxx
//:
// \file
// \author fsm

#include <iostream>
#include <cstdlib>
#include <new>
#include "osl_edgel_chain.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <osl/osl_hacks.h>

//--------------------------------------------------------------------------------

osl_edgel_chain::osl_edgel_chain() : n(0), x(nullptr), y(nullptr), grad(nullptr), theta(nullptr) { }

osl_edgel_chain::osl_edgel_chain(unsigned int n_)
  : n(n_)
  , x(new float[n fsm_pad])
  , y(new float[n fsm_pad])
  , grad(new float[n fsm_pad])
  , theta(new float[n fsm_pad])
{
}

osl_edgel_chain::osl_edgel_chain(osl_edgel_chain const &that)
  : n(that.n)
  , x(new float[n fsm_pad])
  , y(new float[n fsm_pad])
  , grad(new float[n fsm_pad])
  , theta(new float[n fsm_pad])
{
  for (unsigned int i=0; i<n; ++i) {
    x[i] = that.x[i];
    y[i] = that.y[i];
    grad[i] = that.grad[i];
    theta[i] = that.theta[i];
  }
}

osl_edgel_chain& osl_edgel_chain::operator=(osl_edgel_chain const &that)
{
  std::cerr << __FILE__ ": assignment to osl_edgel_chain\n";
  if (this != &that) {
    this->~osl_edgel_chain();
    new (this) osl_edgel_chain(that);
  }
  return *this;
}

osl_edgel_chain::~osl_edgel_chain()
{
  n = 0;
  fsm_delete_array x; x = nullptr;
  fsm_delete_array y; y = nullptr;
  fsm_delete_array grad; grad = nullptr;
  fsm_delete_array theta; theta = nullptr;
}

float  osl_edgel_chain::GetGrad(unsigned int i) const { return grad[i]; }
float *osl_edgel_chain::GetGrad() const { return grad; }
float  osl_edgel_chain::GetTheta(unsigned int i) const { return theta[i]; }
float *osl_edgel_chain::GetTheta() const { return theta; }
float  osl_edgel_chain::GetX(unsigned int i) const { return x[i]; }
float *osl_edgel_chain::GetX() const { return x; }
float  osl_edgel_chain::GetY(unsigned int i) const { return y[i]; }
float *osl_edgel_chain::GetY() const { return y; }
void osl_edgel_chain::SetGrad(float v, unsigned int i) { grad[i] = v; }
void osl_edgel_chain::SetTheta(float v, unsigned int i) { theta[i] = v; }
void osl_edgel_chain::SetX(float v, unsigned int i) { x[i] = v; }
void osl_edgel_chain::SetY(float v, unsigned int i) { y[i] = v; }
unsigned int osl_edgel_chain::size() const { return n; }

void osl_edgel_chain::SetLength(unsigned int nn)
{
  if (nn <= n)
    n = nn;
  else
    std::abort();
}

void osl_edgel_chain::write_ascii(std::ostream &os) const
{
  os << n << std::endl; // length
  for (unsigned int i=0; i<n; ++i)
    os << x[i] << ' ' << y[i] << ' ' << grad[i] << ' ' << theta[i] << std::endl;
}

void osl_edgel_chain::read_ascii(std::istream &is)
{
  int n_ = -1;
  is >> std::ws >> n_;
  if (n_<0 || is.bad()) {
    std::cerr << __FILE__ ": failed to read length of osl_edgel_chain\n";
    return;
  }
  //SetLength(n_);
  this->~osl_edgel_chain();       // destruct
  new (this) osl_edgel_chain((unsigned int)n_); // construct

  for (unsigned int i=0; i<n; ++i)
    is >> std::ws >> x[i] >> y[i] >> grad[i] >> theta[i];
  if (is.bad()) {
    std::cerr << __FILE__ ": stream bad before end of osl_edgel_chain\n";
    return;
  }

  // if we get here, it's probably OK.
}
