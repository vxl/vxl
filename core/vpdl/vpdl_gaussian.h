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
#include <vpdl/vpdl_eigen_sym_matrix.h>
#include <vcl_limits.h>
#include <vnl/vnl_erf.h>

#include <vpdl/vpdl_gaussian_sphere.h>

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
  vpdl_gaussian(unsigned int var_dim = n) 
  : vpdl_gaussian_base<T,n>(var_dim), covar_(var_dim) {}
  
  //: Constructor - from mean and variance
  vpdl_gaussian(const vector& mean, const covar_type& covar) 
  : vpdl_gaussian_base<T,n>(mean), covar_(covar) {}
  
  //: Destructor
  virtual ~vpdl_gaussian() {}
  
  //: Create a copy on the heap and return base class pointer
  virtual vpdl_distribution<T,n>* clone() const
  {
    return new vpdl_gaussian<T,n>(*this);
  }
  
  //: Evaluate the probability density at a point
  virtual T prob_density(const vector& pt) const
  {
    T norm = norm_const();
    if(vnl_math_isinf(norm))
      return T(0);
    
    return static_cast<T>(norm * vcl_exp(-sqr_mahal_dist(pt)/2));
  }
  
  //: Evaluate the log probability density at a point
  virtual T log_prob_density(const vector& pt) const
  {
    T norm = norm_const();
    if(vnl_math_isinf(norm))
      return -vcl_numeric_limits<T>::infinity();
    
    return static_cast<T>(vcl_log(norm) - sqr_mahal_dist(pt)/2);
  };
  
  //: compute the normalization constant (independent of sample point).
  // Can be precomputed when evaluating at multiple points
  // non-virtual for efficiency
  T norm_const() const
  { 
    return static_cast<T>(vcl_sqrt(1/(vpdl_two_pi_power(this->dimension())*
                                      covar_.determinant())));
  }
  
  //: The squared Mahalanobis distance to this point
  // non-virtual for efficiency
  T sqr_mahal_dist(const vector& pt) const
  {    
    return covar_.inverse_quad_form(pt - this->mean_);
  }
  
  //: Evaluate the cumulative distribution function at a point
  // This is the integral of the density function from negative infinity
  // (in all dimensions) to the point in question
  virtual T cumulative_prob(const vector& pt) const
  {    
    // FIXME: implement this
    // probably requires numerical integration
    return vcl_numeric_limits<T>::quiet_NaN(); 
  }
  
  //: Access the covariance - requires computation
  covar_type covariance() const 
  { 
    covar_type M;
    compute_covar(M);
    return M; 
  }
  
  //: Set the covariance matrix
  void set_covariance(const covar_type& covar) { covar_.set_matrix(covar); }
  
  
  //: Compute the covariance of the distribution.
  virtual void compute_covar(matrix& covar) const
  {
    covar_.form_matrix(covar);
  }
  
  //: Access the eigenvectors of the covariance matrix
  const matrix& covar_eigenvecs() const { return covar_.eigenvectors(); }
  
  //: Access the eigenvalues of the covariance matrix
  const vector& covar_eigenvals() const { return covar_.eigenvalues(); }
  
  //: Set the eigenvectors of the covariance matrix
  void set_covar_eigenvecs(const matrix& m) { covar_.set_eigenvectors(m); }
  
  //: Set the eigenvalues of the covariance matrix
  void set_covar_eigenvals(const vector& v) { covar_.set_eigenvalues(v); }
  
protected:
  //: the matrix covariance
  vpdl_eigen_sym_matrix<T,n> covar_;
}; 



#endif // vpdl_gaussian_h_
