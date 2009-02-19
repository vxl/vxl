// This is core/vpdl/vpdl_multi_cmp_dist.h
#ifndef vpdl_multi_cmp_dist_h_
#define vpdl_multi_cmp_dist_h_
//:
// \file
// \author Matthew Leotta
// \date February 18, 2009
// \brief base class for multiple component distributions
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <vpdl/vpdl_distribution.h>

//: The base class for all multiple component probability distributions.
template<class T, unsigned int n=0>
class vpdl_multi_cmp_dist : public vpdl_distribution<T,n>
{
 public:
  //: the data type used for vectors (e.g. the mean)
  typedef typename vpdl_base_traits<T,n>::vector vector;
  //: the data type used for matrices (e.g. covariance)
  typedef typename vpdl_base_traits<T,n>::matrix matrix;

  //: Constructor
  // Optionally initialize the dimension for when n==0.
  // Otherwise var_dim is ignored
  vpdl_multi_cmp_dist(unsigned int var_dim = n) 
  : vpdl_distribution<T,n>(var_dim) {}
  
  //: Destructor
  virtual ~vpdl_multi_cmp_dist() {}

  //: The number of components
  virtual unsigned int num_components() const = 0;

};




#endif // vpdl_multi_cmp_dist_h_
