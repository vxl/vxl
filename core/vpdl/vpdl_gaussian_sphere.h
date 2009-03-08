// This is core/vpdl/vpdl_gaussian_sphere.h
#ifndef vpdl_gaussian_sphere_h_
#define vpdl_gaussian_sphere_h_
//:
// \file
// \author Matthew Leotta
// \date February 11, 2009
// \brief A Gaussian with (hyper-)spherical covariance
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <vpdl/vpdl_gaussian_base.h>
#include <vnl/vnl_erf.h>
#include <vcl_limits.h>
#include <vcl_cassert.h>

#include <vpdl/vpdt/vpdt_gaussian.h>

//: A Gaussian with (hyper-)spherical covariance
template<class T, unsigned int n=0>
class vpdl_gaussian_sphere : public vpdl_gaussian_base<T,n>
{
 public:
  //: the data type used for vectors
  typedef typename vpdt_field_default<T,n>::type vector;
  //: the data type used for matrices
  typedef typename vpdt_field_traits<vector>::matrix_type matrix;
  //: the type used internally for covariance
  typedef T covar_type;

  //: Constructor
  // Optionally initialize the dimension for when n==0.
  // Otherwise var_dim is ignored
  vpdl_gaussian_sphere(unsigned int var_dim = n)
  : impl_(var_dim) {}

  //: Constructor - from mean and variance
  vpdl_gaussian_sphere(const vector& mean, const covar_type& var)
  : impl_(mean,var) {}

  //: Destructor
  virtual ~vpdl_gaussian_sphere() {}

  //: Create a copy on the heap and return base class pointer
  virtual vpdl_distribution<T,n>* clone() const
  {
    return new vpdl_gaussian_sphere<T,n>(*this);
  }

  //: Return the run time dimension, which does not equal \c n when \c n==0
  virtual unsigned int dimension() const { return impl_.dimension(); }

  //: Evaluate the unnormalized density at a point
  virtual T density(const vector& pt) const
  {
    return impl_.density(pt);
  }

  //: Evaluate the probability density at a point
  virtual T prob_density(const vector& pt) const
  {
    return impl_.prob_density(pt);
  }

  //: Evaluate the log probability density at a point
  virtual T log_prob_density(const vector& pt) const
  {
    return impl_.log_prob_density(pt);
  };

  //: The normalization constant for the density
  // When density() is multiplied by this value it becomes prob_density
  // norm_const() is reciprocal of the integral of density over the entire field
  T norm_const() const
  {
    return impl_.norm_const();
  }

  //: The squared Mahalanobis distance to this point
  // non-virtual for efficiency
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

  //: The probability of being in an axis-aligned box
  // The box is defined by two points, the minimum and maximum.
  // Reimplemented for effeciency since the axis are independent
  T box_prob(const vector& min_pt, const vector& max_pt) const
  {
    const unsigned int dim = this->dimension();

    double s2 = 1/vcl_sqrt(2*impl_.covar);
    // return zero for ill-defined box
    double prob = T(1);
    for (unsigned int i=0; i<dim; ++i){
      if (vpdt_index(max_pt,i)<=vpdt_index(min_pt,i))
        return T(0);
      prob *= (vnl_erf(s2*(vpdt_index(max_pt,i)-vpdt_index(impl_.mean,i))) -
               vnl_erf(s2*(vpdt_index(min_pt,i)-vpdt_index(impl_.mean,i))))/2;
    }
    return static_cast<T>(prob);
  }

  //: Access the mean directly
  virtual const vector& mean() const { return impl_.mean; }

  //: Set the mean
  virtual void set_mean(const vector& mean) { impl_.mean = mean; }

  //: Compute the mean of the distribution.
  virtual void compute_mean(vector& mean) const { mean = impl_.mean; }

  //: Access the scalar variance
  const covar_type& covariance() const { return impl_.covar; }

  //: Set the scalar variance
  void set_covariance(const covar_type& var) { impl_.covar = var; }


  //: Compute the covariance of the distribution.
  // Should be the identity matrix times var_
  virtual void compute_covar(matrix& covar) const
  {
    impl_.compute_covar(covar);
  }

 protected:
  //: the Gaussian implementation from vpdt
  vpdt_gaussian<vector,covar_type> impl_;
};


#endif // vpdl_gaussian_sphere_h_
