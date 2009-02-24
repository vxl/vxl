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
// Modifications
//   None
// \endverbatim

#include <vpdl/vpdl_multi_cmp_dist.h>
#include <vcl_cassert.h>
#include <vcl_vector.h>

//: A base class for kernel (aka Parzen window) distributions
// A kernel distribution is restricted form of a mixture where each component
// has the same weight and takes the same form.  Essentially, a copy of a single
// distribution is translated (and possibly scaled) to each point in a collection
// of samples
template<class T, unsigned int n=0>
class vpdl_kernel_base : public vpdl_multi_cmp_dist<T,n>
{
public:
  //: the data type used for vectors (e.g. the mean)
  typedef typename vpdl_base_traits<T,n>::vector vector;
  //: the data type used for matrices (e.g. covariance)
  typedef typename vpdl_base_traits<T,n>::matrix matrix;


  // Default Constructor
  vpdl_kernel_base(unsigned int var_dim = n) 
    : vpdl_multi_cmp_dist<T,n>(var_dim) {}
  
  // Constructor from sample points
  vpdl_kernel_base(const vcl_vector<vector>& samples) 
  : vpdl_multi_cmp_dist<T,n>(samples.empty()?n:v_size(samples.front())), 
    samples_(samples) {}


  //: Return the number of components in the mixture
  unsigned int num_components() const { return samples_.size(); }


  //: Add a new sample point
  virtual void add_sample(const vector& s)
  { 
    // set variable dimension from the first inserted component
    if(this->dimension() == 0 && samples_.empty())
      this->set_dimension(v_size(s));
    assert(v_size(s) == this->dimension());
    samples_.push_back(s);
  }

  //: Remove all sample points
  virtual void clear_samples() 
  { 
    samples_.clear();
  }

  //: Set the collection of sample points
  virtual void set_samples(const vcl_vector<vector>& samples)
  {
    // adjust the dimension for these new samples
    if(!samples.empty())
      this->set_dimension(v_size(samples.front()));
    samples_ = samples;
  }
  
  //: Access the sample points
  const vcl_vector<vector>& samples() const
  {
    return samples_;
  }
  
  //: Compute the mean of the distribution.
  // Assume that each kernel has its mean at the sample point
  virtual void compute_mean(vector& mean) const
  {
    const unsigned int d = this->dimension();
    v_init(mean,d,T(0));
    if(samples_.empty())
      return;
    typedef typename vcl_vector<vector>::const_iterator samp_itr;
    for (samp_itr s = samples_.begin(); s != samples_.end(); ++s){
      mean += *s;
    }
    mean /= samples_.size();
  }

private:
  //: The sample points around which the kernels are centered
  vcl_vector<vector> samples_;

};


//: A base class for fixed bandwidth kernel distributions
// This class assumes that the bandwidth is fixed for all kernels
template<class T, unsigned int n=0>
class vpdl_kernel_fbw_base : public vpdl_kernel_base<T,n>
{
public:
  //: the data type used for vectors (e.g. the mean)
  typedef typename vpdl_base_traits<T,n>::vector vector;
  //: the data type used for matrices (e.g. covariance)
  typedef typename vpdl_base_traits<T,n>::matrix matrix;
  
  // Default Constructor
  vpdl_kernel_fbw_base(unsigned int var_dim = n) 
  : vpdl_kernel_base<T,n>(var_dim), bandwidth_(T(1)) {}
  
  // Constructor from sample points and a bandwidth
  vpdl_kernel_fbw_base(const vcl_vector<vector>& samples, T bandwidth = T(1)) 
  : vpdl_kernel_base<T,n>(samples), bandwidth_(bandwidth){}
  
  //: Access the bandwidth
  T bandwidth() const { return bandwidth_; }
  
  //: Set the kernel bandwidth
  void set_bandwidth(T b) { bandwidth_ = b; }
  
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
  //: the data type used for vectors (e.g. the mean)
  typedef typename vpdl_base_traits<T,n>::vector vector;
  //: the data type used for matrices (e.g. covariance)
  typedef typename vpdl_base_traits<T,n>::matrix matrix;
  
  // Default Constructor
  vpdl_kernel_vbw_base(unsigned int var_dim = n) 
  : vpdl_kernel_base<T,n>(var_dim) {}
  
  // Constructor from sample points and bandwidths
  vpdl_kernel_vbw_base(const vcl_vector<vector>& samples, 
                       const vcl_vector<T>& bandwidths) 
  : vpdl_kernel_base<T,n>(samples), bandwidths_(bandwidths){}
  
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
  virtual void set_samples(const vcl_vector<vector>& samples)
  {
    vpdl_kernel_base<T,n>::set_samples(samples);
    bandwidths_.clear();
    bandwidths.resize(samples.size(),T(1));
  }
  
  //: Set the collection of sample points and bandwidths
  virtual void set_samples(const vcl_vector<vector>& samples,
                           const vcl_vector<T>& bandwidths)
  {
    assert(samples.size() == bandwidths.size());
    vpdl_kernel_base<T,n>::set_samples(samples);
    bandwidths_ = bandwidths;
  }
  
  //: Access the bandwidths
  const vcl_vector<T>& bandwidths() const { return bandwidths_; }
  
private:
  //: the bandwidths for each kernel
  vcl_vector<T> bandwidths_;
  
};


#endif // vpdl_kernel_base_h_
