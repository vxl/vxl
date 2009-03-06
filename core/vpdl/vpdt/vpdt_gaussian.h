// This is core/vpdl/vpdt/vpdt_gaussian.h
#ifndef vpdt_gaussian_h_
#define vpdt_gaussian_h_
//:
// \file
// \author Matthew Leotta
// \date March 5, 2009
// \brief A generic Gaussian distribution 
//
// \verbatim
// Modifications
//   None
// \endverbatim


#include <vpdl/vpdt/vpdt_field_traits.h>
#include <vpdl/vpdt/vpdt_field_default.h>
#include <vpdl/vpdt/vpdt_access.h>
#include <vpdl/vpdt/vpdt_eigen_sym_matrix.h>
#include <vpdl/vpdt/vpdt_norm_metric.h>
#include <vcl_limits.h>
#include <vnl/vnl_erf.h>


//: A Gaussian with variance independent in each dimension 
template<class F, 
         class Covar = typename vpdt_eigen_sym_matrix_gen<F>::type,
         class Metric = vpdt_norm_metric<F,Covar> >
class vpdt_gaussian 
{
public:
  //: The field type
  typedef F field_type;
  //: The covariance type
  typedef Covar covar_type;
  //: The metric type
  typedef Metric metric_type;

  //: the data type used for scalars
  typedef typename vpdt_field_traits<F>::scalar_type T;
  //: the data type used for vectors
  typedef typename vpdt_field_traits<F>::vector_type vector;
  //: the data type used for matrices
  typedef typename vpdt_field_traits<F>::matrix_type matrix;
  
  //: Constructor
  // Optionally initialize the dimension for when n==0.
  // Otherwise var_dim is ignored
  vpdt_gaussian(unsigned int var_dim = vpdt_field_traits<F>::dimension) 
  {
    vpdt_set_size(mean,var_dim);
    vpdt_set_size(covar,var_dim);
    vpdt_fill(mean,T(0));
    vpdt_fill(covar,T(0));
  }
  
  //: Constructor - from mean and variance
  vpdt_gaussian(const F& m, const covar_type& c) 
  : mean(m), covar(c) {}

  //: Return the dimension
  unsigned int dimension() const { return vpdt_size(mean); } 

  //: Evaluate the unnormalized density at a point
  // this must be multiplied by norm_const() to integrate to 1
  T density(const F& pt) const
  {    
    return static_cast<T>(vcl_exp(-sqr_mahal_dist(pt)/2));
  }
  
  //: Evaluate the probability density at a point
  T prob_density(const F& pt) const
  {
    T norm = norm_const();
    if(vnl_math_isinf(norm))
      return T(0);
  
    return static_cast<T>(norm * density(pt));
  }

  //: Evaluate the log unnormalized density at a point
  T log_density(const F& pt) const
  {    
    return static_cast<T>(-sqr_mahal_dist(pt)/2);
  };
  
  //: Evaluate the log probability density at a point
  T log_prob_density(const F& pt) const
  {
    T norm = norm_const();
    if(vnl_math_isinf(norm))
      return -vcl_numeric_limits<T>::infinity();
    
    return static_cast<T>(vcl_log(norm) - sqr_mahal_dist(pt)/2);
  };
  
  //: compute the normalization constant (independent of sample point).
  // Can be precomputed when evaluating at multiple points
  T norm_const() const
  { 
    const unsigned int d = dimension();
    const double two_pi = 2.0*vnl_math::pi;
    double two_pi_n = two_pi;
    for(unsigned int i=1; i<d; ++i)
      two_pi_n *= two_pi;
    
    return static_cast<T>(vcl_sqrt(1/(two_pi_n*Metric::covar_det(mean,covar))));
  }
  
  //: The squared Mahalanobis distance to this point
  // non-for efficiency
  T sqr_mahal_dist(const F& pt) const
  {    
    return Metric::sqr_distance(pt,mean,covar);
  }
  
  //: Evaluate the cumulative distribution function at a point
  // This is the integral of the density function from negative infinity
  // (in all dimensions) to the point in question
  T cumulative_prob(const vector& pt) const
  {    
    // FIXME: implement this
    // probably requires numerical integration
    return vcl_numeric_limits<T>::quiet_NaN(); 
  }

  //: Compute the mean of the distribution.
  void compute_mean(vector& m) const { m = mean; }
    
  //: Compute the covariance matrix of the distribution.
  void compute_covar(matrix& c) const
  {
    // use the metric to compute the covariance at the mean
    Metric::compute_covar(c, mean, covar);
  }

//==============================================================================
// member variables - public for efficiency
    
  //: the mean
  F mean;
  //: the matrix covariance
  covar_type covar;
}; 



#endif // vpdt_gaussian_h_
