// This is core/vpdl/vpdl_gaussian_base.h
#ifndef vpdl_gaussian_base_h_
#define vpdl_gaussian_base_h_
//:
// \file
// \author Matthew Leotta
// \date February 11, 2009
// \brief The abstract base class for Gaussian distributions
//
// \verbatim
// Modifications
//   None
// \endverbatim

#include <vpdl/vpdl_distribution.h>


//: The abstrace base class for Gaussian distributions
// All Gaussian classes represent the mean in the same way,
// so it is managed in this base class.
// Derived classes differ in how they represent covariance
template<class T, unsigned int n=0>
class vpdl_gaussian_base : public vpdl_distribution<T,n>
{
public:
  //: the data type used for vectors (e.g. the mean) 
  typedef typename vpdl_base_traits<T,n>::vector vector;
  //: the data type used for matrices (e.g. covariance)
  typedef typename vpdl_base_traits<T,n>::matrix matrix;
  
  //: Constructor
  // Optionally initialize the dimension for when n==0.
  // Otherwise var_dim is ignored
  vpdl_gaussian_base(unsigned int var_dim = n) 
  : vpdl_distribution<T,n>(var_dim)
  {
    // initialize mean to all zeros
    v_init(mean_,var_dim,T(0));
  }
  
  //: Constructor - from a mean point
  vpdl_gaussian_base(const vector& mean) 
  : vpdl_distribution<T,n>(v_size(mean)), 
    mean_(mean) {}
  
  //: Destructor
  virtual ~vpdl_gaussian_base() {}
  
  //: Access the mean directly
  const vector& mean() const { return mean_; }
  
  //: Set the mean
  void set_mean(const vector& mean) 
  { 
    mean_ = mean; 
    set_dimension(v_size(mean)); 
  }
  
  //: Compute the mean of the distribution.
  virtual void compute_mean(vector& mean) const { mean = mean_; }
  
protected:
  //: the mean of the distribution
  vector mean_;
}; 




#endif // vpdl_gaussian_base_h_
