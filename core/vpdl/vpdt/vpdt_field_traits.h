// This is core/vpdl/vpdt/vpdt_field_traits.h
#ifndef vpdt_field_traits_h_
#define vpdt_field_traits_h_
//:
// \file
// \author Matthew Leotta
// \brief specialized template trait classes for properties of a field type
// \date March 5, 2009
//
// Each type of object upon which you define a field for a probability
// distribution requires a field traits class.  The field traits determine
// the dimension, scalar type, field type, vector type, matrix type, etc to be used.
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: The field traits class (scalar)
//  The default is to treat it as a 1-d (scalar) field
template <class T>
struct vpdt_field_traits
{
  //: The compile time dimension of the field
  static const unsigned int dimension = 1;
  //: The type used for scalar operations
  typedef T scalar_type;
  //: the data type to represent a point in the field
  typedef T field_type;
  //: the data type used for vectors (difference between points)
  typedef T vector_type;
  //: the data type used for square matrices
  typedef T matrix_type;

  //: use this to disambiguate templates
  typedef void type_is_scalar;
};


//: The field traits class (vnl_vector_fixed)
//  This specialization is for fixed length vnl vectors
template <class T, unsigned int n>
struct vpdt_field_traits<vnl_vector_fixed<T,n> >
{
  //: The compile time dimension of the field
  static const unsigned int dimension = n;
  //: The type used for scalar operations
  typedef T scalar_type;
  //: the data type to represent a point in the field
  typedef vnl_vector_fixed<T,n> field_type;
  //: the data type used for vectors (difference between points)
  typedef vnl_vector_fixed<T,n> vector_type;
  //: the data type used for square matrices
  typedef vnl_matrix_fixed<T,n,n> matrix_type;

  //: use this to disambiguate templates
  typedef void type_is_vector;
};


//: The field traits class (vnl_vector)
//  This specialization is for variable length vnl vectors
//  \note dimension of 0 indicates variable dimension at run time
template <class T>
struct vpdt_field_traits<vnl_vector<T> >
{
  //: The compile time dimension of the field
  static const unsigned int dimension = 0;
  //: The type used for scalar operations
  typedef T scalar_type;
  //: the data type to represent a point in the field
  typedef vnl_vector<T> field_type;
  //: the data type used for vectors (difference between points)
  typedef vnl_vector<T> vector_type;
  //: the data type used for square matrices
  typedef vnl_matrix<T> matrix_type;

  //: use this to disambiguate templates
  typedef void type_is_vector;
};



#endif // vpdt_field_traits_h_
