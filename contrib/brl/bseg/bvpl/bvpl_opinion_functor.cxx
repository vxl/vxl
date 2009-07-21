//:
// \file
#include "bvpl_opinion_functor.h"
#include <vcl_cmath.h>
#include <vcl_iostream.h>


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
  //vcl_cout << "-->>>B=" << val.b() << " U=" << val.u() << vcl_endl;

  if (d.c_ == 1) {
    if (n_p_ > 1) 
      b_=vcl_pow(b_, n_p_/(n_p_+1))*vcl_pow(val.b(), 1/(n_p_+1));
     else 
      b_=val.b();
    //vcl_cout << "++ n=" << n_p_ << "  B=" << b_ << vcl_endl<< vcl_endl;
    n_p_++;
  } else if (d.c_ == -1) {
    if (n_m_ > 1) 
      u_=vcl_pow(u_, n_m_/(n_m_+1))*vcl_pow(val.u(), 1/(n_m_+1));
    else 
      u_=val.u();
    //vcl_cout << "-- n=" << n_m_ << "  U=" << u_ << vcl_endl<< vcl_endl;
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
