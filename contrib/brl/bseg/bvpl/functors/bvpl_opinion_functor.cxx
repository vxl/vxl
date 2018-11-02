//:
// \file
#include <iostream>
#include <cmath>
#include "bvpl_opinion_functor.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


// Default constructor
bvpl_opinion_functor::bvpl_opinion_functor()
{
  this->init();
}

//:Initializes all local variables
void bvpl_opinion_functor::init()
{
  b_=1.0;
  u_=1.0;
  n_p_=1;
  n_m_=1;
}

void bvpl_opinion_functor::apply(bvxm_opinion& val, bvpl_kernel_dispatch& d)
{
  //std::cout << "-->>>B=" << val.b() << " U=" << val.u() << std::endl;

  if (d.c_ == 1) {
    if (n_p_ > 1)
      b_=std::pow(b_, n_p_/(n_p_+1))*std::pow(val.b(), 1/(n_p_+1));
     else
      b_=val.b();
    //std::cout << "++ n=" << n_p_ << "  B=" << b_ << std::endl<< std::endl;
    n_p_++;
  } else if (d.c_ == -1) {
    if (n_m_ > 1)
      u_=std::pow(u_, n_m_/(n_m_+1))*std::pow(val.u(), 1/(n_m_+1));
    else
      u_=val.u();
    //std::cout << "-- n=" << n_m_ << "  U=" << u_ << std::endl<< std::endl;
    n_m_++;
  }
}

bvxm_opinion bvpl_opinion_functor::result()
{
  float b = b_*u_;
  float u = 1 - b;
  bvxm_opinion result(u,b);

  //reset all variables
  init();

  return result;
}
