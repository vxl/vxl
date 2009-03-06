// This is core/vpdl/vpdt/vpdt_dist_traits.h
#ifndef vpdt_dist_traits_h_
#define vpdt_dist_traits_h_
//:
// \file
// \author Matthew Leotta
// \brief specialized template trait classes for properties of a distribution type
// \date March 5, 2009
//
// Provides a short cut to vpdt_field_traits<typename dist::vector>::*
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <vpdl/vpdt/vpdt_field_traits.h>


//: The distribution traits class
//  traits are taken from vpdt_field_traits 
template <class dist>
struct vpdt_dist_traits 
{
  //: The compile time dimension of the distribution
  static const unsigned int dimension = vpdt_field_traits<typename dist::vector>::dimension;
  //: The type used for scalar operations 
  typedef typename vpdt_field_traits<typename dist::vector>::scalar_type scalar_type;
  //: the data type used for vectors (e.g. the mean)
  typedef typename vpdt_field_traits<typename dist::vector>::vector_type vector_type;
  //: the data type used for matrices (e.g. covariance)
  typedef typename vpdt_field_traits<typename dist::vector>::matrix_type matrix_type;
  
};


#endif // vpdt_dist_traits_h_
