// This is core/vpdl/vpdl_update_gaussian.h
#ifndef vpdl_update_gaussian_h_
#define vpdl_update_gaussian_h_
//:
// \file
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date February 17, 2009
// \brief Iterative updating of Gaussians
//
// \endverbatim

#include <vpdl/vpdl_gaussian_sphere.h>
#include <vpdl/vpdl_gaussian_indep.h>
#include <vpdl/vpdl_gaussian.h>
#include <vcl_algorithm.h>
#include <vcl_cassert.h>


//: Update the statistics given a 1D Gaussian distribution and a learning rate
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T>
void vpdl_update_gaussian(vpdl_gaussian<T,1>& gaussian, T rho,
                          const T& sample )
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = 1.0f - rho;
  // compute the updated mean
  const T& old_mean = gaussian.mean();

  T diff = sample - old_mean;
  T new_var = rho_comp * gaussian.covariance();
  new_var += (rho * rho_comp) * diff*diff;

  gaussian.set_covariance(new_var);
  gaussian.set_mean((old_mean) +  (rho * diff));
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T, unsigned n>
void vpdl_update_gaussian(vpdl_gaussian_sphere<T,n>& gaussian, T rho,
                          const typename vpdl_base_traits<T,n>::vector& sample )
{
  typedef typename vpdl_base_traits<T,n>::vector vector;
  
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = 1.0f - rho;
  // compute the updated mean
  const vector& old_mean = gaussian.mean();

  vector diff(sample - old_mean);
  T new_var = rho_comp * gaussian.covariance();
  new_var += (rho * rho_comp) * dot_product(diff,diff);

  gaussian.set_covariance(new_var);
  gaussian.set_mean((old_mean) +  (rho * diff));
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T, unsigned n>
void vpdl_update_gaussian(vpdl_gaussian_indep<T,n>& gaussian, T rho,
                          const typename vpdl_base_traits<T,n>::vector& sample )
{
  typedef typename vpdl_base_traits<T,n>::vector vector;
  
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = 1.0f - rho;
  // compute the updated mean
  const vector& old_mean = gaussian.mean();

  vector diff(sample - old_mean);

  vector new_covar(rho_comp * gaussian.covariance());
  new_covar += (rho * rho_comp) * element_product(diff,diff);

  gaussian.set_covariance(new_covar);
  gaussian.set_mean((old_mean) +  (rho * diff));
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class T, unsigned n>
void vpdl_update_gaussian(vpdl_gaussian<T,n>& gaussian, T rho,
                          const typename vpdl_base_traits<T,n>::vector& sample )
{
  typedef typename vpdl_base_traits<T,n>::vector vector;
  typedef typename vpdl_base_traits<T,n>::matrix matrix;
  
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = 1.0f - rho;
  // compute the updated mean
  const vector& old_mean = gaussian.mean();

  vector diff(sample - old_mean);

  // this is inefficient because we need to recompose the eigenvalue 
  // decomposition, add the matrices, and decompose again
  // it seems that there should be a better way
  matrix new_covar(rho_comp * gaussian.covariance());
  new_covar += (rho * rho_comp) * outer_product(diff,diff);

  gaussian.set_covariance(new_covar);
  gaussian.set_mean((old_mean) +  (rho * diff));
}


//-----------------------------------------------------------------------------
// The following versions allow for a lower limit on variances.
// If the same sample is observed repeatedly, the variances will
// converge to the minimum value parameter rather than zero.



//: element-wise minimum of vector.
template <class T, unsigned n>
typename vpdl_base_traits<T,n>::vector 
element_max(const typename vpdl_base_traits<T,n>::vector& a_vector,
            const T& b)
{
  const unsigned int dim = a_vector.size();
  typename vpdl_base_traits<T,n>::vector min_vector;
  vpdl_base_traits<T,n>::set_size(min_vector,dim);
  
  T* r = min_vector.data_block();
  const T* a = a_vector.data_block();
  for (unsigned i=0; i<dim; ++i, ++r, ++a)
    *r = vcl_max(*a,b);
  return min_vector;
}


//: element-wise minimum of vector.
template <class T, unsigned n>
typename vpdl_base_traits<T,n>::vector 
element_max(const typename vpdl_base_traits<T,n>::vector & a_vector,
            const typename vpdl_base_traits<T,n>::vector & b_vector)
{
  const unsigned int dim = a_vector.size();
  assert(dim == b_vector.size());
  typename vpdl_base_traits<T,n>::vector min_vector;
  vpdl_base_traits<T,n>::set_size(min_vector,dim);
  
  T* r = min_vector.data_block();
  const T* a = a_vector.data_block();
  const T* b = b_vector.data_block();
  for (unsigned i=0; i<n; ++i, ++r, ++a, ++b)
    *r = vcl_max(*a,*b);
  return min_vector;
}



//: Update the statistics given a Gaussian distribution and a learning rate
// \param min_var forces the variance to stay above this limit
template <class T, unsigned n>
inline void vpdl_update_gaussian(vpdl_gaussian_sphere<T,n>& gaussian, T rho,
                                 const typename vpdl_base_traits<T,n>::vector& sample,
                                 T min_var)
{
  vpdl_update_gaussian(gaussian, rho, sample);
  gaussian.set_covariance(vcl_max(gaussian.covariance(),min_var));
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \param min_var forces all the variances to stay above this limit
template <class T, unsigned n>
inline void vpdl_update_gaussian(vpdl_gaussian_indep<T,n>& gaussian, T rho,
                                 const typename vpdl_base_traits<T,n>::vector& sample,
                                 T min_var)
{
  vpdl_update_gaussian(gaussian, rho, sample);
  gaussian.set_covariance(element_max<T,n>(gaussian.covariance(),min_var));
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \param min_var forces each variance to stay above these limits
template <class T, unsigned n>
inline void vpdl_update_gaussian(vpdl_gaussian_indep<T,n>& gaussian, T rho,
                                 const typename vpdl_base_traits<T,n>::vector& sample,
                                 const typename vpdl_base_traits<T,n>::vector& min_var)
{
  vpdl_update_gaussian(gaussian, rho, sample);
  gaussian.set_covariance(element_max<T,n>(gaussian.covariance(),min_var));
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \param min_var forces the eigenvalues of covariance to stay above this limit
template <class T, unsigned n>
inline void vpdl_update_gaussian(vpdl_gaussian<T,n>& gaussian, T rho,
                                 const typename vpdl_base_traits<T,n>::vector& sample,
                                 T min_var)
{
  vpdl_update_gaussian(gaussian, rho, sample);
  gaussian.set_covar_eigenvals(element_max<T,n>(gaussian.covar_eigenvals(),min_var));
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \param min_var forces the eigenvalues of covariance to stay above these limits
template <class T, unsigned n>
inline void vpdl_update_gaussian(vpdl_gaussian<T,n>& gaussian, T rho,
                                 const typename vpdl_base_traits<T,n>::vector& sample,
                                 const typename vpdl_base_traits<T,n>::vector& min_var)
{
  vpdl_update_gaussian(gaussian, rho, sample);
  gaussian.set_covar_eigenvals(element_max<T,n>(gaussian.covar_eigenvals(),min_var));
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \param min_var forces the eigenvalues of covariance to stay above this limit
template <class T>
inline void vpdl_update_gaussian(vpdl_gaussian<T,1>& gaussian, T rho,
                                 T sample,
                                 T min_var)
{
  vpdl_update_gaussian(gaussian, rho, sample);
  gaussian.set_covar_eigenvals(vcl_max(gaussian.covar_eigenvals(),min_var));
}



#endif // vpdl_update_gaussian_h_
