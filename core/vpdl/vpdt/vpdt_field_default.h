// This is core/vpdl/vpdt/vpdt_field_default.h
#ifndef vpdt_field_default_h_
#define vpdt_field_default_h_
//:
// \file
// \author Matthew Leotta
// \brief A type generator for the default types (those used in vpdl)
// \date March 5, 2009
//
// The default field types are scalar, fixed vector, and variable vector
// This class acts as an inverse to vpdt_field_traits.  From the scalar
// type (T) and dimension (n), it produces the default field type according to:
// - For n > 1 the field type is vnl_vector_fixed<T,n>
// - For n == 1 the field type is T
// - For n == 0 the field type is vnl_vector<T>
//
// The n == 0 indicates that the dimension is dynamic and set at run time.
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>


//: Generate the default field type for scalar type T and dimension n
template <class T, unsigned int n=0>
struct vpdt_field_default
{
  //: The default field type
  typedef vnl_vector_fixed<T,n> type;
};


//: Generate the default field type for scalar type T and dimension n
template <class T>
struct vpdt_field_default<T,1>
{
  //: The default field type
  typedef T type;
};


//: Generate the default field type for scalar type T and dimension n
template <class T>
struct vpdt_field_default<T,0>
{
  //: The default field type
  typedef vnl_vector<T> type;
};


#endif // vpdt_field_default_h_
