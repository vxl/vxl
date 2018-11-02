// This is core/vpdl/vpdl_kernel_gaussian_sfbw.h
#ifndef vpdl_kernel_gaussian_sfbw_h_
#define vpdl_kernel_gaussian_sfbw_h_
//:
// \file
// \author Matthew Leotta
// \date February 24, 2009
// \brief A fixed bandwidth spherical Gaussian kernel distribution
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <limits>
#include <vpdl/vpdl_kernel_base.h>
#include <vpdl/vpdt/vpdt_access.h>
#include <vnl/vnl_erf.h>
#include <vnl/vnl_math.h> // for twopi
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: A fixed bandwidth spherical Gaussian kernel distribution
// The bandwidth is the standard deviation of the Gaussian kernel.
template<class T, unsigned int n=0>
class vpdl_kernel_gaussian_sfbw : public vpdl_kernel_fbw_base<T,n>
{
 public:
  //: the data type used for vectors
  typedef typename vpdt_field_default<T,n>::type vector;
  //: the data type used for matrices
  typedef typename vpdt_field_traits<vector>::matrix_type matrix;

  //: Default Constructor
  vpdl_kernel_gaussian_sfbw() {}

  //: Constructor - from sample centers and bandwidth (variance)
  vpdl_kernel_gaussian_sfbw(const std::vector<vector>& samplez,
                            T bandwid = T(1))
  : vpdl_kernel_fbw_base<T,n>(samplez,bandwid) {}

  //: Create a copy on the heap and return base class pointer
  virtual vpdl_distribution<T,n>* clone() const
  {
    return new vpdl_kernel_gaussian_sfbw<T,n>(*this);
  }

  //: Evaluate the unnormalized density at a point
  virtual T density(const vector& pt) const
  {
    const unsigned int nc = this->num_components();
    if (nc <= 0)
      return 0.0;

    const unsigned int d = this->dimension();
    T sum = T(0);
    typedef typename std::vector<vector>::const_iterator vitr;
    for (vitr s=this->samples().begin(); s!=this->samples().end(); ++s) {
      T ssd = T(0);
      for (unsigned int i=0; i<d; ++i) {
        T tmp = (vpdt_index(pt,i)-vpdt_index(*s,i))/this->bandwidth();
        ssd += tmp*tmp;
      }
      sum += T(std::exp(-0.5*ssd));
    }

    return sum;
  }

  //: Evaluate the probability density at a point
  virtual T prob_density(const vector& pt) const
  {
    const unsigned int nc = this->num_components();
    if (nc <= 0)
      return 0.0;

    return density(pt)*this->norm_const();
  }

  //: Compute the gradient of the unnormalized density at a point
  // \return the density at \a pt since it is usually needed as well, and
  //         is often trivial to compute while computing gradient
  // \retval g the gradient vector
  virtual T gradient_density(const vector& pt, vector& g) const
  {
    const unsigned int d = this->dimension();
    vpdt_set_size(g,d);
    vpdt_fill(g,T(0));
    const unsigned int nc = this->num_components();
    if (nc <= 0)
      return 0.0;

    T sum = T(0);
    vector g_s;
    vpdt_set_size(g_s,d);
    typedef typename std::vector<vector>::const_iterator vitr;
    for (vitr s=this->samples().begin(); s!=this->samples().end(); ++s) {
      T ssd = T(0);
      for (unsigned int i=0; i<d; ++i) {
        T tmp = (vpdt_index(pt,i)-vpdt_index(*s,i))/this->bandwidth();
        vpdt_index(g_s,i) = tmp/this->bandwidth();
        ssd += tmp*tmp;
      }
      T dens = T(std::exp(-0.5*ssd));
      g_s *= -dens;
      sum += dens;
      g += g_s;
    }

    return sum;
  }

  //: Evaluate the cumulative distribution function at a point
  // This is the integral of the density function from negative infinity
  // (in all dimensions) to the point in question
  virtual T cumulative_prob(const vector& pt) const
  {
    const unsigned int nc = this->num_components();
    if (nc <= 0)
      return 0.0;

    const unsigned int d = this->dimension();
    double s2 = 1/(this->bandwidth()*std::sqrt(2.0));

    double sum = 0.0;
    typedef typename std::vector<vector>::const_iterator vitr;
    for (vitr s=this->samples().begin(); s!=this->samples().end(); ++s) {
      double val = 1.0;
      for (unsigned int i=0; i<d; ++i) {
        val *= 0.5*vnl_erf(s2*(vpdt_index(pt,i)-vpdt_index(*s,i))) + 0.5;
      }
      sum += val;
    }
    return static_cast<T>(sum/nc);
  }

  //: The probability of being in an axis-aligned box
  // The box is defined by two points, the minimum and maximum.
  // Reimplemented for efficiency since the axis are independent
  T box_prob(const vector& min_pt, const vector& max_pt) const
  {
    const unsigned int nc = this->num_components();
    if (nc <= 0)
      return 0.0;

    const unsigned int dim = this->dimension();
    double s2 = 1/(this->bandwidth()*std::sqrt(2.0));

    double sum = 0.0;
    typedef typename std::vector<vector>::const_iterator vitr;
    for (vitr s=this->samples().begin(); s!=this->samples().end(); ++s) {
      double prob = 1.0;
      for (unsigned int i=0; i<dim; ++i) {
        if (vpdt_index(max_pt,i)<=vpdt_index(min_pt,i))
          return T(0);
        prob *= (vnl_erf(s2*(vpdt_index(max_pt,i)-vpdt_index(*s,i))) -
                 vnl_erf(s2*(vpdt_index(min_pt,i)-vpdt_index(*s,i))))/2;
      }
      sum += prob;
    }
    return static_cast<T>(sum/nc);
  }

  //: Compute the covariance of the distribution.
  virtual void compute_covar(matrix& covar) const
  {
    const unsigned int d = this->dimension();
    const unsigned int nc = this->num_components();
    vector mean;
    vpdt_set_size(covar,d);
    vpdt_fill(covar,T(0));
    vpdt_set_size(mean,d);
    vpdt_fill(mean,T(0));
    typedef typename std::vector<vector>::const_iterator samp_itr;
    for (samp_itr s = this->samples().begin(); s != this->samples().end(); ++s) {
      covar += outer_product(*s,*s);
      mean += *s;
    }
    mean /= T(nc);
    covar /= T(nc);
    covar -= outer_product(mean,mean);
    T var = this->bandwidth()*this->bandwidth();
    for (unsigned int i=0; i<d; ++i)
      vpdt_index(covar,i,i) += var;
  }

  //: The normalization constant for the kernel
  virtual T kernel_norm_const() const
  {
    const unsigned int dim = this->dimension();
    double v2pi = this->bandwidth()*this->bandwidth()*vnl_math::twopi;
    double denom = v2pi;
    for (unsigned int i=1; i<dim; ++i)
      denom *= v2pi;

    return static_cast<T>(std::sqrt(1/denom));
  }
};


#endif // vpdl_kernel_gaussian_sfbw_h_
