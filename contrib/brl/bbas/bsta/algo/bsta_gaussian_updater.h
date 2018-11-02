// This is brl/bbas/bsta/algo/bsta_gaussian_updater.h
#ifndef bsta_gaussian_updater_h_
#define bsta_gaussian_updater_h_
//:
// \file
// \brief Iterative updating of Gaussians
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date February 22, 2006
//
// \verbatim
//  Modifications
//   Jun 18, 2008 - Matt Leotta -- Adjusted such that min_var is a hard minimum
//                                 instead of a minimum in the limit
// \endverbatim

#include <iostream>
#include <algorithm>
#include <bsta/bsta_gaussian_sphere.h>
#include <bsta/bsta_gaussian_indep.h>
#include <bsta/bsta_gaussian_full.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_attributes.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Update the statistics given a 1D Gaussian distribution and a learning rate
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T>
void bsta_update_gaussian(bsta_gaussian_sphere<T,1>& gaussian, T rho,
                          const T& sample )
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = 1.0f - rho;
  // compute the updated mean
  const T& old_mean = gaussian.mean();

  T diff = sample - old_mean;
  T new_var = rho_comp * gaussian.var();
  new_var += (rho * rho_comp) * diff*diff;

  gaussian.set_var(new_var);
  gaussian.set_mean((old_mean) +  (rho * diff));
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T, unsigned n>
void bsta_update_gaussian(bsta_gaussian_sphere<T,n>& gaussian, T rho,
                          const vnl_vector_fixed<T,n>& sample )
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = 1.0f - rho;
  // compute the updated mean
  const vnl_vector_fixed<T,n>& old_mean = gaussian.mean();

  vnl_vector_fixed<T,n> diff(sample - old_mean);
  T new_var = rho_comp * gaussian.var();
  new_var += (rho * rho_comp) * dot_product(diff,diff);

  gaussian.set_var(new_var);
  gaussian.set_mean((old_mean) +  (rho * diff));
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T, unsigned n>
void bsta_update_gaussian(bsta_gaussian_indep<T,n>& gaussian, T rho,
                          const vnl_vector_fixed<T,n>& sample )
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = 1.0f - rho;
  // compute the updated mean
  const vnl_vector_fixed<T,n>& old_mean = gaussian.mean();

  vnl_vector_fixed<T,n> diff(sample - old_mean);

  vnl_vector_fixed<T,n> new_covar(rho_comp * gaussian.diag_covar());
  new_covar += (rho * rho_comp) * element_product(diff,diff);

  gaussian.set_covar(new_covar);
  gaussian.set_mean((old_mean) +  (rho * diff));
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T, unsigned n>
void bsta_update_gaussian(bsta_gaussian_full<T,n>& gaussian, T rho,
                          const vnl_vector_fixed<T,n>& sample )
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = 1.0f - rho;
  // compute the updated mean
  const vnl_vector_fixed<T,n>& old_mean = gaussian.mean();

  vnl_vector_fixed<T,n> diff(sample - old_mean);

  vnl_matrix_fixed<T,n,n> new_covar(rho_comp * gaussian.covar());
  new_covar += (rho * rho_comp) * outer_product(diff,diff);

  gaussian.set_covar(new_covar);
  gaussian.set_mean((old_mean) +  (rho * diff));
}


//-----------------------------------------------------------------------------
// The following versions allow for a lower limit on variances.
// If the same sample is observed repeatedly, the variances will
// converge to the minimum value parameter rather than zero.


template <class T>
inline T element_max(const T& a, const T& b)
{
  return std::max(a,b);
}


//: element-wise minimum of vector.
template <class T, unsigned n>
vnl_vector_fixed<T,n> element_max(const vnl_vector_fixed<T,n>& a_vector,
                                  const T& b)
{
  vnl_vector_fixed<T,n> min_vector;
  T* r = min_vector.data_block();
  const T* a = a_vector.data_block();
  for (unsigned i=0; i<n; ++i, ++r, ++a)
    *r = std::max(*a,b);
  return min_vector;
}


//: element-wise minimum of vector.
template <class T, unsigned n>
vnl_vector_fixed<T,n> element_max(const vnl_vector_fixed<T,n>& a_vector,
                                  const vnl_vector_fixed<T,n>& b_vector)
{
  vnl_vector_fixed<T,n> min_vector;
  T* r = min_vector.data_block();
  const T* a = a_vector.data_block();
  const T* b = b_vector.data_block();
  for (unsigned i=0; i<n; ++i, ++r, ++a, ++b)
    *r = std::max(*a,*b);
  return min_vector;
}


//: element-wise minimum on the matrix diagonal.
template <class T, unsigned n>
vnl_matrix_fixed<T,n,n> element_max(const vnl_matrix_fixed<T,n,n>& a_matrix,
                                    const T& b)
{
  vnl_matrix_fixed<T,n,n> min_matrix(a_matrix);
  T* r = min_matrix.data_block();
  const T* a = a_matrix.data_block();
  const unsigned step = n+1;
  for (unsigned i=0; i<n; ++i, r+=step, a+=step)
    *r = std::max(*a,b);
  return min_matrix;
}


//: element-wise minimum of matrix.
template <class T, unsigned n>
vnl_matrix_fixed<T,n,n> element_max(const vnl_matrix_fixed<T,n,n>& a_matrix,
                                    const vnl_matrix_fixed<T,n,n>& b_matrix)
{
  vnl_matrix_fixed<T,n,n> min_matrix;
  T* r = min_matrix.data_block();
  const T* a = a_matrix.data_block();
  const T* b = b_matrix.data_block();
  const unsigned num_elements = n*n;
  for (unsigned i=0; i<num_elements; ++i, ++r, ++a, ++b)
    *r = std::max(*a,*b);
  return min_matrix;
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \param min_covar forces the covariance to stay above this limit
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class gauss_>
inline void bsta_update_gaussian(gauss_& gaussian,
                                 typename gauss_::math_type rho,
                                 const typename gauss_::vector_type& sample,
                                 const typename gauss_::covar_type& min_covar)
{
  bsta_update_gaussian(gaussian, rho, sample);
  gaussian.set_covar(element_max(gaussian.covar(),min_covar));
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \param min_var forces all the variances to stay above this limit
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T, unsigned n>
inline void bsta_update_gaussian(bsta_gaussian_indep<T,n>& gaussian, T rho,
                                 const vnl_vector_fixed<T,n>& sample,
                                 T min_var)
{
  bsta_update_gaussian(gaussian, rho, sample);
  gaussian.set_covar(element_max(gaussian.covar(),min_var));
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \param min_var forces the diagonal covariance to stay above this limit
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T, unsigned n>
inline void bsta_update_gaussian(bsta_gaussian_full<T,n>& gaussian, T rho,
                                 const vnl_vector_fixed<T,n>& sample,
                                 T min_var)
{
  bsta_update_gaussian(gaussian, rho, sample);
  gaussian.set_covar(element_max(gaussian.covar(),min_var));
}


//-----------------------------------------------------------------------------


//: An updater for statistically updating Gaussian distributions
template <class gauss_>
class bsta_gaussian_updater
{
    typedef bsta_num_obs<gauss_> obs_gauss_;
    typedef typename gauss_::math_type T;
    typedef vnl_vector_fixed<T,gauss_::dimension> vector_;
  public:

    //: for compatibility with vpdl/vpdt
    typedef typename gauss_::field_type field_type;
    typedef gauss_ distribution_type;


    //: The main function
    // make the appropriate type casts and call a helper function
    void operator() ( obs_gauss_& d, const vector_& sample ) const
    {
      d.num_observations += T(1);
      bsta_update_gaussian(d, T(1)/d.num_observations, sample);
    }
};


//: An updater for updating Gaussian distributions with a moving window
// When the number of samples exceeds the window size the most recent
// samples contribute more toward the distribution.
template <class gauss_>
class bsta_gaussian_window_updater
{
    typedef bsta_num_obs<gauss_> obs_gauss_;
    typedef typename gauss_::math_type T;
    typedef vnl_vector_fixed<T,gauss_::dimension> vector_;
  public:

    //: for compatibility with vpdl/vpdt
    typedef typename gauss_::field_type field_type;
    typedef gauss_ distribution_type;


    //: Constructor
    bsta_gaussian_window_updater(unsigned int ws) : window_size(ws) {}

    //: The main function
    // make the appropriate type casts and call a helper function
    void operator() ( obs_gauss_& d, const vector_& sample) const
    {
      if (d.num_observations < window_size)
        d.num_observations += T(1);
      bsta_update_gaussian(d, T(1)/d.num_observations, sample);
    }

    unsigned int window_size;
};


#endif // bsta_gaussian_updater_h_
