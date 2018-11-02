/* Copyright 2007-2010 Brad King
   Copyright 2007-2008 Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_weight_smooth_hxx
#define rtvl_weight_smooth_hxx

#include <iostream>
#include <cstdlib>
#include "rtvl_weight_smooth.h"

#include "rtvl_terms.h"
#include "rtvl_intpow.h"

#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//----------------------------------------------------------------------------
template <unsigned int N>
rtvl_weight_smooth<N>::rtvl_weight_smooth(double gs, unsigned int n):
  derived(gs)
{
  cos_power = 2*n;
  switch(n)
    {
    // n ==> 2*n-1
    case 1: this->intpow = rtvl_intpow_impl<double, 1>::compute; break;
    case 2: this->intpow = rtvl_intpow_impl<double, 3>::compute; break;
    case 3: this->intpow = rtvl_intpow_impl<double, 5>::compute; break;
    case 4: this->intpow = rtvl_intpow_impl<double, 7>::compute; break;
    case 5: this->intpow = rtvl_intpow_impl<double, 9>::compute; break;
    case 6: this->intpow = rtvl_intpow_impl<double, 11>::compute; break;
    case 7: this->intpow = rtvl_intpow_impl<double, 13>::compute; break;
    case 8: this->intpow = rtvl_intpow_impl<double, 15>::compute; break;
    default: std::abort();
    }
}

//----------------------------------------------------------------------------
template <unsigned int N>
double
rtvl_weight_smooth<N>
::compute_flat(rtvl_terms<N> const& terms)
{
  z = terms.vlen / geodesic_scale;
  if(z < 3)
    {
    double zm3 = z-3;
    double zm3p2 = zm3*zm3;
    zm3p3 = zm3p2*zm3;
    zm3p4 = zm3p3*zm3;
    return z*z*zm3p4/16;
    }
  else
    {
    return 0;
    }
}

//----------------------------------------------------------------------------
template <unsigned int N>
void
rtvl_weight_smooth<N>
::compute_flat_d(rtvl_terms<N> const& terms,
                 vnl_vector_fixed<double, N>& dwflat)
{
  if(z < 3)
    {
    dwflat = terms.vhat*(z*(zm3p4 + 2*z*zm3p3)/(8*geodesic_scale));
    }
  else
    {
    dwflat.fill(0.0);
    }
}

//----------------------------------------------------------------------------
template <unsigned int N>
double
rtvl_weight_smooth<N>
::compute_curved(rtvl_terms<N> const& terms)
{
  cos_theta_2nm1 = intpow(terms.cos_theta);
  wangle = terms.cos_theta * cos_theta_2nm1;
  return terms.wflat*wangle;
}

//----------------------------------------------------------------------------
template <unsigned int N>
void
rtvl_weight_smooth<N>
::compute_curved_d(rtvl_terms<N> const& terms,
                   vnl_vector_fixed<double, N>& dwcurve)
{
  vnl_vector_fixed<double, N> dwangle =
    terms.dtheta * (-terms.sin_theta*cos_power*cos_theta_2nm1);
  dwcurve = terms.dwflat*wangle + dwangle*terms.wflat;
}

//----------------------------------------------------------------------------
#define RTVL_WEIGHT_SMOOTH_INSTANTIATE(N) \
  template class rtvl_weight_smooth<N>

#endif
