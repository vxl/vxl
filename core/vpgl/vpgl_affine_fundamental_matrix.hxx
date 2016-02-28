// This is core/vpgl/vpgl_affine_fundamental_matrix.hxx
#ifndef vpgl_affine_fundamental_matrix_hxx_
#define vpgl_affine_fundamental_matrix_hxx_
//:
// \file

#include "vpgl_affine_fundamental_matrix.h"
//
#include <vnl/vnl_matrix_fixed.h>

//---------------------------------
template <class T>
vpgl_affine_fundamental_matrix<T>::vpgl_affine_fundamental_matrix() :
  vpgl_fundamental_matrix<T>()
{
  vnl_matrix_fixed<T,3,3> default_matrix( (T)0 );
  default_matrix(0,1) = default_matrix(0,2) = (T)1;
  default_matrix(1,0) = default_matrix(2,0) = -(T)1;
  vpgl_fundamental_matrix<T>::set_matrix( default_matrix );
}


//---------------------------------
template <class T>
vpgl_affine_fundamental_matrix<T>::vpgl_affine_fundamental_matrix(
  const vpgl_fundamental_matrix<T>& fm )
{
  set_from_params( fm.get_matrix()(2,0), fm.get_matrix()(2,1), fm.get_matrix()(2,2),
                   fm.get_matrix()(1,2), fm.get_matrix()(0,2) );
}


//---------------------------------
template <class T>
void vpgl_affine_fundamental_matrix<T>::set_from_params( T a, T b, T c, T d, T e )
{
  vnl_matrix_fixed<T,3,3> fm( (T)0 );
  fm.put( 2, 0, a );
  fm.put( 2, 1, b );
  fm.put( 2, 2, c );
  fm.put( 1, 2, d );
  fm.put( 0, 2, e );
  vpgl_fundamental_matrix<T>::set_matrix( fm );
};


// Code for easy instantiation.
#undef vpgl_AFFINE_FUNDAMENTAL_MATRIX_INSTANTIATE
#define vpgl_AFFINE_FUNDAMENTAL_MATRIX_INSTANTIATE(T) \
template class vpgl_affine_fundamental_matrix<T >

#endif // vpgl_affine_fundamental_matrix_hxx_
