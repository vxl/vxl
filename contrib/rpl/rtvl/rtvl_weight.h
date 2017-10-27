/* Copyright 2007-2010 Brad King
   Copyright 2007-2008 Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_weight_h
#define rtvl_weight_h

template <class T, unsigned int n> class vnl_vector_fixed;

template <unsigned int N> class rtvl_terms;

template <unsigned int N>
class rtvl_weight
{
public:
  rtvl_weight(double gs): geodesic_scale(gs) {}
  virtual ~rtvl_weight() {}
  virtual void set_scale(double gs) { this->geodesic_scale = gs; }
  virtual double get_scale() const { return geodesic_scale; }
  virtual double compute_flat(rtvl_terms<N> const& terms) = 0;
  virtual void compute_flat_d(rtvl_terms<N> const& terms,
                              vnl_vector_fixed<double, N>& dwflat) = 0;
  virtual double compute_curved(rtvl_terms<N> const& terms) = 0;
  virtual void compute_curved_d(rtvl_terms<N> const& terms,
                                vnl_vector_fixed<double, N>& dwcurve) = 0;
protected:
  double geodesic_scale;
};

#endif
