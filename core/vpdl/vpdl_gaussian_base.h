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


//: The abstract base class for Gaussian distributions
// All Gaussian classes represent the mean in the same way,
// so it is managed in this base class.
// Derived classes differ in how they represent covariance
template<class T, unsigned int n=0>
class vpdl_gaussian_base : public vpdl_distribution<T,n>
{
 public:
  //: the data type used for vectors
  typedef typename vpdt_field_default<T,n>::type vector;


  //: Destructor
  virtual ~vpdl_gaussian_base() {}

  //: Access the mean directly
  virtual const vector& mean() const = 0;

  //: Set the mean
  virtual void set_mean(const vector& mean) = 0;
};


#endif // vpdl_gaussian_base_h_
