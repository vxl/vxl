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
  //: the data type to represent a point in the field
  typedef typename vpdt_field_traits<typename dist::field_type>::field_type field_type;
  //: The compile time dimension of the distribution
  static const unsigned int dimension = vpdt_field_traits<field_type>::dimension;
  //: The type used for scalar operations
  typedef typename vpdt_field_traits<field_type>::scalar_type scalar_type;
  //: the data type used for vectors (difference between points)
  typedef typename vpdt_field_traits<field_type>::vector_type vector_type;
  //: the data type used for square matrices
  typedef typename vpdt_field_traits<field_type>::matrix_type matrix_type;

};


template <class dist>
struct vpdt_is_mixture
{
  static const bool value = false;
};


#endif // vpdt_dist_traits_h_
