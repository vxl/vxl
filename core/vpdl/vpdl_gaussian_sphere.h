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
#include <vpdl/vpdl_npower.h>
#include <vnl/vnl_erf.h>
#include <vcl_limits.h>
#include <vcl_cassert.h>

//: A Gaussian with (hyper-)spherical covariance
template<class T, unsigned int n=0>
class vpdl_gaussian_sphere : public vpdl_gaussian_base<T,n>
{
 public:
  //: the data type used for vectors (e.g. the mean)
  typedef typename vpdl_base_traits<T,n>::vector vector;
  //: the data type used for matrices (e.g. covariance)
  typedef typename vpdl_base_traits<T,n>::matrix matrix;
  //: the type used internally for covariance
  typedef T covar_type;

  //: Constructor
  // Optionally initialize the dimension for when n==0.
  // Otherwise var_dim is ignored
  vpdl_gaussian_sphere(unsigned int var_dim = n)
  : vpdl_gaussian_base<T,n>(var_dim), var_(T(0)) {}

  //: Constructor - from mean and variance
  vpdl_gaussian_sphere(const vector& mean, const covar_type& var)
  : vpdl_gaussian_base<T,n>(mean), var_(var) {}

  //: Destructor
  virtual ~vpdl_gaussian_sphere() {}

  //: Create a copy on the heap and return base class pointer
  virtual vpdl_distribution<T,n>* clone() const
  {
    return new vpdl_gaussian_sphere<T,n>(*this);
  }

  //: Evaluate the probability density at a point
  virtual T prob_density(const vector& pt) const
  {
    if (var_<=T(0))
      return 0.0;
    return static_cast<T>(norm_const() * vcl_exp(-sqr_mahal_dist(pt)/2));
  }

  //: Evaluate the log probability density at a point
  virtual T log_prob_density(const vector& pt) const
  {
    if (var_<=T(0))
      return -vcl_numeric_limits<T>::infinity();
    return static_cast<T>(vcl_log(norm_const()) - sqr_mahal_dist(pt)/2);
  };

  //: compute the normalization constant (independent of sample point).
  // Can be precomputed when evaluating at multiple points
  // non-virtual for efficiency
  T norm_const() const
  {
    const unsigned int dim = this->dimension();
    double v2pi = var_*2.0*vnl_math::pi;
    double denom = v2pi;
    for(unsigned int i=1; i<dim; ++i)
      denom *= v2pi;
      
    return static_cast<T>(vcl_sqrt(1/denom));
  }

  //: The squared Mahalanobis distance to this point
  // non-virtual for efficiency
  T sqr_mahal_dist(const vector& pt) const
  {
    if (var_<=T(0))
      return vcl_numeric_limits<T>::infinity();

    const unsigned int d = this->dimension();
    T val = T(0);
    for (unsigned int i=0; i<d; ++i)
    {
      T tmp = (index(pt,i)-index(this->mean_,i));
      val += tmp*tmp/var_;
    }
    return val;
  }

  //: Evaluate the cumulative distribution function at a point
  // This is the integral of the density function from negative infinity
  // (in all dimensions) to the point in question
  virtual T cumulative_prob(const vector& pt) const
  {
    const unsigned int d = this->dimension();
    if (var_<=T(0))
    {
      for (unsigned int i=0; i<d; ++i)
        if (index(pt,i) < index(this->mean_,i))
          return T(0);
      return T(1);
    }
    double sigma_sq_2 = 2.0*static_cast<double>(var_);
    double s2 = 1/vcl_sqrt(sigma_sq_2);

    double val = 1.0;
    for (unsigned int i=0; i<d; ++i)
    {
      val *= 0.5*vnl_erf(s2*(index(pt,i)-index(this->mean_,i))) + 0.5;
    }
    return static_cast<T>(val);
  }

  //: The probability of being in an axis-aligned box
  // The box is defined by two points, the minimum and maximum.
  // Reimplemented for effeciency since the axis are independent
  T box_prob(const vector& min_pt, const vector& max_pt) const
  {
    const unsigned int dim = this->dimension();

    double s2 = 1/vcl_sqrt(2*var_);
    // return zero for ill-defined box
    double prob = T(1);
    for (unsigned int i=0; i<dim; ++i){
      if (index(max_pt,i)<=index(min_pt,i))
        return T(0);
      prob *= (vnl_erf(s2*(index(max_pt,i)-index(this->mean_,i))) -
               vnl_erf(s2*(index(min_pt,i)-index(this->mean_,i))))/2;
    }
    return static_cast<T>(prob);
  }

  //: Access the scalar variance
  const covar_type& covariance() const { return var_; }

  //: Set the scalar variance
  void set_covariance(const covar_type& var) { var_ = var; }


  //: Compute the covariance of the distribution.
  // Should be the identity matrix times var_
  virtual void compute_covar(matrix& covar) const
  {
    const unsigned int d = this->dimension();
    set_size(covar,d);
    assert(m_size(covar) == d);
    for (unsigned int i=0; i<d; ++i)
    {
      index(covar,i,i) = var_;
      for (unsigned int j=i+1; j<d; ++j)
        index(covar,i,j) = index(covar,j,i) = T(0);
    }
  }

 protected:
  //: the scalar variance
  covar_type var_;
};


#endif // vpdl_gaussian_sphere_h_
