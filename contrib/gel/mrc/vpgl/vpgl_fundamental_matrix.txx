// This is gel/mrc/vpgl/vpgl_fundamental_matrix.txx
#ifndef vpgl_fundamental_matrix_txx_
#define vpgl_fundamental_matrix_txx_
//:
// \file

#include <vnl/vnl_fwd.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_rank.h>// not used
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_cross_product_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/io/vnl_io_matrix_fixed.h>
#include <vcl_iosfwd.h>
#include <vcl_cassert.h>

#include "vpgl_fundamental_matrix.h"


//---------------------------------
template <class T>
vpgl_fundamental_matrix<T>::vpgl_fundamental_matrix()
  : cached_svd_(NULL)
{
  vnl_matrix_fixed<T,3,3> default_matrix( (T)0 );
  default_matrix(0,0) = default_matrix(1,1) = (T)1;
  set_matrix( default_matrix );
}

//---------------------------------
//: Copy Constructor
template <class T>
vpgl_fundamental_matrix<T>::vpgl_fundamental_matrix(
  const vpgl_fundamental_matrix<T>& other)
  : cached_svd_(NULL)
{
  set_matrix( other.F_ );
}


//---------------------------------
template <class T>
const vpgl_fundamental_matrix<T>&
vpgl_fundamental_matrix<T>::operator=( const vpgl_fundamental_matrix<T>& fm )
{
  set_matrix( fm.F_ );
  return *this;
}


//---------------------------------
//: Destructor
template <class T>
vpgl_fundamental_matrix<T>::~vpgl_fundamental_matrix()
{
  delete cached_svd_;
}


//---------------------------------
template <class T>
void vpgl_fundamental_matrix<T>::get_epipoles( vgl_homg_point_2d<T>& er,
  vgl_homg_point_2d<T>& el ) const
{
  vnl_vector_fixed<T,3> v = cached_svd_->nullvector();
  er.set( v[0], v[1], v[2] );
  v = cached_svd_->left_nullvector();
  el.set( v[0], v[1], v[2] );
}


//---------------------------------
template <class T>
vgl_homg_line_2d<T> vpgl_fundamental_matrix<T>::r_epipolar_line(
  const vgl_homg_point_2d<T>& pl ) const
{
  vnl_vector_fixed<T,3> lr = F_.transpose() *
    vnl_vector_fixed<T,3>( pl.x(), pl.y(), pl.w() );
  return vgl_homg_line_2d<T>( lr(0), lr(1), lr(2) );
}


//---------------------------------
template <class T>
vgl_homg_line_2d<T> vpgl_fundamental_matrix<T>::l_epipolar_line(
  const vgl_homg_point_2d<T>& pr ) const
{
  vnl_vector_fixed<T,3> ll =
    F_ * vnl_vector_fixed<T,3>( pr.x(), pr.y(), pr.w() );
  return vgl_homg_line_2d<T>( ll(0), ll(1), ll(2) );
}


//---------------------------------
template <class T>
vpgl_proj_camera<T> vpgl_fundamental_matrix<T>::extract_left_camera(
  const vnl_vector_fixed<T,3>& v, T lambda ) const
{
  vgl_homg_point_2d<T> er, el;
  get_epipoles( er, el );
  vnl_matrix_fixed<T,3,3> elx((T)0);
  elx.put( 0, 1, -el.w() ); elx.put( 0, 2, el.y() );
  elx.put( 1, 0, el.w() ); elx.put( 1, 2, -el.x() );
  elx.put( 2, 0, -el.y() ); elx.put( 2, 1, el.x() );

  vnl_matrix_fixed<T,3,3> elvt;
  elvt(0,0) = el.x()*v[0]; elvt(1,0) = el.y()*v[0]; elvt(2,0) = el.w()*v[0];
  elvt(0,1) = el.x()*v[1]; elvt(1,1) = el.y()*v[1]; elvt(2,1) = el.w()*v[1];
  elvt(0,2) = el.x()*v[2]; elvt(1,2) = el.y()*v[2]; elvt(2,2) = el.w()*v[2];

  vnl_matrix_fixed<T,3,4> P;
  P.set_columns( 0, elx*F_+elvt );
  P.set_column( 3, vnl_vector_fixed<T,3>( lambda*el.x(), lambda*el.y(), lambda*el.w() ) );
  return P;
}


//--------------------------------
template <class T>
vpgl_proj_camera<T> vpgl_fundamental_matrix<T>::extract_left_camera(
    const vcl_vector< vgl_point_3d<T> >& world_points,
    const vcl_vector< vgl_point_2d<T> >& image_points ) const
{
  assert( world_points.size() == image_points.size() );
  assert( world_points.size() >= 2 );

  vgl_homg_point_2d<T> er, el;
  get_epipoles( er, el );
  vnl_matrix_fixed<T,3,3> elxF((T)0);
  elxF.put( 0, 1, -el.w() ); elxF.put( 0, 2, el.y() );
  elxF.put( 1, 0, el.w() ); elxF.put( 1, 2, -el.x() );
  elxF.put( 2, 0, -el.y() ); elxF.put( 2, 1, el.x() );
  elxF*=F_;

  vnl_matrix<T> A( 3*image_points.size(), 4 );
  vnl_vector<T> y( 3*image_points.size() );
  for ( unsigned p = 0; p < image_points.size(); p++ ){
    vnl_vector_fixed<T,3> wp_vnl(
      world_points[p].x(), world_points[p].y(), world_points[p].z() );
    vnl_vector_fixed<T,3> ip_vnl(
      image_points[p].x(), image_points[p].y(), (T)1 );
    vnl_vector_fixed<T,3> yp = ip_vnl - elxF * wp_vnl;
    T ei;
    for ( unsigned i = 0; i < 3; i++ ){
      y(3*p+i) = yp(i);
      if ( i == 0 ) ei = el.x(); else if ( i == 1 ) ei = el.y(); else ei = el.w();
      A(3*p+i,0) = ei*wp_vnl[0]; A(3*p+i,1) = ei*wp_vnl[1]; A(3*p+i,2) = ei*wp_vnl[2];
      A(3*p+i,3) = ei;
    }
  }

  // Do least squares estimation of y=Ax
  vnl_vector<T> x = vnl_svd<T>(A).solve(y);
  vnl_vector_fixed<T,3> v( x(0), x(1), x(2) );
  T lambda = x(3);
  return extract_left_camera( v, lambda );
}


//---------------------------------
template <class T>
void vpgl_fundamental_matrix<T>::set_matrix( const vpgl_proj_camera<T>& cr,
                                              const vpgl_proj_camera<T>& cl )
{
  vnl_cross_product_matrix e2x( cl.get_matrix() * cr.svd()->nullvector() );
  set_matrix( e2x * cl.get_matrix() * cr.svd()->inverse() );
}


//---------------------------------
template <class T>
void vpgl_fundamental_matrix<T>::set_matrix( const vnl_matrix_fixed<T,3,3>& F )
{
  F_ = vnl_svd<T>( F ).recompose(2);
  if ( cached_svd_ != NULL ) delete cached_svd_;
  cached_svd_ = new vnl_svd<T>( F_ );
}

//:vpgl_fundamental_matrix stream I/O

template <class T>
vcl_ostream&  operator<<(vcl_ostream& s, vpgl_fundamental_matrix<T> const& p)
{
  s << p.get_matrix();
  return s;
}

//: Read vpgl_perspective_camera  from stream
template <class T>
vcl_istream&  operator>>(vcl_istream& s, vpgl_fundamental_matrix<T>& p)
{
  vnl_matrix_fixed<T, 3, 3> m;
  s >> m;
  p.set_matrix(m);
  return s;
}
// Code for easy instantiation.
#undef vpgl_FUNDAMENTAL_MATRIX_INSTANTIATE
#define vpgl_FUNDAMENTAL_MATRIX_INSTANTIATE(T) \
template class vpgl_fundamental_matrix<T >; \
template vcl_ostream& operator<<(vcl_ostream&, const vpgl_fundamental_matrix<T >&); \
template vcl_istream& operator>>(vcl_istream&, vpgl_fundamental_matrix<T >&)

#endif // vpgl_fundamental_matrix_txx_
