// This is core/vgl/algo/vgl_h_matrix_3d.hxx
#ifndef vgl_h_matrix_3d_hxx_
#define vgl_h_matrix_3d_hxx_

#include <iostream>
#include <fstream>
#include <cmath>
#include <limits>
#include <cstdlib>
#include "vgl_h_matrix_3d.h"
#include <cassert>
#include <vcl_compiler_detection.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_plane_3d.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/algo/vnl_svd.h>
# include <vcl_deprecated.h>

template <class T>
vgl_h_matrix_3d<T>::vgl_h_matrix_3d(std::vector<vgl_homg_point_3d<T> > const& points1,
                                    std::vector<vgl_homg_point_3d<T> > const& points2)
{
  vnl_matrix<T> W;
  assert(points1.size() == points2.size());
  unsigned int numpoints = static_cast<int>( points1.size());
  if (numpoints < 5)
  {
    std::cerr << "\nvhl_h_matrix_3d - minimum of 5 points required\n";
    std::exit(0);
  }

  W.set_size(3*numpoints, 16);

  for (unsigned int i = 0; i < numpoints; i++)
  {
    T x1 = points1[i].x(), y1 = points1[i].y(), z1 = points1[i].z(), w1 = points1[i].w();
    T x2 = points2[i].x(), y2 = points2[i].y(), z2 = points2[i].z(), w2 = points2[i].w();

    W[i*3][0]=x1*w2;     W[i*3][1]=y1*w2;     W[i*3][2]=z1*w2;     W[i*3][3]=w1*w2;
    W[i*3][4]=0.0;       W[i*3][5]=0.0;       W[i*3][6]=0.0;       W[i*3][7]=0.0;
    W[i*3][8]=0.0;       W[i*3][9]=0.0;       W[i*3][10]=0.0;      W[i*3][11]=0.0;
    W[i*3][12]=-x1*x2;   W[i*3][13]=-y1*x2;   W[i*3][14]=-z1*x2;   W[i*3][15]=-w1*x2;

    W[i*3+1][0]=0.0;     W[i*3+1][1]=0.0;     W[i*3+1][2]=0.0;     W[i*3+1][3]=0.0;
    W[i*3+1][4]=x1*w2;   W[i*3+1][5]=y1*w2;   W[i*3+1][6]=z1*w2;   W[i*3+1][7]=w1*w2;
    W[i*3+1][8]=0.0;     W[i*3+1][9]=0.0;     W[i*3+1][10]=0.0;    W[i*3+1][11]=0.0;
    W[i*3+1][12]=-x1*y2; W[i*3+1][13]=-y1*y2; W[i*3+1][14]=-z1*y2; W[i*3+1][15]=-w1*y2;

    W[i*3+2][0]=0.0;     W[i*3+2][1]=0.0;     W[i*3+2][2]=0.0;     W[i*3+2][3]=0.0;
    W[i*3+2][4]=0.0;     W[i*3+2][5]=0.0;     W[i*3+2][6]=0.0;     W[i*3+2][7]=0.0;
    W[i*3+2][8]=x1*w2;   W[i*3+2][9]=y1*w2;   W[i*3+2][10]=z1*w2;  W[i*3+2][11]=w1*w2;
    W[i*3+2][12]=-x1*z2; W[i*3+2][13]=-y1*z2; W[i*3+2][14]=-z1*z2; W[i*3+2][15]=-w1*z2;
  }

  vnl_svd<T> SVD(W);
  t12_matrix_ = vnl_matrix_fixed<T,4,4>(SVD.nullvector().data_block()); // 16-dim. nullvector
}

template <class T>
vgl_h_matrix_3d<T>::vgl_h_matrix_3d(std::istream& s)
{
  t12_matrix_.read_ascii(s);
}

template <class T>
vgl_h_matrix_3d<T>::vgl_h_matrix_3d(char const* filename)
{
  std::ifstream f(filename);
  if (!f.good())
    std::cerr << "vgl_h_matrix_3d::read: Error opening " << filename << std::endl;
  else
    t12_matrix_.read_ascii(f);
}

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

// == OPERATIONS ==

//-----------------------------------------------------------------------------
//
template <class T>
vgl_homg_point_3d<T>
vgl_h_matrix_3d<T>::operator()(vgl_homg_point_3d<T> const& p) const
{
  vnl_vector_fixed<T,4> v2 = t12_matrix_ * vnl_vector_fixed<T,4>(p.x(), p.y(), p.z(), p.w());
  return vgl_homg_point_3d<T>(v2[0], v2[1], v2[2], v2[3]);
}

template <class T>
vgl_pointset_3d<T> vgl_h_matrix_3d<T>::operator()(vgl_pointset_3d<T> const& ptset) const{
  vgl_pointset_3d<T> ret;
  bool has_norms = ptset.has_normals();
  unsigned np = ptset.npts();
  std::vector<vgl_point_3d<T> > pts;
  std::vector<vgl_vector_3d<T> > normals;
  for(unsigned i =0; i<np; ++i){
    vgl_homg_point_3d<T> hp = (*this)(vgl_homg_point_3d<T>(ptset.p(i)));
    pts.push_back(vgl_point_3d<T>(hp));
    if(has_norms){
      vgl_vector_3d<T> norm = ptset.n(i);
      vgl_homg_point_3d<T> hn(norm.x(), norm.y(), norm.z(), 0.0);//direction vector
      vgl_homg_point_3d<T> hhn = (*this)(hn);
      vgl_vector_3d<T> hv(hhn.x(), hhn.y(), hhn.z());
      normals.push_back(hv);
    }
  }
  if(has_norms)
    ret.set_points_with_normals(pts, normals);
  else
    ret.set_points(pts);
  return ret;
}


template <class T>
vgl_homg_plane_3d<T>
vgl_h_matrix_3d<T>::correlation(vgl_homg_point_3d<T> const& p) const
{
  vnl_vector_fixed<T,4> v2 = t12_matrix_ * vnl_vector_fixed<T,4>(p.x(), p.y(), p.z(), p.w());
  return vgl_homg_plane_3d<T>(v2[0], v2[1], v2[2], v2[3]);
}

template <class T>
vgl_homg_plane_3d<T>
vgl_h_matrix_3d<T>::preimage(vgl_homg_plane_3d<T> const& l) const
{
  vnl_vector_fixed<T,4> v2 = t12_matrix_.transpose() * vnl_vector_fixed<T,4>(l.a(), l.b(), l.c(), l.d());
  return vgl_homg_plane_3d<T>(v2[0], v2[1], v2[2], v2[3]);
}

template <class T>
vgl_homg_point_3d<T>
vgl_h_matrix_3d<T>::correlation(vgl_homg_plane_3d<T> const& l) const
{
  vnl_vector_fixed<T,4> v2 = t12_matrix_ * vnl_vector_fixed<T,4>(l.a(), l.b(), l.c(), l.d());
  return vgl_homg_point_3d<T>(v2[0], v2[1], v2[2], v2[3]);
}

template <class T>
vgl_homg_point_3d<T>
vgl_h_matrix_3d<T>::preimage(vgl_homg_point_3d<T> const& p) const
{
  vnl_vector_fixed<T,4> v = vnl_inverse(t12_matrix_) * vnl_vector_fixed<T,4>(p.x(), p.y(), p.z(), p.w());
  return vgl_homg_point_3d<T>(v[0], v[1], v[2], v[3]);
}
template <class T>
vgl_pointset_3d<T> vgl_h_matrix_3d<T>::preimage(vgl_pointset_3d<T> const& ptset) const{
  vgl_h_matrix_3d<T> hinv = this->get_inverse();
  return hinv(ptset);
}

template <class T>
vgl_homg_plane_3d<T>
vgl_h_matrix_3d<T>::operator()(vgl_homg_plane_3d<T> const& l) const
{
  vnl_vector_fixed<T,4> v = vnl_inverse_transpose(t12_matrix_) * vnl_vector_fixed<T,4>(l.a(), l.b(), l.c(), l.d());
  return vgl_homg_plane_3d<T>(v[0], v[1], v[2], v[3]);
}

template <class T>
std::ostream& operator<<(std::ostream& s, vgl_h_matrix_3d<T> const& h)
{
  return s << h.get_matrix();
}

template <class T>
bool vgl_h_matrix_3d<T>::read(std::istream& s)
{
  t12_matrix_.read_ascii(s);
  return s.good() || s.eof();
}

template <class T>
bool vgl_h_matrix_3d<T>::read(char const* filename)
{
  std::ifstream f(filename);
  if (!f.good())
    std::cerr << "vgl_h_matrix_3d::read: Error opening " << filename << std::endl;
  return read(f);
}

// == DATA ACCESS ==

template <class T>
T vgl_h_matrix_3d<T>::get (unsigned int row_index, unsigned int col_index) const
{
  return t12_matrix_.get(row_index, col_index);
}

template <class T>
void vgl_h_matrix_3d<T>::get (T* H) const
{
  for (T const* iter = t12_matrix_.begin(); iter < t12_matrix_.end(); ++iter)
    *H++ = *iter;
}

template <class T>
void vgl_h_matrix_3d<T>::get (vnl_matrix_fixed<T,4,4>* H) const
{
  *H = t12_matrix_;
}

template <class T>
void vgl_h_matrix_3d<T>::get (vnl_matrix<T>* H) const
{
  VXL_DEPRECATED_MACRO("vgl_h_matrix_3d<T>::get(vnl_matrix<T>*) const");
  *H = t12_matrix_.as_ref(); // size 4x4
}

template <class T>
vgl_h_matrix_3d<T>
vgl_h_matrix_3d<T>::get_inverse() const
{
  return vgl_h_matrix_3d<T>(vnl_inverse(t12_matrix_));
}

template <class T>
vgl_h_matrix_3d<T>&
vgl_h_matrix_3d<T>::set (T const* H)
{
  for (T* iter = t12_matrix_.begin(); iter < t12_matrix_.end(); ++iter)
    *iter = *H++;
  return *this;
}

template <class T>
vgl_h_matrix_3d<T>&
vgl_h_matrix_3d<T>::set (vnl_matrix_fixed<T,4,4> const& H)
{
  t12_matrix_ = H;
  return *this;
}

template <class T>
bool vgl_h_matrix_3d<T>::projective_basis(std::vector<vgl_homg_point_3d<T> > const& /*five_points*/)
{
  std::cerr << "vgl_h_matrix_3d<T>::projective_basis(5pts) not yet implemented\n";
  return false;
}

template <class T>
bool vgl_h_matrix_3d<T>::projective_basis(std::vector<vgl_homg_plane_3d<T> > const& /*five_planes*/)
{
  std::cerr << "vgl_h_matrix_3d<T>::projective_basis(5planes) not yet implemented\n";
  return false;
}

template <class T>
vgl_h_matrix_3d<T>&
vgl_h_matrix_3d<T>::set_identity ()
{
  t12_matrix_.set_identity();
  return *this;
}

template <class T>
vgl_h_matrix_3d<T>&
vgl_h_matrix_3d<T>::set_translation(T tx, T ty, T tz)
{
  t12_matrix_(0, 3)  = tx;
  t12_matrix_(1, 3)  = ty;
  t12_matrix_(2, 3)  = tz;
  return *this;
}

template <class T>
vgl_h_matrix_3d<T>&
vgl_h_matrix_3d<T>::set_scale(T scale)
{
  for (unsigned r = 0; r<3; ++r)
    for (unsigned c = 0; c<4; ++c)
      t12_matrix_[r][c]*=scale;
  return *this;
}

template <class T>
vgl_h_matrix_3d<T>&
vgl_h_matrix_3d<T>::set_affine(vnl_matrix_fixed<T,3,4> const& M34)
{
  for (unsigned r = 0; r<3; ++r)
    for (unsigned c = 0; c<4; ++c)
      t12_matrix_[r][c] = M34[r][c];
  t12_matrix_[3][0] = t12_matrix_[3][1] = t12_matrix_[3][2] = T(0); t12_matrix_[3][3] = T(1);
  return *this;
}

template <class T>
vgl_h_matrix_3d<T>&
vgl_h_matrix_3d<T>::set_rotation_about_axis(vnl_vector_fixed<T,3> const& axis, T angle)
{
  vnl_quaternion<T> q(axis, angle);
  //get the transpose of the rotation matrix
  vnl_matrix_fixed<T,3,3> R = q.rotation_matrix_transpose();
  //fill in with the transpose
  for (int c = 0; c<3; c++)
    for (int r = 0; r<3; r++)
      t12_matrix_[r][c]=R[c][r];
  return *this;
}

template <class T>
vgl_h_matrix_3d<T>&
vgl_h_matrix_3d<T>::set_rotation_roll_pitch_yaw(T yaw, T pitch, T roll)
{
  typedef typename vnl_numeric_traits<T>::real_t real_t;
  real_t ax = yaw/2, ay = pitch/2, az = roll/2;

  vnl_quaternion<T> qx((T)std::sin(ax),0,0,(T)std::cos(ax));
  vnl_quaternion<T> qy(0,(T)std::sin(ay),0,(T)std::cos(ay));
  vnl_quaternion<T> qz(0,0,(T)std::sin(az),(T)std::cos(az));
  vnl_quaternion<T> q = qz*qy*qx;

  vnl_matrix_fixed<T,3,3> R = q.rotation_matrix_transpose();
  //fill in with the transpose
  for (int c = 0; c<3; c++)
    for (int r = 0; r<3; r++)
      t12_matrix_[r][c]=R[c][r];
  return *this;
}

template <class T>
vgl_h_matrix_3d<T>&
vgl_h_matrix_3d<T>::set_rotation_euler(T rz1, T ry, T rz2)
{
  typedef typename vnl_numeric_traits<T>::real_t real_t;
  real_t az1 = rz1/2, ay = ry/2, az2 = rz2/2;

  vnl_quaternion<T> qz1(0,0,T(std::sin(az1)),T(std::cos(az1)));
  vnl_quaternion<T> qy(0,T(std::sin(ay)),0,T(std::cos(ay)));
  vnl_quaternion<T> qz2(0,0,T(std::sin(az2)),T(std::cos(az2)));
  vnl_quaternion<T> q = qz2*qy*qz1;

  vnl_matrix_fixed<T,3,3> R = q.rotation_matrix_transpose();
  //fill in with the transpose
  for (int c = 0; c<3; c++)
    for (int r = 0; r<3; r++)
      t12_matrix_[r][c]=R[c][r];
  return *this;
}

template <class T>
vgl_h_matrix_3d<T>&
vgl_h_matrix_3d<T>::set_rotation_matrix(vnl_matrix_fixed<T,3,3> const& R)
{
  for (unsigned r = 0; r<3; ++r)
    for (unsigned c = 0; c<3; ++c)
      t12_matrix_[r][c] = R[r][c];
  return *this;
}


template <class T>
void
vgl_h_matrix_3d<T>::set_reflection_plane(vgl_plane_3d<double> const& l)
{
  t12_matrix_.fill(T(0));
  t12_matrix_(0,0) = T(l.nx()*l.nx());
  t12_matrix_(1,1) = T(l.ny()*l.ny());
  t12_matrix_(2,2) = T(l.nz()*l.nz());
  t12_matrix_(0,1) = t12_matrix_(1,0) = T(l.nx()*l.ny());
  t12_matrix_(0,2) = t12_matrix_(2,0) = T(l.nx()*l.nz());
  t12_matrix_(1,2) = t12_matrix_(2,1) = T(l.ny()*l.nz());
  t12_matrix_(0,3) = T(l.nx()*l.d());
  t12_matrix_(1,3) = T(l.ny()*l.d());
  t12_matrix_(2,3) = T(l.nz()*l.d());
  t12_matrix_ *= -2/(t12_matrix_(0,0)+t12_matrix_(1,1)+t12_matrix_(2,2));
  t12_matrix_(0,0) += (T)1;
  t12_matrix_(1,1) += (T)1;
  t12_matrix_(2,2) += (T)1;
  t12_matrix_(3,3) += (T)1;
}


template <class T>
bool vgl_h_matrix_3d<T>::is_rotation() const
{
  return t12_matrix_.get(0,3) == (T)0
      && t12_matrix_.get(1,3) == (T)0
      && t12_matrix_.get(2,3) == (T)0
      && this->is_euclidean();
}


template <class T>
bool vgl_h_matrix_3d<T>::is_euclidean() const
{
  T eps = 10*std::numeric_limits<T>::epsilon();
  if ( t12_matrix_.get(3,0) != (T)0 ||
       t12_matrix_.get(3,1) != (T)0 ||
       t12_matrix_.get(3,2) != (T)0 ||
       std::fabs(t12_matrix_.get(3,3)-T(1)) > eps)
    return false; // should not have a projective part

  // use an error tolerance on the orthonormality constraint
  vnl_matrix_fixed<T,3,3> R = get_upper_3x3_matrix();
  R *= R.transpose();
  R(0,0) -= T(1);
  R(1,1) -= T(1);
  R(2,2) -= T(1);
  return R.absolute_value_max() <= eps;
}

template <class T>
bool vgl_h_matrix_3d<T>::is_affine() const{
  if ( t12_matrix_.get(3,0) != (T)0 ||
       t12_matrix_.get(3,1) != (T)0 ||
       t12_matrix_.get(3,2) != (T)0 ||
       std::fabs(t12_matrix_.get(3,3)) > 10*std::numeric_limits<T>::epsilon())
    return false; // should not have a projective part
  return !(this->is_euclidean());
}

template <class T>
bool vgl_h_matrix_3d<T>::is_identity() const
{
  return t12_matrix_.is_identity();
}


template <class T>
vgl_h_matrix_3d<T>
vgl_h_matrix_3d<T>::get_upper_3x3() const
{
  //only sensible for affine transformations
  T d = t12_matrix_[3][3];
  assert(d<-1e-9 || d>1e-9);
  vnl_matrix_fixed<T,4,4> m(0.0);
  for (unsigned r = 0; r<3; r++)
    for (unsigned c = 0; c<3; c++)
      m[r][c] = t12_matrix_[r][c]/d;
  m[3][3]=1.0;
  return vgl_h_matrix_3d<T>(m);
}

template <class T>
vnl_matrix_fixed<T,3,3>
vgl_h_matrix_3d<T>::get_upper_3x3_matrix() const
{
  vnl_matrix_fixed<T,3,3> R;
  vgl_h_matrix_3d<T> m = this->get_upper_3x3();
  for (unsigned r = 0; r<3; r++)
    for (unsigned c = 0; c<3; c++)
      R[r][c] = m.get(r,c);
  return R;
}
template <class T>
void vgl_h_matrix_3d<T>::polar_decomposition(vnl_matrix_fixed<T, 3, 3>& S, vnl_matrix_fixed<T, 3, 3>& R) const{
  vnl_matrix_fixed<T, 3, 3> up = this->get_upper_3x3_matrix();
  vnl_matrix<T> M(up);
  vnl_svd<T> svd(M);
  vnl_matrix<T> U = svd.U();
  vnl_matrix<T> W = svd.W();
  vnl_matrix<T> V = svd.V();
  R = vnl_matrix_fixed<T, 3, 3> ( U*V.transpose());
  S = vnl_matrix_fixed<T, 3, 3> (V*W*V.transpose());
  return;
}

template <class T>
vgl_homg_point_3d<T>
vgl_h_matrix_3d<T>::get_translation() const
{
  //only sensible for affine transformations
  T d = t12_matrix_[3][3];
  assert(d<-1e-9 || d>1e-9);
  return vgl_homg_point_3d<T>(t12_matrix_[0][3]/d,
                              t12_matrix_[1][3]/d,
                              t12_matrix_[2][3]/d,
                              (T)1);
}

template <class T>
vnl_vector_fixed<T,3>
vgl_h_matrix_3d<T>::get_translation_vector() const
{
  vgl_homg_point_3d<T> p = this->get_translation();
  return vnl_vector_fixed<T,3>(p.x(), p.y(), p.z());
}

//----------------------------------------------------------------------------
#undef VGL_H_MATRIX_3D_INSTANTIATE
#define VGL_H_MATRIX_3D_INSTANTIATE(T) \
template class vgl_h_matrix_3d<T >; \
template std::ostream& operator<<(std::ostream&, vgl_h_matrix_3d<T > const& ); \
template std::istream& operator>>(std::istream&, vgl_h_matrix_3d<T >& )

#endif // vgl_h_matrix_3d_hxx_
