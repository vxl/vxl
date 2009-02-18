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
//   None
// \endverbatim

#include <vpdl/vpdl_gaussian_base.h>
#include <vpdl/vpdl_npower.h>
#include <vnl/vnl_erf.h>
#include <vcl_limits.h>
#include <vcl_cassert.h>

//: A Gaussian with variance independent in each dimension
template<class T, unsigned int n=0>
class vpdl_gaussian_indep : public vpdl_gaussian_base<T,n>
{
 public:
  //: the data type used for vectors (e.g. the mean)
  typedef typename vpdl_base_traits<T,n>::vector vector;
  //: the data type used for matrices (e.g. covariance)
  typedef typename vpdl_base_traits<T,n>::matrix matrix;
  //: the type used internally for covariance
  typedef vector covar_type;

  //: Constructor
  // Optionally initialize the dimension for when n==0.
  // Otherwise var_dim is ignored
  vpdl_gaussian_indep(unsigned int var_dim = n)
  : vpdl_gaussian_base<T,n>(var_dim)
  {
    // initialize variance to all zeros
    v_init(var_,var_dim,T(0));
  }

  //: Constructor - from mean and variance
  vpdl_gaussian_indep(const vector& mean, const covar_type& var)
  : vpdl_gaussian_base<T,n>(mean), var_(var) {}

  //: Destructor
  virtual ~vpdl_gaussian_indep() {}

  //: Create a copy on the heap and return base class pointer
  virtual vpdl_distribution<T,n>* clone() const
  {
    return new vpdl_gaussian_indep<T,n>(*this);
  }

  //: Evaluate the probability density at a point
  virtual T prob_density(const vector& pt) const
  {
    T norm = norm_const();
    if (vnl_math_isinf(norm))
      return T(0);

    return static_cast<T>(norm * vcl_exp(-sqr_mahal_dist(pt)/2));
  }

  //: Evaluate the log probability density at a point
  virtual T log_prob_density(const vector& pt) const
  {
    T norm = norm_const();
    if (vnl_math_isinf(norm))
      return -vcl_numeric_limits<T>::infinity();

    return static_cast<T>(vcl_log(norm) - sqr_mahal_dist(pt)/2);
  };

  //: compute the normalization constant (independent of sample point).
  // Can be precomputed when evaluating at multiple points
  // non-virtual for efficiency
  T norm_const() const
  {
    const unsigned int d = this->dimension();
    double det = 1.0;
    for (unsigned int i=0; i<d; ++i)
      det *= index(var_,i);

    if (det<=T(0))
      return vcl_numeric_limits<T>::infinity();

    return static_cast<T>(vcl_sqrt(1/(vpdl_two_pi_power(this->dimension())*
                                      det)));
  }

  //: The squared Mahalanobis distance to this point
  // non-virtual for efficiency
  T sqr_mahal_dist(const vector& pt) const
  {
    const unsigned int d = this->dimension();
    T val = T(0);
    for (unsigned int i=0; i<d; ++i)
    {
      T tmp = (index(pt,i)-index(this->mean_,i));
      val += tmp*tmp/index(var_,i);
    }
    return val;
  }

  //: Evaluate the cumulative distribution function at a point
  // This is the integral of the density function from negative infinity
  // (in all dimensions) to the point in question
  virtual T cumulative_prob(const vector& pt) const
  {
    const unsigned int d = this->dimension();
    double val = 1.0;
    for (unsigned int i=0; i<d; ++i)
    {
      double sigma_sq_2 = 2.0*static_cast<double>(index(var_,i));
      val *= 0.5*vnl_erf((index(pt,i)-index(this->mean_,i))
                         / vcl_sqrt(sigma_sq_2)) + 0.5;
    }
    return static_cast<T>(val);
  }

  //: Access the vector of variance
  const covar_type& covariance() const { return var_; }

  //: Set the vector of variance
  void set_covariance(const covar_type& var) { var_ = var; }


  //: Compute the covariance of the distribution.
  // Should be the diagonal matrix of var_
  virtual void compute_covar(matrix& covar) const
  {
    const unsigned int d = this->dimension();
    set_size(covar,d);
    assert(m_size(covar) == d);
    for (unsigned int i=0; i<d; ++i)
    {
      index(covar,i,i) = index(var_,i);
      for (unsigned int j=i+1; j<d; ++j)
        index(covar,i,j) = index(covar,j,i) = T(0);
    }
  }

 protected:
  //: the vector variance (diagonal of the matrix)
  covar_type var_;
};


#endif // vpdl_gaussian_indep_h_
