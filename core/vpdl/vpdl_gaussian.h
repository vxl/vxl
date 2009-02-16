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
// Modifications
//   None
// \endverbatim

#include <vpdl/vpdl_gaussian_base.h>
#include <vpdl/vpdl_npower.h>
#include <vcl_limits.h>
#include <vnl/vnl_erf.h>

//: A Gaussian with variance independent in each dimension 
template<class T, unsigned int n=0>
class vpdl_gaussian : public vpdl_gaussian_base<T,n>
{
public:
  //: the data type used for vectors (e.g. the mean) 
  typedef typename vpdl_base_traits<T,n>::vector vector;
  //: the data type used for matrices (e.g. covariance)
  typedef typename vpdl_base_traits<T,n>::matrix matrix;
  //: the type used internally for covariance
  typedef matrix covar_type;
  
  //: Constructor
  // Optionally initialize the dimension for when n==0.
  // Otherwise var_dim is ignored
  vpdl_gaussian(unsigned int var_dim = n) : vpdl_gaussian_base<T,n>(var_dim) {}
  
  //: Constructor - from mean and variance
  vpdl_gaussian(const vector& mean, const covar_type& covar) 
  : vpdl_gaussian_base<T,n>(mean), covar_(covar) {}
  
  //: Destructor
  virtual ~vpdl_gaussian() {}
  
  //: Create a copy on the heap and return base class pointer
  virtual vpdl_base_traits<T,n>* clone() const
  {
    return new vpdl_gaussian<T,n>(*this);
  }
  
  //: Evaluate the probability density at a point
  virtual T prob_density(const vector& pt) const
  {
    return static_cast<T>(norm_const() * vcl_exp(-sqr_mahal_dist(pt)/2));
  }
  
  //: Evaluate the log probability density at a point
  virtual T log_prob_density(const vector& pt) const
  {
    return static_cast<T>(vcl_log(norm_const()) - sqr_mahal_dist(pt)/2);
  };
  
  //: compute the normalization constant (independent of sample point).
  // Can be precomputed when evaluating at multiple points
  // non-virtual for efficiency
  T norm_const() const
  { 
    const unsigned int d = this->dimension();
    double det = 1.0;
    // FIXME: compute the determinant
    
    return static_cast<T>(vcl_sqrt(1/(vpdl_two_pi_power(this->dimension())*
                                      det)));
  }
  
  //: The squared Mahalanobis distance to this point
  // non-virtual for efficiency
  T sqr_mahal_dist(const vector& pt) const
  {    
    const unsigned int d = this->dimension();
    T val = 1.0;
    // FIXME: implement this
    return val;
  }
  
  //: Evaluate the cumulative distribution function at a point
  // This is the integral of the density function from negative infinity
  // (in all dimensions) to the point in question
  virtual T cumulative_prob(const vector& pt) const
  {    
    // FIXME: implement this
    return 0.0; 
  }
  
  //: Access the scalar variance
  const covar_type& covariance() const { return covar_; }
  
  //: Set the scalar variance
  void set_covariance(const covar_type& covar) { covar_ = covar; }
  
  
  //: Compute the covariance of the distribution.
  // Should be the identity matrix times var_
  virtual void compute_covar(matrix& covar) const
  {
    covar = covar_;
  }
  
protected:
  //: the matrix covariance
  covar_type covar_;
}; 



#endif // vpdl_gaussian_h_
