/* Copyright 2007-2010 Brad King
   Copyright 2007-2008 Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_weight_original_hxx
#define rtvl_weight_original_hxx

#include "rtvl_weight_original.h"

#include "rtvl_terms.h"

#include <vnl/vnl_vector_fixed.h>

//----------------------------------------------------------------------------
template <unsigned int N>
rtvl_weight_original<N>::rtvl_weight_original(double gs, double cc):
  derived(gs), cconst(cc)
{
  this->set_scale_impl(gs);
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_weight_original<N>::set_scale_impl(double gs)
{
  geodesic_scale2 = gs*gs;

  // Curvature penalty constant.  It has units of (distance)^4.
  c = cconst*geodesic_scale2*geodesic_scale2;
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_weight_original<N>::set_scale(double gs)
{
  this->derived::set_scale(gs);
  this->set_scale_impl(gs);
}

//----------------------------------------------------------------------------
template <unsigned int N>
double
rtvl_weight_original<N>
::compute_flat(rtvl_terms<N> const& terms)
{
  return std::exp(-terms.vlen_squared/geodesic_scale2);
}

//----------------------------------------------------------------------------
template <unsigned int N>
void
rtvl_weight_original<N>
::compute_flat_d(rtvl_terms<N> const& terms,
                 vnl_vector_fixed<double, N>& dwflat)
{
  dwflat = terms.v*(-2*terms.wflat/geodesic_scale2);
}

//----------------------------------------------------------------------------
template <unsigned int N>
double
rtvl_weight_original<N>
::compute_curved(rtvl_terms<N> const& terms)
{
  // Angle between the votee and tangent direction.
  theta = std::asin(terms.sin_theta);

  // Geodesic arc length of the hypothesized curve.
  arclen = terms.vlen * theta / terms.sin_theta;

  // Curvature of the hypothesized curve.
  kappa = 2*terms.sin_theta / terms.vlen;

  // Strength of the vote.
  double sk2 = arclen*arclen + c*kappa*kappa;
  return std::exp(-sk2/geodesic_scale2);
}

//----------------------------------------------------------------------------
template <unsigned int N>
void
rtvl_weight_original<N>
::compute_curved_d(rtvl_terms<N> const& terms,
                   vnl_vector_fixed<double, N>& dwcurve)
{
  vnl_vector_fixed<double, N> darclen;
  vnl_vector_fixed<double, N> dkappa;
  darclen = (terms.vhat*(theta/terms.sin_theta) +
             terms.dtheta*((terms.vlen/terms.sin_theta) -
                            ((terms.vlen*theta*terms.cos_theta)/
                             (terms.sin_theta*terms.sin_theta))));
  dkappa = (terms.dtheta*(2*terms.cos_theta/terms.vlen) -
            terms.vhat*(2*terms.sin_theta/terms.vlen_squared));
  double scale = -2*terms.wcurve/geodesic_scale2;
  for(unsigned int k=0; k < N; ++k)
    {
    dwcurve(k) = (scale*(arclen*darclen[k]+c*kappa*dkappa[k]));
    }
}

//----------------------------------------------------------------------------
#define RTVL_WEIGHT_ORIGINAL_INSTANTIATE(N) \
  template class rtvl_weight_original<N>

#endif
