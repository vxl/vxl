#ifndef rtvl_tensor_hxx
#define rtvl_tensor_hxx
// Copyright 2007-2010 Brad King
// Copyright 2007-2008 Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rtvl_license_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "rtvl_tensor.h"

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

//----------------------------------------------------------------------------
template <unsigned int N>
rtvl_tensor<N>::rtvl_tensor()
{
  this->set_ball_tensor();
}

//----------------------------------------------------------------------------
template <unsigned int N>
rtvl_tensor<N>::rtvl_tensor(vnl_matrix_fixed<double, N, N> const& tensor)
{
  this->set_tensor(tensor);
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_tensor<N>::set_tensor(vnl_matrix_fixed<double, N, N> const& tensor)
{
  vnl_symmetric_eigensystem<double> eig(tensor);
  for (unsigned int i=0; i < N; ++i)
  {
    this->lambda_[i] = eig.get_eigenvalue((N-1)-i);
    this->basis_[i] = eig.get_eigenvector((N-1)-i);
  }
  this->lambda_[N] = 0;
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_tensor<N>::set_ball_tensor()
{
  for (unsigned int i=0; i < N; ++i)
  {
    this->lambda_[i] = 1;
    this->basis_[i].fill(0.0);
    this->basis_[i][i] = 1;
  }
  this->lambda_[N] = 0;
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_tensor<N>::normalize()
{
  if (this->lambda_[0] > 0)
  {
    for (unsigned int i=1; i < N; ++i)
    {
      this->lambda_[i] /= this->lambda_[0];
    }
    this->lambda_[0] = 1;
  }
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_tensor<N>::limit_saliency(double limit)
{
  if (this->lambda_[0] > limit)
  {
    double factor = limit/this->lambda_[0];
    for (unsigned int i=1; i < N; ++i)
    {
      this->lambda_[i] *= factor;
    }
    this->lambda_[0] = limit;
  }
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_tensor<N>::next_scale(double scale_multiplier, double max_saliency)
{
  double const z = 1.0/scale_multiplier;
  if (z < 3 && this->lambda_[0] > 0.0)
  {
    // Downweight old information with increasing scale change.
    double const w = z*z*(z-3)*(z-3)*(z-3)*(z-3)/16;

    // Smoothly scale the old information to limit its saliency.
    double f = max_saliency / this->lambda_[0];
    if (f > 1)
    {
      double const t = 1.0 / f;
      double const t2 = t*t;
      double const h = 3*t2 - 2*t*t2;
      f = f * h;
    }

    // Combine the scaled old information with a ball tensor.
    double const factor = w * f;
    for (unsigned int i=0; i < N; ++i)
    {
      this->lambda_[i] = 1 + this->lambda_[i] * factor;
    }
  }
  else
  {
    // Current information is useless for the next scale, so use a
    // canonical ball tensor.
    this->set_ball_tensor();
  }
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_tensor<N>::remove_ballness(unsigned int d)
{
  this->lambda_[d] = 0;
}

//----------------------------------------------------------------------------
#define RTVL_TENSOR_INSTANTIATE(N) \
  template class rtvl_tensor<N >

#endif
