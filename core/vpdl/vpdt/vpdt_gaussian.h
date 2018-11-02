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
//   <None yet>
// \endverbatim


#include <limits>
#include <vpdl/vpdt/vpdt_field_traits.h>
#include <vpdl/vpdt/vpdt_field_default.h>
#include <vpdl/vpdt/vpdt_dist_traits.h>
#include <vpdl/vpdt/vpdt_access.h>
#include <vpdl/vpdt/vpdt_eigen_sym_matrix.h>
#include <vpdl/vpdt/vpdt_norm_metric.h>
#include <vnl/vnl_math.h> // for twopi
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_erf.h>


//: Forward declare integration helper struct
template <class F, class Covar, class Metric, class Disambiguate= void >
struct vpdt_gaussian_integrator;


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

  //: Evaluate the unnormalized density at a point \a pt
  // This must be multiplied by norm_const() to integrate to 1
  T density(const F& pt) const
  {
    return static_cast<T>(std::exp(-sqr_mahal_dist(pt)/2));
  }

  //: Compute the gradient of the density function, returned in \a g
  // The return value of the function is the density itself
  T gradient_density(const F& pt, vector& g) const
  {
    T d = Metric::sqr_distance_deriv(pt,mean,covar,g);
    d = std::exp(-d/2);
    g *= -d/2;
    return d;
  }

  //: compute the normalization constant (independent of sample point).
  // Can be precomputed when evaluating at multiple points
  T norm_const() const
  {
    return T(1)/vpdt_gaussian_integrator<F,Covar,Metric>::
                    domain_integral(*this);
  }

  //: The squared Mahalanobis distance to this point
  // Non-virtual for efficiency
  T sqr_mahal_dist(const F& pt) const
  {
    return Metric::sqr_distance(pt,mean,covar);
  }

  //: Evaluate the cumulative distribution function at a point
  // This is the integral of the density function from negative infinity
  // (in all dimensions) to the point in question
  T cumulative_prob(const F& pt) const
  {
    return vpdt_gaussian_integrator<F,Covar,Metric>::
               partial_integral(*this,pt);
  }

  //: Compute the mean of the distribution.
  void compute_mean(vector& m) const { m = mean; }

  //: Compute the covariance matrix of the distribution.
  void compute_covar(matrix& c) const
  {
    // use the metric to compute the covariance at the mean
    Metric::compute_covar(c, mean, covar);
  }

  //=========================================
  // member variables - public for efficiency

  //: the mean
  F mean;
  //: the matrix covariance
  covar_type covar;
};


//: Compute the log of the unnormalized density
template<class F, class C, class M >
inline typename vpdt_dist_traits<vpdt_gaussian<F,C,M> >::scalar_type
vpdt_log_density(const vpdt_gaussian<F,C,M>& d,
                 const typename vpdt_dist_traits<vpdt_gaussian<F,C,M> >::field_type& pt)
{
  typedef typename vpdt_dist_traits<vpdt_gaussian<F,C,M> >::scalar_type T;
  return static_cast<T>(-d.sqr_mahal_dist(pt)/2);
}

//: Compute the gradient of the log of the unnormalized density
template<class F, class C, class M >
inline typename vpdt_dist_traits<vpdt_gaussian<F,C,M> >::scalar_type
vpdt_gradient_log_density(const vpdt_gaussian<F,C,M>& d,
                          const typename vpdt_dist_traits<vpdt_gaussian<F,C,M> >::field_type& pt,
                          typename vpdt_dist_traits<vpdt_gaussian<F,C,M> >::field_type& g)
{
  typedef typename vpdt_dist_traits<vpdt_gaussian<F,C,M> >::scalar_type T;
  T logd = M::sqr_distance_deriv(pt,d.mean,d.covar,g);
  g /= -2;
  return static_cast<T>(-logd/2);
}


//=============================================================================
// Implementations of Gaussian integration structs

//: integrate over a Gaussian distribution
//  This is the variation for multivariate with general covariance
template <class F>
struct vpdt_gaussian_integrator<F, typename vpdt_eigen_sym_matrix_gen<F>::type,
           vpdt_norm_metric<F, typename vpdt_eigen_sym_matrix_gen<F>::type>,
           typename vpdt_field_traits<F>::type_is_vector >
{
  typedef typename vpdt_eigen_sym_matrix_gen<F>::type Covar;
  typedef vpdt_norm_metric<F,typename vpdt_eigen_sym_matrix_gen<F>::type> Metric;
  typedef typename vpdt_field_traits<F>::scalar_type T;

  //: integrate over the entire domain
  static inline T domain_integral(const vpdt_gaussian<F,Covar>& g)
  {
    const unsigned int d = g.dimension();
    const double two_pi = vnl_math::twopi;
    double two_pi_n = two_pi;
    for (unsigned int i=1; i<d; ++i)
      two_pi_n *= two_pi;

    return static_cast<T>(std::sqrt(two_pi_n*Metric::covar_det(g.mean,g.covar)));
  }

  //: integrate from -infinity to \c pt
  static inline T partial_integral(const vpdt_gaussian<F,Covar>& /*g*/, const F& /*pt*/)
  {
    // FIXME: implement this
    // probably requires numerical integration
    return std::numeric_limits<T>::quiet_NaN();
  }
};


//: integrate over a Gaussian distribution
//  This is the variation for multivariate with independent covariance
template <class F>
struct vpdt_gaussian_integrator<F,F,vpdt_norm_metric<F,F>,
           typename vpdt_field_traits<F>::type_is_vector >
{
  typedef typename vpdt_field_traits<F>::scalar_type T;

  //: integrate over the entire domain
  static inline T domain_integral(const vpdt_gaussian<F,F>& g)
  {
    const unsigned int d = g.dimension();
    const double two_pi = vnl_math::twopi;
    double val = 1.0;
    for (unsigned int i=0; i<d; ++i)
      val *= two_pi*g.covar[i];

    return static_cast<T>(std::sqrt(val));
  }

  //: integrate from -infinity to \c pt
  static inline T partial_integral(const vpdt_gaussian<F,F>& g, const F& pt)
  {
    const unsigned int d = g.dimension();
    double val = 1.0;
    for (unsigned int i=0; i<d; ++i)
    {
      if (vpdt_index(g.covar,i) <= T(0))
      {
        if (vpdt_index(pt,i) < vpdt_index(g.mean,i))
          return T(0);
      }
      else{
        double s2 = std::sqrt(2.0*vpdt_index(g.covar,i));
        val *= 0.5*vnl_erf((vpdt_index(pt,i)-vpdt_index(g.mean,i))/s2) + 0.5;
      }
    }
    return static_cast<T>(val);
  }
};


//: integrate over a Gaussian distribution
//  This is the variation for multivariate with hyper-spherical covariance
template <class F>
struct vpdt_gaussian_integrator<F,typename vpdt_field_traits<F>::scalar_type,
          vpdt_norm_metric<F,typename vpdt_field_traits<F>::scalar_type>,
          typename vpdt_field_traits<F>::type_is_vector>
{
  typedef typename vpdt_field_traits<F>::vector_type vector;
  typedef typename vpdt_field_traits<F>::scalar_type T;

  //: integrate over the entire domain
  static inline T domain_integral(const vpdt_gaussian<F,T>& g)
  {
    const unsigned int d = g.dimension();
    const double two_pi = vnl_math::twopi;
    double val = 1.0;
    for (unsigned int i=0; i<d; ++i)
      val *= two_pi*g.covar;

    return static_cast<T>(std::sqrt(val));
  }

  //: integrate from -infinity to \c pt
  static inline T partial_integral(const vpdt_gaussian<F,T>& g, const F& pt)
  {
    const unsigned int d = g.dimension();
    if (g.covar<=T(0))
    {
      for (unsigned int i=0; i<d; ++i)
        if (vpdt_index(pt,i) < vpdt_index(g.mean,i))
          return T(0);
      return T(1);
    }
    double s2 = 1/std::sqrt(2.0*g.covar);
    double val = 1.0;
    for (unsigned int i=0; i<d; ++i)
    {
      val *= 0.5*vnl_erf(s2*(vpdt_index(pt,i)-vpdt_index(g.mean,i))) + 0.5;
    }
    return static_cast<T>(val);
  }
};


//: integrate over a Gaussian distribution
//  This is the variation for univariate
template <class F>
struct vpdt_gaussian_integrator<F,F,vpdt_norm_metric<F,F>,
typename vpdt_field_traits<F>::type_is_scalar>
{
  typedef typename vpdt_field_traits<F>::scalar_type T;

  //: integrate over the entire domain
  static inline T domain_integral(const vpdt_gaussian<F,F>& g)
  {
    return static_cast<T>(std::sqrt(vnl_math::twopi*g.covar));
  }

  //: integrate from -infinity to \c pt
  static inline T partial_integral(const vpdt_gaussian<F,F>& g, const F& pt)
  {
    if (g.covar<=T(0))
    {
      if (pt < g.mean)
        return T(0);
      return T(1);
    }
    double val = 0.5*vnl_erf((pt-g.mean)/std::sqrt(2.0*g.covar)) + 0.5;
    return static_cast<T>(val);
  }
};


#endif // vpdt_gaussian_h_
