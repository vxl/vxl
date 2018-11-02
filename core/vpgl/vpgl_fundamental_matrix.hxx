// This is core/vpgl/vpgl_fundamental_matrix.hxx
#ifndef vpgl_fundamental_matrix_hxx_
#define vpgl_fundamental_matrix_hxx_
//:
// \file

#include <iosfwd>
#include "vpgl_fundamental_matrix.h"
#include "vpgl_essential_matrix.h"
#include "vpgl_calibration_matrix.h"

//
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_cross_product_matrix.h>
#include <vnl/vnl_inverse.h>
//#include <vnl/io/vnl_io_matrix_fixed.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>


//---------------------------------
template <class T>
vpgl_fundamental_matrix<T>::vpgl_fundamental_matrix()
  : cached_svd_(nullptr)
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
  : cached_svd_(nullptr)
{
  set_matrix( other.F_ );
}

//---------------------------------
//: From Essential Matrix.
//  Since $E = Kl^T \times F \times Kr$, $F = Kl^{-T} \times E \times Kr^{-1}$
template <class T>
vpgl_fundamental_matrix<T>::vpgl_fundamental_matrix(
    const vpgl_calibration_matrix<T> &kr,
    const vpgl_calibration_matrix<T> &kl,
    const vpgl_essential_matrix<T>   &em)
  : cached_svd_(nullptr)
{
  vnl_matrix_fixed<T, 3, 3> kl_tinv = vnl_inverse(kl.get_matrix().transpose());
  vnl_matrix_fixed<T, 3, 3> kr_inv = vnl_inverse(kr.get_matrix());
  this->set_matrix(kl_tinv * em.get_matrix() * kr_inv);
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
vgl_homg_line_2d<T> vpgl_fundamental_matrix<T>::r_epipolar_line(
    const vgl_homg_line_2d<T> &epiline_l) const
{
  vgl_homg_point_2d<T> er, el;

  get_epipoles(er, el);

  vgl_homg_line_2d<T> el_as_line(el.x(), el.y(), el.w());

  vnl_vector_fixed<T,3> epiline_r =
    get_matrix().transpose() *
    vgl_homg_operators_2d<T>::get_vector(
        vgl_homg_operators_2d<T>::intersection( el_as_line , epiline_l));

  return vgl_homg_line_2d<T>(epiline_r[0], epiline_r[1], epiline_r[2]);
}


//---------------------------------
template <class T>
vgl_homg_line_2d<T> vpgl_fundamental_matrix<T>::l_epipolar_line(
    const vgl_homg_line_2d<T> &epiline_r) const
{
  vgl_homg_point_2d<T> er, el;

  get_epipoles(er, el);

  vgl_homg_line_2d<T> er_as_line(er.x(), er.y(), er.w());

  vnl_vector_fixed<T,3> epiline_l =
    get_matrix() *
    vgl_homg_operators_2d<T>::get_vector(
        vgl_homg_operators_2d<T>::intersection( er_as_line , epiline_r));

  return vgl_homg_line_2d<T>(epiline_l[0], epiline_l[1], epiline_l[2]);
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
  P.set_columns( 0, (elx*F_+elvt).as_ref() ); // size 3x3
  P.set_column( 3, vnl_vector_fixed<T,3>( lambda*el.x(), lambda*el.y(), lambda*el.w() ) );
  return P;
}



//--------------------------------
template <class T>
vpgl_proj_camera<T> vpgl_fundamental_matrix<T>::extract_left_camera(
    const std::vector< vgl_point_3d<T> >& world_points,
    const std::vector< vgl_point_2d<T> >& image_points ) const
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

  vnl_matrix<T> A(static_cast<unsigned int>(3 * image_points.size()), 4 );
  vnl_vector<T> y( 3*image_points.size() );
  for ( unsigned p = 0; p < image_points.size(); p++ ) {
    vnl_vector_fixed<T,3> wp_vnl(
      world_points[p].x(), world_points[p].y(), world_points[p].z() );
    vnl_vector_fixed<T,3> ip_vnl(
      image_points[p].x(), image_points[p].y(), (T)1 );
    vnl_vector_fixed<T,3> yp = ip_vnl - elxF * wp_vnl;
    T ei;
    for ( unsigned i = 0; i < 3; i++ ) {
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
  vnl_vector_fixed<T, 4> nv = cr.svd()->nullvector();
  vnl_matrix_fixed<T, 3, 4> m = cl.get_matrix();
  vnl_vector_fixed<T, 3> nvt =  m * nv;
  vnl_vector_fixed<double, 3> nvtd;
  // unfortunately, vnl_cross_product_matrix is only defined for double
  for (unsigned i = 0; i<3; ++i)
    nvtd[i] = static_cast<double>(nvt[i]);
  vnl_cross_product_matrix e2x( nvtd );
  vnl_matrix_fixed<T, 3, 3> e2xt;
   for (unsigned r = 0; r<3; ++r)
    for (unsigned c = 0; c<3; ++c)
      e2xt[r][c] = static_cast<T>(e2x[r][c]);
   vnl_matrix_fixed<T, 3, 3> temp = e2xt * (cl.get_matrix() * cr.svd()->inverse());
   set_matrix( temp );
}


//---------------------------------
template <class T>
void vpgl_fundamental_matrix<T>::set_matrix( const vnl_matrix_fixed<T,3,3>& F )
{
  F_ = vnl_svd<T>( F.as_ref() ).recompose(2);
  if ( cached_svd_ != nullptr ) delete cached_svd_;
  cached_svd_ = new vnl_svd<T>( F_.as_ref() );
}

//: write vpgl_fundamental_matrix to stream
template <class T>
std::ostream&  operator<<(std::ostream& s, vpgl_fundamental_matrix<T> const& p)
{
  s << p.get_matrix();
  return s;
}

//: Read vpgl_perspective_camera from stream
template <class T>
std::istream&  operator>>(std::istream& s, vpgl_fundamental_matrix<T>& p)
{
  vnl_matrix_fixed<T, 3, 3> m;
  s >> m;
  p.set_matrix(m);
  return s;
}

// Macro for easy instantiation.
#undef vpgl_FUNDAMENTAL_MATRIX_INSTANTIATE
#define vpgl_FUNDAMENTAL_MATRIX_INSTANTIATE(T) \
template class vpgl_fundamental_matrix<T >; \
template std::ostream& operator<<(std::ostream&, const vpgl_fundamental_matrix<T >&); \
template std::istream& operator>>(std::istream&, vpgl_fundamental_matrix<T >&)

#endif // vpgl_fundamental_matrix_hxx_
