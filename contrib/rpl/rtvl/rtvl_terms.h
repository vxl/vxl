/* Copyright 2007-2010 Brad King
   Copyright 2007-2008 Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_terms_h
#define rtvl_terms_h

#include <vnl/vnl_vector_fixed.h>

template <unsigned int N>
class rtvl_terms
{
public:
  /** Voter-to-votee vector.  */
  vnl_vector_fixed<double, N> v;
  vnl_vector_fixed<double, N> vhat;
  double vlen;
  double vlen_squared;

  /** Voter-to-votee angle.  */
  double cos_theta;
  double sin_theta;
  vnl_vector_fixed<double, N> dtheta;

  /** Weight terms.  */
  double wflat;
  double wcurve;
  vnl_vector_fixed<double, N> dwflat;
  vnl_vector_fixed<double, N> dwcurve;
};

#endif
