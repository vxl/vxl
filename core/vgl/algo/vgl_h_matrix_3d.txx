// This is core/vgl/algo/vgl_h_matrix_3d.txx
#ifndef vgl_h_matrix_3d_txx_
#define vgl_h_matrix_3d_txx_
//:
// \file

#include "vgl_h_matrix_3d.h"
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_transpose.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_quaternion.h>
//--------------------------------------------------------------
//
//: Default constructor
template <class T>
vgl_h_matrix_3d<T>::vgl_h_matrix_3d()
{
}

//: Copy constructor
template <class T>
vgl_h_matrix_3d<T>::vgl_h_matrix_3d(const vgl_h_matrix_3d<T>& M)
{
  t12_matrix_ = M.t12_matrix_;
}

//--------------------------------------------------------------
//
//: Constructor
template <class T>
vgl_h_matrix_3d<T>::vgl_h_matrix_3d(vnl_matrix_fixed<T, 4, 4> const& M)
{
  t12_matrix_ = M;
}

//--------------------------------------------------------------
//
//: Load H from ASCII vcl_istream.
template <class T>
vgl_h_matrix_3d<T>::vgl_h_matrix_3d(vcl_istream& s)
{
  read(s);
}
//--------------------------------------------------------------
//
//: Load from file
template <class T>
vgl_h_matrix_3d<T>::vgl_h_matrix_3d(char const* filename)
{
  vcl_ifstream f(filename);
  if (!f.good())
    vcl_cerr << "vgl_h_matrix_3d::read: Error opening " << filename << vcl_endl;
  else
    t12_matrix_.read_ascii(f);
}

//--------------------------------------------------------------
//
//: Construct an affine vgl_h_matrix_3d from 3x3 M and 3x1 m.
//
template <class T>
vgl_h_matrix_3d<T>::vgl_h_matrix_3d(vnl_matrix_fixed<T,3,3> const& M,
                                    vnl_vector_fixed<T,3> const& m)
{
  for (int r = 0; r < 3; ++r) {
    for (int c = 0; c < 3; ++c)
      (t12_matrix_)(r, c) = M(r,c);
    (t12_matrix_)(r, 3) = m(r);
  }
  for (int c = 0; c < 3; ++c)
    (t12_matrix_)(3,c) = 0;
  (t12_matrix_)(3,3) = 1;
}

//--------------------------------------------------------------
//
//: Construct from a 16-element row-storage array of double.
template <class T>
vgl_h_matrix_3d<T>::vgl_h_matrix_3d (const T* t_matrix)
  : t12_matrix_ (t_matrix)
{
}

//: Destructor
template <class T>
vgl_h_matrix_3d<T>::~vgl_h_matrix_3d()
{
}

// == OPERATIONS ==

//-----------------------------------------------------------------------------
//:return the transformed point
template <class T>
vgl_homg_point_3d<T>
vgl_h_matrix_3d<T>::operator()(vgl_homg_point_3d<T> const & x) const
{
  vnl_vector_fixed<T, 4> v;
  v[0]=x.x();  v[1]=x.y();   v[2]=x.z();   v[3]=x.w();
  vnl_vector_fixed<T,4> v2 = t12_matrix_ * v;
  return vgl_homg_point_3d<T>(v2[0], v2[1], v2[2], v2[3]);
}

//-----------------------------------------------------------------------------
//
//: Return the preimage of a transformed plane

template <class T>
vgl_homg_plane_3d<T>
vgl_h_matrix_3d<T>::preimage(vgl_homg_plane_3d<T> const& p)
{
  vnl_vector_fixed<T, 4> v;
  v[0]=p.a();  v[1]=p.b();   v[2]=p.c();   v[3]=p.d();
  vnl_vector_fixed<T,4> v2 = vnl_transpose(t12_matrix_) * v;
  return vgl_homg_plane_3d<T>(v2[0], v2[1], v2[2], v2[3]);
}

//-----------------------------------------------------------------------------
//
//: Return the preimage of a transformed point (requires an inverse)
template <class T>
vgl_homg_point_3d<T>
vgl_h_matrix_3d<T>::preimage(vgl_homg_point_3d<T> const& x) const
{
  vnl_vector_fixed<T, 4> v;
  v[0]=x.x();  v[1]=x.y();   v[2]=x.z();   v[3]=x.w();
  v = vnl_inverse(t12_matrix_) * v;
  return vgl_homg_point_3d<T>(v[0], v[1], v[2], v[3]);
}

//-----------------------------------------------------------------------------
//
//: Transform a plane (requires an inverse)
template <class T>
vgl_homg_plane_3d<T>
vgl_h_matrix_3d<T>::operator()(vgl_homg_plane_3d<T> const& p) const
{
  vnl_vector_fixed<T, 4> v;
  v[0]=p.a();  v[1]=p.b();   v[2]=p.c();   v[3]=p.d();
  v = vnl_inverse_transpose(t12_matrix_) * v;
  return vgl_homg_plane_3d<T>(v[0], v[1], v[2], v[3]);
}

//-----------------------------------------------------------------------------
//: Print H on vcl_ostream
template <class T>
vcl_ostream& operator<<(vcl_ostream& s, const vgl_h_matrix_3d<T>& h)
{
  return s << h.get_matrix();
}

//: Load H from ASCII file.
template <class T>
bool vgl_h_matrix_3d<T>::read(vcl_istream& s)
{
  t12_matrix_.read_ascii(s);
  return s.good() || s.eof();
}

//: Load H from ASCII file.
template <class T>
vcl_istream& operator>>(vcl_istream& s, vgl_h_matrix_3d<T>& H)
{
  H.read(s);
  return s;
}

// == DATA ACCESS ==

//-----------------------------------------------------------------------------
//: Get matrix element at (row_index, col_index)
template <class T>
T vgl_h_matrix_3d<T>::get (unsigned int row_index, unsigned int col_index) const
{
  return t12_matrix_.get(row_index, col_index);
}

//-----------------------------------------------------------------------------
//: Fill t_matrix with contents of H
template <class T>
void vgl_h_matrix_3d<T>::get (T* t_matrix) const
{
  for (int row_index = 0; row_index < 4; row_index++)
    for (int col_index = 0; col_index < 4; col_index++)
      *t_matrix++ = t12_matrix_.get(row_index, col_index);
}

//-----------------------------------------------------------------------------
//: Fill t_matrix with contents of H
template <class T>
void vgl_h_matrix_3d<T>::get (vnl_matrix_fixed<T, 4, 4>* t_matrix) const
{
  *t_matrix = t12_matrix_;
}

//-----------------------------------------------------------------------------
//: Return the inverse of this vgl_h_matrix_3d<T>.
template <class T>
vgl_h_matrix_3d<T> vgl_h_matrix_3d<T>::get_inverse() const
{
  vnl_matrix_fixed<T, 4, 4> temp = vnl_inverse(t12_matrix_);
  return vgl_h_matrix_3d<T>(temp);
}

//: Set to 4x4 row-stored matrix
template <class T>
void vgl_h_matrix_3d<T>::set (const T* H)
{
  for (int row_index = 0; row_index < 4; row_index++)
    for (int col_index = 0; col_index < 4; col_index++)
      t12_matrix_. put (row_index, col_index, *H++);
}

//: Set to given vnl_matrix
template <class T>
void vgl_h_matrix_3d<T>::set (vnl_matrix_fixed<T,4,4> const& H)
{
  t12_matrix_ = H;
}

//: Compute transform to projective basis given five points, no 4 coplanar
template <class T>
bool vgl_h_matrix_3d<T>::
projective_basis(vcl_vector<vgl_homg_point_3d<T> > const & /*five_points*/)
{
  vcl_cerr << "vgl_h_matrix_3d<T>::projective_basis(5pts) not yet implemented\n";
  return false;
}

//: Set to identity
template <class T>
void vgl_h_matrix_3d<T>::set_identity ()
{
  t12_matrix_.set_identity();
}

//: Set to translation
template <class T>
void vgl_h_matrix_3d<T>::set_translation(T tx, T ty, T tz)
{
  (t12_matrix_)(0, 3)  = tx;
  (t12_matrix_)(1, 3)  = ty;
  (t12_matrix_)(2, 3)  = tz;
}

//: Set to rotation about an axis
template <class T>
void vgl_h_matrix_3d<T>::
set_rotation_about_axis(const vnl_vector_fixed<T,3>& axis, T angle)
{
  vnl_quaternion<T> q(axis, angle);
  //get the transpose of the rotation matrix
  vnl_matrix_fixed<T,3,3> R = q.rotation_matrix_transpose();
  //fill in with the transpose
  for (int c = 0; c<3; c++)
    for (int r = 0; r<3; r++)
      t12_matrix_[r][c]=R[c][r];
}

//: Set to roll, pitch and yaw specified rotation.
// - roll is rotation about z
// - pitch is rotation about y
// - yaw is rotation about x

template <class T>
void vgl_h_matrix_3d<T>::
set_rotation_roll_pitch_yaw(T yaw, T pitch, T roll)
{
  typedef typename vnl_numeric_traits<T>::real_t real_t;
  real_t ax = yaw/2, ay = pitch/2, az = roll/2;

  vnl_quaternion<T> qx(vcl_sin(ax),0,0,vcl_cos(ax));
  vnl_quaternion<T> qy(0,vcl_sin(ay),0,vcl_cos(ay));
  vnl_quaternion<T> qz(0,0,vcl_sin(az),vcl_cos(az));
  vnl_quaternion<T> q = qz*qy*qx;

  vnl_matrix_fixed<T,3,3> R = q.rotation_matrix_transpose();
  //fill in with the transpose
  for (int c = 0; c<3; c++)
    for (int r = 0; r<3; r++)
      t12_matrix_[r][c]=R[c][r];
}

//: Set to rotation specified by Euler angles
template <class T>
void vgl_h_matrix_3d<T>::
set_rotation_euler(T rz1, T ry, T rz2)
{
  typedef typename vnl_numeric_traits<T>::real_t real_t;
  real_t az1 = rz1/2, ay = ry/2, az2 = rz2/2;

  vnl_quaternion<T> qz1(0,0,vcl_sin(az1),vcl_cos(az1));
  vnl_quaternion<T> qy(0,vcl_sin(ay),0,vcl_cos(ay));
  vnl_quaternion<T> qz2(0,0,vcl_sin(az2),vcl_cos(az2));
  vnl_quaternion<T> q = qz2*qy*qz1;

  vnl_matrix_fixed<T,3,3> R = q.rotation_matrix_transpose();
  //fill in with the transpose
  for (int c = 0; c<3; c++)
    for (int r = 0; r<3; r++)
      t12_matrix_[r][c]=R[c][r];
}


//----------------------------------------------------------------------------
#undef VGL_H_MATRIX_3D_INSTANTIATE
#define VGL_H_MATRIX_3D_INSTANTIATE(T) \
template class vgl_h_matrix_3d<T >; \
template vcl_ostream& operator << (vcl_ostream& s, const vgl_h_matrix_3d<T >& h); \
template vcl_istream& operator >> (vcl_istream& s, vgl_h_matrix_3d<T >& h)

#endif // vgl_h_matrix_3d_txx_
