/* Copyright 2007-2010 Brad King
   Copyright 2007-2008 Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_voter_hxx
#define rtvl_voter_hxx

#include "rtvl_voter.h"

#include "rtvl_tensor.h"

//----------------------------------------------------------------------------
template <unsigned int N>
rtvl_voter<N>::
rtvl_voter(vnl_vector_fixed<double, N> const& voter_location,
           rtvl_tensor<N> const& voter_tensor):
  location_(voter_location), tensor_(voter_tensor)
{
}

//----------------------------------------------------------------------------
template <unsigned int N>
double
rtvl_voter<N>::lambda(unsigned int d) const
{
  return this->tensor_.lambda(d);
}

//----------------------------------------------------------------------------
template <unsigned int N>
typename rtvl_voter<N>::basis_const_reference
rtvl_voter<N>::basis() const
{
  return this->tensor_.basis();
}

//----------------------------------------------------------------------------
template <unsigned int N>
vnl_vector_fixed<double, N> const&
rtvl_voter<N>::basis(unsigned int d) const
{
  return this->tensor_.basis(d);
}

//----------------------------------------------------------------------------
#define RTVL_VOTER_INSTANTIATE(N) \
  template class rtvl_voter<N>

#endif
