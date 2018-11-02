// This is core/vpgl/vpgl_essential_matrix.hxx
#ifndef vpgl_essential_matrix_hxx_
#define vpgl_essential_matrix_hxx_
//:
// \file

#include <iosfwd>
#include "vpgl_essential_matrix.h"
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_det.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vpgl_proj_camera.h"


//---------------------------------
template <class T>
vpgl_essential_matrix<T>::vpgl_essential_matrix() = default;

template <class T>
vpgl_essential_matrix<T>::
vpgl_essential_matrix( const vpgl_fundamental_matrix<T>& F,
                       const vpgl_calibration_matrix<T>& K_left,
                       const vpgl_calibration_matrix<T>& K_right)
{
  vnl_matrix_fixed<T, 3, 3> klm = K_left.get_matrix();
  vnl_matrix_fixed<T, 3, 3> krm = K_right.get_matrix();
  vnl_matrix_fixed<T, 3, 3> fm = F.get_matrix();
  this->set_matrix(klm.transpose()*fm*krm);
}

//--------------------------------------------
//: Compute from Euclidean properties of perspective cameras
template <class T>
vpgl_essential_matrix<T>::
vpgl_essential_matrix( const vpgl_perspective_camera<T>& cr,
                       const vpgl_perspective_camera<T>& cl )
{
  //copy the cameras
  vpgl_perspective_camera<T> crc = cr, clc = cl;
  //set the calibration matrices to identity
  vpgl_calibration_matrix<T> K;
  crc.set_calibration(K); clc.set_calibration(K);
  vpgl_proj_camera<T> ppr = static_cast<vpgl_proj_camera<T> >(crc);
  vpgl_proj_camera<T> ppl = static_cast<vpgl_proj_camera<T> >(clc);
  this->set_matrix(crc, clc);
}

//---------------------------------
//: Copy Constructor
template <class T>
vpgl_essential_matrix<T>::
vpgl_essential_matrix(const vpgl_essential_matrix<T>& other)
: vpgl_fundamental_matrix<double>(other)
{
}

//---------------------------------
template <class T>
const vpgl_essential_matrix<T>&
vpgl_essential_matrix<T>::operator=( const vpgl_essential_matrix<T>& fm )
{
  vpgl_fundamental_matrix<T>::set_matrix( fm.F_ );
  return *this;
}

//---------------------------------
//: Destructor
template <class T>
vpgl_essential_matrix<T>::~vpgl_essential_matrix() = default;

//---------------------------------
// Decompose the essential matrix to obtain rotation and translation of
// the "left" camera. A correspondence is needed in order to remove the
// ambiguity of the decomposition. There are four possible solutions.
// The translation vector, t, in the decomposition,[R|t], is normalized to
// have magnitude, translation_mag.
template <class T>
bool extract_left_camera(const vpgl_essential_matrix<T>& E,
                         const vgl_point_2d<T>& left_corr,
                         const vgl_point_2d<T>& right_corr,
                         vpgl_perspective_camera<T>& p_left,
                         const T translation_mag )
{
  //The right camera is the identity camera [I|0]
  vpgl_perspective_camera<T> p_right;

  vnl_matrix_fixed<T,3,3> W;
  W[0][0]=0; W[0][1]=-1;W[0][2]=0;
  W[1][0]=1; W[1][1]=0; W[1][2]=0;
  W[2][0]=0; W[2][1]=0; W[2][2]=1;

  vnl_svd<T> SVD(E.get_matrix().as_ref());
  vnl_matrix_fixed<T,3,3> U=SVD.U();
  vnl_matrix_fixed<T,3,3> V=SVD.V();
  vnl_matrix_fixed<T,3,3> R;
  vnl_vector_fixed<T, 3> t;
  // checking for the correct combination of cameras
  for ( int c = 0; c < 4; c++ )
  {
    if ( c == 0 ) { //case 1
      R=U*W.transpose()*V.transpose();
      t=U.get_column(2);
    }
    if ( c == 1 ) { //case 2
      R=U*W*V.transpose();
      t=U.get_column(2);
    }
    if ( c == 2 ) { //case 3
      R=U*W.transpose()*V.transpose();
      t=-U.get_column(2);
    }
    if ( c == 3 ) { //case 4
      R=U*W*V.transpose();
      t=-U.get_column(2);
    }
    if ( vnl_det<T>( R ) < 0 ) R = -R ;
    t*=translation_mag;
    vnl_vector_fixed<T, 3> cc = -R.transpose()*t;
    p_left.set_rotation( vgl_rotation_3d<T>(R) );
    p_left.set_camera_center( vgl_point_3d<T>( cc(0), cc(1), cc(2) ) );
    vpgl_proj_camera<T> ppl =
      static_cast<vpgl_proj_camera<T> >(p_left);
    vpgl_proj_camera<T> ppr =
      static_cast<vpgl_proj_camera<T> >(p_right);
    //Test to see if the 3-d point formed by triangulation is in front
    //of the camera. There should exist one solution.
    vgl_point_3d<T> p3d = triangulate_3d_point<T>(ppl, left_corr, ppr, right_corr );
    vgl_homg_point_3d<T> ph3d(p3d);
    if (!p_right.is_behind_camera(ph3d) && !p_left.is_behind_camera(ph3d))
      break;
    if ( c == 3 ) {
      std::cerr << "ERROR: extract_left_camera in vpgl_essential_matrix failed.\n";
      return false;
    }
  }

  return true;
}

template <class T>
std::ostream&  operator<<(std::ostream& s, vpgl_essential_matrix<T> const& p)
{
  s << p.get_matrix();
  return s;
}

//: Read vpgl_perspective_camera  from stream
template <class T>
std::istream&  operator>>(std::istream& s, vpgl_essential_matrix<T>& p)
{
  vnl_matrix_fixed<T, 3, 3> m;
  s >> m;
  p.set_matrix(m);
  return s;
}

// Code for easy instantiation.
#undef vpgl_ESSENTIAL_MATRIX_INSTANTIATE
#define vpgl_ESSENTIAL_MATRIX_INSTANTIATE(T) \
template class vpgl_essential_matrix<T >; \
template bool extract_left_camera(const vpgl_essential_matrix<T >& E, \
                                  const vgl_point_2d<T >& left_corr, \
                                  const vgl_point_2d<T >& right_corr, \
                                  vpgl_perspective_camera<T >& p_left, \
                                  const T translation_mag); \
template std::ostream& operator<<(std::ostream&, const vpgl_essential_matrix<T >&); \
template std::istream& operator>>(std::istream&, vpgl_essential_matrix<T >&)


#endif // vpgl_essential_matrix_hxx_
