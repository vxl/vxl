// This is core/vpdl/vpdl_kernel_base.h
#ifndef vpdl_kernel_base_h_
#define vpdl_kernel_base_h_
//:
// \file
// \author Matthew Leotta
// \date February 24, 2009
// \brief Base classes for kernel (aka Parzen window) distributions
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <vector>
#include <vpdl/vpdl_multi_cmp_dist.h>
#include <vpdl/vpdt/vpdt_access.h> // function vpdt_size(v)
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A base class for kernel (aka Parzen window) distributions
// A kernel distribution is restricted form of a mixture where each component
// has the same weight and takes the same form.  Essentially, a copy of a single
// distribution is translated (and possibly scaled) to each point in a collection
// of samples
template<class T, unsigned int n=0>
class vpdl_kernel_base : public vpdl_multi_cmp_dist<T,n>
{
 public:
  //: the data type used for vectors
  typedef typename vpdt_field_default<T,n>::type vector;
  //: the data type used for matrices
  typedef typename vpdt_field_traits<vector>::matrix_type matrix;

  // Default Constructor
  vpdl_kernel_base()  {}

  // Constructor from sample points
  vpdl_kernel_base(const std::vector<vector>& samplez)
  : samples_(samplez) {}

  //: Return the number of components in the mixture
  unsigned int num_components() const { return samples_.size(); }

  //: Return the run time dimension, which does not equal \c n when \c n==0
  virtual unsigned int dimension() const
  {
    if (n > 0 || num_components() == 0)
      return n;
    return vpdt_size(samples_[0]);
  }

  //: Add a new sample point
  virtual void add_sample(const vector& s)
  {
    // set variable dimension from the first inserted component
    assert(vpdt_size(s) == this->dimension() || num_components() == 0);
    samples_.push_back(s);
  }

  //: Remove all sample points
  virtual void clear_samples()
  {
    samples_.clear();
  }

  //: Set the collection of sample points
  virtual void set_samples(const std::vector<vector>& samplez)
  {
    samples_ = samplez;
  }

  //: Access the sample points
  const std::vector<vector>& samples() const
  {
    return samples_;
  }

  //: Compute the mean of the distribution.
  // Assume that each kernel has its mean at the sample point
  virtual void compute_mean(vector& mean) const
  {
    const unsigned int d = this->dimension();
    vpdt_set_size(mean,d);
    vpdt_fill(mean,T(0));
    if (samples_.empty())
      return;
    typedef typename std::vector<vector>::const_iterator samp_itr;
    for (samp_itr s = samples_.begin(); s != samples_.end(); ++s) {
      mean += *s;
    }
    mean /= T(samples_.size());
  }

 private:
  //: The sample points around which the kernels are centered
  std::vector<vector> samples_;
};


//: A base class for fixed bandwidth kernel distributions
// This class assumes that the bandwidth is fixed for all kernels
template<class T, unsigned int n=0>
class vpdl_kernel_fbw_base : public vpdl_kernel_base<T,n>
{
 public:
  //: the data type used for vectors
  typedef typename vpdt_field_default<T,n>::type vector;
  //: the data type used for matrices
  typedef typename vpdt_field_traits<vector>::matrix_type matrix;

  // Default Constructor
  vpdl_kernel_fbw_base()
  : bandwidth_(T(1)) {}

  // Constructor from sample points and a bandwidth
  vpdl_kernel_fbw_base(const std::vector<vector>& samplez, T bandwid = T(1))
  : vpdl_kernel_base<T,n>(samplez), bandwidth_(bandwid) {}

  //: Access the bandwidth
  T bandwidth() const { return bandwidth_; }

  //: Set the kernel bandwidth
  void set_bandwidth(T b) { bandwidth_ = b; }

  //: The normalization constant for the kernel
  virtual T kernel_norm_const() const = 0;

  //: The normalization constant for the density
  // When density() is multiplied by this value it becomes prob_density
  // norm_const() is reciprocal of the integral of density over the entire field
  virtual T norm_const() const
  {
    return kernel_norm_const()/this->num_components();
  }

 private:
  //: the fixed bandwidth for all kernels
  T bandwidth_;
};


//: A base class for variable bandwidth kernel distributions
// This class assumes that each sample has its own bandwidth
template<class T, unsigned int n=0>
class vpdl_kernel_vbw_base : public vpdl_kernel_base<T,n>
{
 public:
  //: the data type used for vectors
  typedef typename vpdt_field_default<T,n>::type vector;
  //: the data type used for matrices
  typedef typename vpdt_field_traits<vector>::matrix matrix;

  // Default Constructor
  vpdl_kernel_vbw_base(unsigned int var_dim = n)
  : vpdl_kernel_base<T,n>(var_dim) {}

  // Constructor from sample points and bandwidths
  vpdl_kernel_vbw_base(const std::vector<vector>& samplez,
                       const std::vector<T>& bandwidthz)
  : vpdl_kernel_base<T,n>(samplez), bandwidths_(bandwidthz) {}

  //: Add a new sample point
  virtual void add_sample(const vector& s)
  {
    vpdl_kernel_base<T,n>::add_sample(s);
    bandwidths_.push_back(T(1));
  }

  //: Add a new sample point with bandwidth
  virtual void add_sample(const vector& s, T bw)
  {
    vpdl_kernel_base<T,n>::add_sample(s);
    bandwidths_.push_back(bw);
  }

  //: Remove all sample points
  virtual void clear_samples()
  {
    vpdl_kernel_base<T,n>::clear_samples();
    bandwidths_.clear();
  }

  //: Set the collection of sample points
  virtual void set_samples(const std::vector<vector>& samplez)
  {
    vpdl_kernel_base<T,n>::set_samples(samplez);
    bandwidths_.clear();
    bandwidths_.resize(samplez.size(),T(1));
  }

  //: Set the collection of sample points and bandwidths
  virtual void set_samples(const std::vector<vector>& samplez,
                           const std::vector<T>& bandwidthz)
  {
    assert(samplez.size() == bandwidthz.size());
    vpdl_kernel_base<T,n>::set_samples(samplez);
    bandwidths_ = bandwidthz;
  }

  //: Access the bandwidths
  const std::vector<T>& bandwidths() const { return bandwidths_; }

 private:
  //: the bandwidths for each kernel
  std::vector<T> bandwidths_;
};


#endif // vpdl_kernel_base_h_
