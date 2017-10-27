/* Copyright 2007-2010 Brad King
   Copyright 2007-2008 Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_votee_h
#define rtvl_votee_h

template <class T, unsigned int n> class vnl_vector_fixed;
template <class T, unsigned int nr, unsigned int nc> class vnl_matrix_fixed;

template <unsigned int N> class rtvl_vote_internal;

template <unsigned int N>
class rtvl_votee
{
public:
  rtvl_votee(vnl_vector_fixed<double, N> const& votee_location,
             vnl_matrix_fixed<double, N, N>& votee_tensor);

  vnl_vector_fixed<double, N> const& location()
    { return this->location_; }

  virtual void go(rtvl_vote_internal<N>& vi, double saliency);
private:
  vnl_vector_fixed<double, N> const& location_;
  vnl_matrix_fixed<double, N, N>& tensor_;
};

#endif
