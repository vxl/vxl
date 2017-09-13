/* Copyright 2007-2010 Brad King
   Copyright 2007-2008 Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_tensor_d_hxx
#define rtvl_tensor_d_hxx

#include "rtvl_tensor_d.h"

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>

//----------------------------------------------------------------------------
template <unsigned int N>
rtvl_tensor_d<N>
::rtvl_tensor_d(vnl_matrix_fixed<double, N, N> const& tensor,
                vnl_matrix_fixed<double, N, N> const (&tensor_d)[N]):
  derived(tensor), tensor_d_(tensor_d)
{
}

//----------------------------------------------------------------------------
template <unsigned int N>
void
rtvl_tensor_d<N>::saliency_d(unsigned int d,
                             vnl_vector_fixed<double, N>& dsal) const
{
  vnl_vector_fixed<double, N> const& ev0 = this->basis(d);
  vnl_vector_fixed<double, N> const& ev1 = this->basis(d+1);
  for(unsigned int k=0; k < N; ++k)
    {
    dsal(k) = (dot_product(ev0, this->tensor_d_[k]*ev0) -
               dot_product(ev1, this->tensor_d_[k]*ev1));
    }
}

//----------------------------------------------------------------------------
#define RTVL_TENSOR_D_INSTANTIATE(N) \
  template class rtvl_tensor_d<N>

#endif
