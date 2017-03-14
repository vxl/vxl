/* Copyright 2007-2010 Brad King
   Copyright 2007-2008 Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_weight_original_h
#define rtvl_weight_original_h

#include "rtvl_weight.h"

template <unsigned int N> class rtvl_terms;

template <unsigned int N>
class rtvl_weight_original: public rtvl_weight<N>
{
public:
  typedef rtvl_weight<N> derived;
  rtvl_weight_original(double gs = 1, double cc = 1);
  virtual void set_scale(double gs);
protected:
  virtual double compute_flat(rtvl_terms<N> const& terms);
  virtual void compute_flat_d(rtvl_terms<N> const& terms,
                              vnl_vector_fixed<double, N>& dwflat);
  virtual double compute_curved(rtvl_terms<N> const& terms);
  virtual void compute_curved_d(rtvl_terms<N> const& terms,
                                vnl_vector_fixed<double, N>& dwcurve);
  void set_scale_impl(double gs);
private:
  double cconst;
  double geodesic_scale2;
  double c;
  double theta;
  double arclen;
  double kappa;
};

#endif
