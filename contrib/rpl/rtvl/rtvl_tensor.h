/* Copyright 2007-2010 Brad King
   Copyright 2007-2008 Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_tensor_h
#define rtvl_tensor_h

template <class T, unsigned int n> class vnl_vector_fixed;
template <class T, unsigned int nr, unsigned int nc> class vnl_matrix_fixed;

#include <vnl/vnl_vector_fixed.h>

#include <rgtl/rgtl_serialize_access.h>
#include <rgtl/rgtl_serialize_vnl_vector_fixed.h>

template <unsigned int N>
class rtvl_tensor
{
public:
  /** Construct with a unit ball tensor.  */
  rtvl_tensor();

  /** Construct with a given tensor.  */
  explicit rtvl_tensor(vnl_matrix_fixed<double, N, N> const& tensor);

  /** Store a given tensor.  */
  void set_tensor(vnl_matrix_fixed<double, N, N> const& tensor);

  /** Store a canonical ball tensor.  */
  void set_ball_tensor();

  /** Scale to make the largest eigenvalue equal to 1.  */
  void normalize();

  /** Scale to make the largest eigenvalue at most the given limit.  */
  void limit_saliency(double limit = 1);

  /** Adjust the tensor towards a ball to move to the next scale.  */
  void next_scale(double scale_multiplier, double max_saliency);

  double lambda(unsigned int d) const
    { return this->lambda_[d]; }

  double saliency(unsigned int d) const
    { return this->lambda_[d] - this->lambda_[d+1]; }

  /** Remove the ballness from the tensor.  */
  void remove_ballness(unsigned int d);

  typedef vnl_vector_fixed<double, N> basis_type[N];
  basis_type const& basis() const
    { return this->basis_; }
  vnl_vector_fixed<double, N> const& basis(unsigned int d) const
    { return this->basis_[d]; }

private:
  basis_type basis_;
  double lambda_[N+1];

private:
  friend class rgtl_serialize_access;
  template <class Serializer>
  void serialize(Serializer& sr)
    {
    sr & basis_;
    sr & lambda_;
    }
};

#endif
