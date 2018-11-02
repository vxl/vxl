// This is core/vpdl/vpdl_gaussian_indep.h
#ifndef vpdl_gaussian_indep_h_
#define vpdl_gaussian_indep_h_
//:
// \file
// \author Matthew Leotta
// \date February 11, 2009
// \brief A Gaussian with variance independent in each dimension
//
// \verbatim
//  Modifications
//   <None yet>
// \endverbatim

#include <limits>
#include <vpdl/vpdl_gaussian_base.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vpdl/vpdt/vpdt_gaussian.h>
#include <vpdl/vpdt/vpdt_probability.h>
#include <vpdl/vpdt/vpdt_log_probability.h>

//: A Gaussian with variance independent in each dimension
template<class T, unsigned int n=0>
class vpdl_gaussian_indep : public vpdl_gaussian_base<T,n>
{
 public:
  //: the data type used for vectors
  typedef typename vpdt_field_default<T,n>::type vector;
  //: the data type used for matrices
  typedef typename vpdt_field_traits<vector>::matrix_type matrix;
  //: the type used internally for covariance
  typedef vector covar_type;

  //: Constructor
  // Optionally initialize the dimension for when n==0.
  // Otherwise var_dim is ignored
  vpdl_gaussian_indep(unsigned int var_dim = n)
  : impl_(var_dim) {}

  //: Constructor - from mean and variance
  vpdl_gaussian_indep(const vector& mean_val, const covar_type& var)
  : impl_(mean_val,var) {}

  //: Destructor
  virtual ~vpdl_gaussian_indep() {}

  //: Create a copy on the heap and return base class pointer
  virtual vpdl_distribution<T,n>* clone() const
  {
    return new vpdl_gaussian_indep<T,n>(*this);
  }

  //: Return the run time dimension, which does not equal \c n when \c n==0
  virtual unsigned int dimension() const { return impl_.dimension();  }

  //: Evaluate the unnormalized density at a point
  virtual T density(const vector& pt) const
  {
    return impl_.density(pt);
  }

  //: Evaluate the probability density at a point
  virtual T prob_density(const vector& pt) const
  {
    return vpdt_prob_density(impl_,pt);
  }

  //: Evaluate the log probability density at a point
  virtual T log_prob_density(const vector& pt) const
  {
    return vpdt_log_prob_density(impl_,pt);
  };

  //: Compute the gradient of the unnormalized density at a point
  // \return the density at \a pt since it is usually needed as well, and
  //         is often trivial to compute while computing gradient
  // \retval g the gradient vector
  virtual T gradient_density(const vector& pt, vector& g) const
  {
    return impl_.gradient_density(pt,g);
  }

  //: The normalization constant for the density
  // When density() is multiplied by this value it becomes prob_density
  // norm_const() is reciprocal of the integral of density over the entire field
  virtual T norm_const() const
  {
    return impl_.norm_const();
  }

  //: The squared Mahalanobis distance to this point
  // Non-virtual for efficiency
  T sqr_mahal_dist(const vector& pt) const
  {
    return impl_.sqr_mahal_dist(pt);
  }

  //: Evaluate the cumulative distribution function at a point
  // This is the integral of the density function from negative infinity
  // (in all dimensions) to the point in question
  virtual T cumulative_prob(const vector& pt) const
  {
    return impl_.cumulative_prob(pt);
  }

  //: Access the mean directly
  virtual const vector& mean() const { return impl_.mean; }

  //: Set the mean
  virtual void set_mean(const vector& mean_val) { impl_.mean = mean_val; }

  //: Compute the mean of the distribution.
  virtual void compute_mean(vector& mean_val) const { mean_val = impl_.mean; }

  //: Access the vector of variance
  const covar_type& covariance() const { return impl_.covar; }

  //: Set the vector of variance
  void set_covariance(const covar_type& var) { impl_.covar = var; }

  //: Compute the covariance of the distribution.
  // Should be the diagonal matrix of var_
  virtual void compute_covar(matrix& covar) const
  {
    impl_.compute_covar(covar);
  }

 protected:
  //: the Gaussian implementation from vpdt
  vpdt_gaussian<vector,covar_type> impl_;
};

#endif // vpdl_gaussian_indep_h_
