// This is core/vpdl/vpdl_gaussian.h
#ifndef vpdl_gaussian_h_
#define vpdl_gaussian_h_
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
#include <vpdl/vpdt/vpdt_gaussian.h>
#include <vpdl/vpdt/vpdt_probability.h>
#include <vpdl/vpdt/vpdt_log_probability.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A Gaussian with variance independent in each dimension
template<class T, unsigned int n=0>
class vpdl_gaussian : public vpdl_gaussian_base<T,n>
{
 public:
  //: the data type used for vectors
  typedef typename vpdt_field_default<T,n>::type vector;
  //: the data type used for matrices
  typedef typename vpdt_field_traits<vector>::matrix_type matrix;
  //: the type used internally for covariance
  typedef matrix covar_type;

  //: Constructor
  // Optionally initialize the dimension for when n==0.
  // Otherwise var_dim is ignored
  vpdl_gaussian(unsigned int var_dim = n)
  : impl_(var_dim) {}

  //: Constructor - from mean and variance
  vpdl_gaussian(const vector& mean_val, const covar_type& covar)
  : impl_(mean_val,covar) {}

  //: Destructor
  virtual ~vpdl_gaussian() {}

  //: Create a copy on the heap and return base class pointer
  virtual vpdl_distribution<T,n>* clone() const
  {
    return new vpdl_gaussian<T,n>(*this);
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

  //: Access the covariance - requires computation
  covar_type covariance() const
  {
    covar_type M;
    impl_.compute_covar(M);
    return M;
  }

  //: Set the covariance matrix
  void set_covariance(const covar_type& covar)
  {
    impl_.covar.set_matrix(covar);
  }

  //: Compute the covariance of the distribution.
  virtual void compute_covar(matrix& covar) const
  {
    impl_.compute_covar(covar);
  }

  //: Access the eigenvectors of the covariance matrix
  const matrix& covar_eigenvecs() const { return impl_.covar.eigenvectors(); }

  //: Access the eigenvalues of the covariance matrix
  const vector& covar_eigenvals() const { return impl_.covar.eigenvalues(); }

  //: Set the eigenvectors of the covariance matrix
  void set_covar_eigenvecs(const matrix& m) { impl_.covar.set_eigenvectors(m); }

  //: Set the eigenvalues of the covariance matrix
  void set_covar_eigenvals(const vector& v) { impl_.covar.set_eigenvalues(v); }

 protected:
  //: the Gaussian implementation from vpdt
  vpdt_gaussian<vector> impl_;
};

#endif // vpdl_gaussian_h_
