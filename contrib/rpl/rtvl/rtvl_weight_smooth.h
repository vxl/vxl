/* Copyright 2007-2010 Brad King
   Copyright 2007-2008 Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_weight_smooth_h
#define rtvl_weight_smooth_h

#include "rtvl_weight.h"

template <unsigned int N> class rtvl_terms;

template <unsigned int N>
class rtvl_weight_smooth: public rtvl_weight<N>
{
public:
  typedef rtvl_weight<N> derived;
  rtvl_weight_smooth(double gs = 1, unsigned int n = 4);
protected:
  virtual double compute_flat(rtvl_terms<N> const& terms);
  virtual void compute_flat_d(rtvl_terms<N> const& terms,
                              vnl_vector_fixed<double, N>& dwflat);
  virtual double compute_curved(rtvl_terms<N> const& terms);
  virtual void compute_curved_d(rtvl_terms<N> const& terms,
                                vnl_vector_fixed<double, N>& dwcurve);
private:
  using derived::geodesic_scale;
  double z;
  double zm3p3;
  double zm3p4;
  double wangle;
  double cos_theta_2nm1;
  double (*intpow)(double);
  unsigned int cos_power;
};

#endif
