/* Copyright 2007-2010 Brad King
   Copyright 2007-2008 Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_voter_h
#define rtvl_voter_h

template <class T, unsigned int n> class vnl_vector_fixed;
template <class T, unsigned int nr, unsigned int nc> class vnl_matrix_fixed;

template <unsigned int N> class rtvl_tensor;

template <unsigned int N>
class rtvl_voter
{
public:
  rtvl_voter(vnl_vector_fixed<double, N> const& voter_location,
             rtvl_tensor<N> const& voter_tensor);

  vnl_vector_fixed<double, N> const& location() const
    { return this->location_; }

  rtvl_tensor<N> const& tensor() const
    { return this->tensor_; }

  double lambda(unsigned int d) const;

  typedef vnl_vector_fixed<double, N> const (&basis_const_reference)[N];
  basis_const_reference basis() const;
  vnl_vector_fixed<double, N> const& basis(unsigned int d) const;

private:
  vnl_vector_fixed<double, N> const& location_;
  rtvl_tensor<N> const& tensor_;
};

#endif
