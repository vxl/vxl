/* Copyright 2007-2010 Brad King
   Copyright 2007-2008 Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_tensor_d_h
#define rtvl_tensor_d_h

#include "rtvl_tensor.h"

template <unsigned int N>
class rtvl_tensor_d: public rtvl_tensor<N>
{
public:
  typedef rtvl_tensor<N> derived;

  /** Construct with a given tensor and its gradient.  */
  rtvl_tensor_d(vnl_matrix_fixed<double, N, N> const& tensor,
                vnl_matrix_fixed<double, N, N> const (&tensor_d)[N]);

  void saliency_d(unsigned int d,
                  vnl_vector_fixed<double, N>& dsal) const;

private:
  vnl_matrix_fixed<double, N, N> const (&tensor_d_)[N];
};

#endif
