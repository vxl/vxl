// This is gel/mrc/vpgl/vpgl_reg_fundamental_matrix.txx
#ifndef vpgl_reg_fundamental_matrix_txx_
#define vpgl_reg_fundamental_matrix_txx_
//:
// \file

#include <vnl/vnl_fwd.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_cross_product_matrix.h>
#include <vcl_iosfwd.h>
#include <vcl_cassert.h>

#include "vpgl_reg_fundamental_matrix.h"


//---------------------------------
template <class T>
vpgl_reg_fundamental_matrix<T>::vpgl_reg_fundamental_matrix() :
  vpgl_fundamental_matrix<T>()
{
  vnl_matrix_fixed<T,3,3> default_matrix( (T)0 );
  default_matrix(0,1) = default_matrix(0,2) = (T)1;
  default_matrix(1,0) = default_matrix(2,0) = -(T)1;
  set_matrix( default_matrix );
}


//---------------------------------
template <class T>
vpgl_reg_fundamental_matrix<T>::vpgl_reg_fundamental_matrix(
  const vgl_point_2d<T>& pr,
  const vgl_point_2d<T>& pl ) :
  vpgl_fundamental_matrix<T>()
{
  set_from_points( pr, pl );
}

//---------------------------------
template <class T>
vpgl_reg_fundamental_matrix<T>::vpgl_reg_fundamental_matrix(
  const vpgl_fundamental_matrix<T>& fm )
{
  set_from_params( fm.get_matrix()(0,2), fm.get_matrix()(2,1) );
}


//---------------------------------
template <class T>
bool vpgl_reg_fundamental_matrix<T>::set_from_points(
  const vgl_point_2d<T>& pr,
  const vgl_point_2d<T>& pl )
{
  T ex = pl.x() - pr.x();
  T ey = pl.y() - pr.y();
  if ( ex == 0 && ey == 0 ) return false;
  vnl_matrix_fixed<T,3,3> fm( (T)0 );
  fm.put( 0, 2, ey );
  fm.put( 1, 2, -ex );
  fm.put( 2, 0, -ey );
  fm.put( 2, 1, ex );
  set_matrix( fm );
  return true;
}


//---------------------------------
template <class T>
void vpgl_reg_fundamental_matrix<T>::set_from_params( T a, T b )
{
  vnl_matrix_fixed<T,3,3> fm( (T)0 );
  fm.put( 0, 2, a );
  fm.put( 1, 2, -b );
  fm.put( 2, 0, -a );
  fm.put( 2, 1, b );
  set_matrix( fm );
};


// Code for easy instantiation.
#undef vpgl_REG_FUNDAMENTAL_MATRIX_INSTANTIATE
#define vpgl_REG_FUNDAMENTAL_MATRIX_INSTANTIATE(T) \
template class vpgl_reg_fundamental_matrix<T >

#endif // vpgl_reg_fundamental_matrix_txx_
