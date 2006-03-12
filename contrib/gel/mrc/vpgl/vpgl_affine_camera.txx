// This is gel/mrc/vpgl/vpgl_affine_camera.txx
#ifndef vpgl_affine_camera_txx_
#define vpgl_affine_camera_txx_
//:
// \file

#include "vpgl_affine_camera.h"
#include <vnl/vnl_vector_fixed.h>
#include <vcl_cassert.h>

//-------------------------------------------
template <class T>
vpgl_affine_camera<T>::vpgl_affine_camera()
{
  vnl_matrix_fixed<T,3,4> C( (T)0 );
  C(0,0) = C(1,1) = C(2,3) = (T)1;
  set_matrix( C );
}


//-------------------------------------------
template <class T>
vpgl_affine_camera<T>::vpgl_affine_camera( const vnl_vector_fixed<T,4>& row1,
                                           const vnl_vector_fixed<T,4>& row2 )
{
  set_rows( row1, row2 );
}


//------------------------------------------
template <class T>
vpgl_affine_camera<T>::vpgl_affine_camera( const vnl_matrix_fixed<T,3,4>& camera_matrix )
{
  assert( camera_matrix(2,3) != 0 );
  vnl_matrix_fixed<T,3,4> C( camera_matrix );
  C = C/C(2,3);
  C(2,0) = (T)0; C(2,1) = (T)0; C(2,2) = (T)0;
  set_matrix( C );
}


//------------------------------------------
template <class T>
void vpgl_affine_camera<T>::set_rows(
  const vnl_vector_fixed<T,4>& row1,
  const vnl_vector_fixed<T,4>& row2 )
 {
  vnl_matrix_fixed<T,3,4> C( (T)0 );
  for ( unsigned int i = 0; i < 4; i++ ) {
    C(0,i) = row1(i);
    C(1,i) = row2(i);
  }
  C(2,3) = (T)1;
  set_matrix( C );
 }

// Code for easy instantiation.
#undef vpgl_AFFINE_CAMERA_INSTANTIATE
#define vpgl_AFFINE_CAMERA_INSTANTIATE(T) \
template class vpgl_affine_camera<T >


#endif // vpgl_affine_camera_txx_
