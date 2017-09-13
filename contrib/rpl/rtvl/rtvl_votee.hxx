/* Copyright 2007-2010 Brad King
   Copyright 2007-2008 Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_votee_hxx
#define rtvl_votee_hxx

#include "rtvl_votee.h"

#include "rtvl_vote.h"

#include <vnl/vnl_matrix_fixed.h>

//----------------------------------------------------------------------------
template <unsigned int N>
rtvl_votee<N>::
rtvl_votee(vnl_vector_fixed<double, N> const& votee_location,
           vnl_matrix_fixed<double, N, N>& votee_tensor):
  location_(votee_location), tensor_(votee_tensor)
{
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_votee<N>::go(rtvl_vote_internal<N>& vi, double saliency)
{
  vnl_matrix_fixed<double, N, N> vote;
  rtvl_vote_component(vi, vote);
  vote *= saliency;
  this->tensor_ += vote;
}

//----------------------------------------------------------------------------
#define RTVL_VOTEE_INSTANTIATE(N) \
  template class rtvl_votee<N>

#endif
