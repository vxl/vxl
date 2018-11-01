// This is core/vgl/algo/vgl_h_matrix_2d.hxx
#ifndef vgl_h_matrix_2d_hxx_
#define vgl_h_matrix_2d_hxx_

#include <limits>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "vgl_h_matrix_2d.h"
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/algo/vnl_svd.h>
#include <vcl_compiler_detection.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vcl_deprecated.h>

template <class T>
vgl_h_matrix_2d<T>::vgl_h_matrix_2d(std::istream& s)
{
  t12_matrix_.read_ascii(s);
}

template <class T>
vgl_h_matrix_2d<T>::vgl_h_matrix_2d(char const* filename)
{
  std::ifstream f(filename);
  if (!f.good())
    std::cerr << "vgl_h_matrix_2d::read: Error opening " << filename << std::endl;
  else
    t12_matrix_.read_ascii(f);
}

template <class T>
vgl_h_matrix_2d<T>::vgl_h_matrix_2d(std::vector<vgl_homg_point_2d<T> > const& points1,
                                    std::vector<vgl_homg_point_2d<T> > const& points2)
{
  vnl_matrix<T> W;
  assert(points1.size() == points2.size());
  unsigned int numpoints = points1.size();
  if (numpoints < 4)
  {
    std::cerr << "\nvhl_h_matrix_2d - minimum of 4 points required\n";
    std::exit(0);
  }

  W.set_size(2*numpoints, 9);

  for (unsigned int i = 0; i < numpoints; ++i)
  {
    T x1 = points1[i].x(), y1 = points1[i].y(), w1 = points1[i].w();
    T x2 = points2[i].x(), y2 = points2[i].y(), w2 = points2[i].w();

    W[i*2][0]=x1*w2;    W[i*2][1]=y1*w2;    W[i*2][2]=w1*w2;
    W[i*2][3]=0.0;      W[i*2][4]=0.0;      W[i*2][5]=0.0;
    W[i*2][6]=-x1*x2;   W[i*2][7]=-y1*x2;   W[i*2][8]=-w1*x2;

    W[i*2+1][0]=0.0;    W[i*2+1][1]=0.0;    W[i*2+1][2]=0.0;
    W[i*2+1][3]=x1*w2;  W[i*2+1][4]=y1*w2;  W[i*2+1][5]=w1*w2;
    W[i*2+1][6]=-x1*y2; W[i*2+1][7]=-y1*y2; W[i*2+1][8]=-w1*y2;
  }

  vnl_svd<T> SVD(W);
  t12_matrix_ = vnl_matrix_fixed<T,3,3>(SVD.nullvector().data_block()); // 9-dim. nullvector
}

template <class T>
vgl_h_matrix_2d<T>::vgl_h_matrix_2d(vnl_matrix_fixed<T,2,2> const& M,
                                    vnl_vector_fixed<T,2> const& m)
{
  for (int r = 0; r < 2; ++r) {
    for (int c = 0; c < 2; ++c)
      (t12_matrix_)(r, c) = M(r,c);
    (t12_matrix_)(r, 2) = m(r);
  }
  for (int c = 0; c < 2; ++c)
    (t12_matrix_)(2,c) = 0;
  (t12_matrix_)(2,2) = 1;
}


// == OPERATIONS ==

template <class T>
vgl_homg_point_2d<T>
vgl_h_matrix_2d<T>::operator()(vgl_homg_point_2d<T> const& p) const
{
  vnl_vector_fixed<T,3> v2 = t12_matrix_ * vnl_vector_fixed<T,3>(p.x(), p.y(), p.w());
  return vgl_homg_point_2d<T>(v2[0], v2[1], v2[2]);
}

template <class T>
vgl_homg_line_2d<T>
vgl_h_matrix_2d<T>::correlation(vgl_homg_point_2d<T> const& p) const
{
  vnl_vector_fixed<T,3> v2 = t12_matrix_ * vnl_vector_fixed<T,3>(p.x(), p.y(), p.w());
  return vgl_homg_line_2d<T>(v2[0], v2[1], v2[2]);
}

template <class T>
vgl_homg_line_2d<T>
vgl_h_matrix_2d<T>::preimage(vgl_homg_line_2d<T> const& l) const
{
  vnl_vector_fixed<T,3> v2 = t12_matrix_.transpose() * vnl_vector_fixed<T,3>(l.a(), l.b(), l.c());
  return vgl_homg_line_2d<T>(v2[0], v2[1], v2[2]);
}

template <class T>
vgl_homg_point_2d<T>
vgl_h_matrix_2d<T>::correlation(vgl_homg_line_2d<T> const& l) const
{
  vnl_vector_fixed<T,3> v2 = t12_matrix_ * vnl_vector_fixed<T,3>(l.a(), l.b(), l.c());
  return vgl_homg_point_2d<T>(v2[0], v2[1], v2[2]);
}

template <class T>
vgl_conic<T>
vgl_h_matrix_2d<T>::operator() (vgl_conic<T> const& C) const
{
  T a=C.a(), b=C.b()/2, c = C.c(), d = C.d()/2, e = C.e()/2, f = C.f();
  vnl_matrix_fixed<T,3,3> M, Mp;
  M(0,0) = a;  M(0,1) = b;  M(0,2) = d;
  M(1,0) = b;  M(1,1) = c;  M(1,2) = e;
  M(2,0) = d;  M(2,1) = e;  M(2,2) = f;
  Mp = (t12_matrix_.transpose())*M*t12_matrix_;
  return   vgl_conic<T>(Mp(0,0),(Mp(0,1)+Mp(1,0)),Mp(1,1),(Mp(0,2)+Mp(2,0)),
                        (Mp(1,2)+Mp(2,1)), Mp(2,2));
}

template <class T>
vgl_conic<T>
vgl_h_matrix_2d<T>::preimage(vgl_conic<T> const& C) const
{
  T a=C.a(), b=C.b()/2, c = C.c(), d = C.d()/2, e = C.e()/2, f = C.f();
  vnl_matrix_fixed<T,3,3> M, Mp;
  M(0,0) = a;  M(0,1) = b;  M(0,2) = d;
  M(1,0) = b;  M(1,1) = c;  M(1,2) = e;
  M(2,0) = d;  M(2,1) = e;  M(2,2) = f;
  Mp = vnl_inverse_transpose(t12_matrix_)*M*vnl_inverse(t12_matrix_);
  return   vgl_conic<T>(Mp(0,0),(Mp(0,1)+Mp(1,0)),Mp(1,1),(Mp(0,2)+Mp(2,0)),
                        (Mp(1,2)+Mp(2,1)), Mp(2,2));
}


template <class T>
vgl_homg_point_2d<T>
vgl_h_matrix_2d<T>::preimage(vgl_homg_point_2d<T> const& p) const
{
  vnl_vector_fixed<T,3> v = vnl_inverse(t12_matrix_) * vnl_vector_fixed<T,3>(p.x(), p.y(), p.w());
  return vgl_homg_point_2d<T>(v[0], v[1], v[2]);
}

template <class T>
vgl_homg_line_2d<T>
vgl_h_matrix_2d<T>::operator()(vgl_homg_line_2d<T> const& l) const
{
  vnl_vector_fixed<T,3> v = vnl_inverse_transpose(t12_matrix_) * vnl_vector_fixed<T,3>(l.a(), l.b(), l.c());
  return vgl_homg_line_2d<T>(v[0], v[1], v[2]);
}

template <class T>
std::ostream& operator<<(std::ostream& s, vgl_h_matrix_2d<T> const& h)
{
  return s << h.get_matrix();
}

template <class T>
bool vgl_h_matrix_2d<T>::read(std::istream& s)
{
  t12_matrix_.read_ascii(s);
  return s.good() || s.eof();
}

template <class T>
bool vgl_h_matrix_2d<T>::read(char const* filename)
{
  std::ifstream f(filename);
  if (!f.good())
    std::cerr << "vgl_h_matrix_2d::read: Error opening " << filename << std::endl;
  return read(f);
}

// == DATA ACCESS ==

template <class T>
T vgl_h_matrix_2d<T>::get(unsigned int row_index, unsigned int col_index) const
{
  return t12_matrix_. get(row_index, col_index);
}

template <class T>
void vgl_h_matrix_2d<T>::get(T* H) const
{
  for (T const* iter = t12_matrix_.begin(); iter < t12_matrix_.end(); ++iter)
    *H++ = *iter;
}

template <class T>
void vgl_h_matrix_2d<T>::get(vnl_matrix_fixed<T,3,3>* H) const
{
  *H = t12_matrix_;
}

template <class T>
void vgl_h_matrix_2d<T>::get(vnl_matrix<T>* H) const
{
  VXL_DEPRECATED_MACRO("vgl_h_matrix_2d<T>::get(vnl_matrix<T>*) const");
  *H = t12_matrix_.as_ref(); // size 3x3
}

template <class T>
vgl_h_matrix_2d<T>&
vgl_h_matrix_2d<T>::set_identity()
{
  t12_matrix_.set_identity();
  return *this;
}

template <class T>
vgl_h_matrix_2d<T>&
vgl_h_matrix_2d<T>::set(const T* H)
{
  for (T* iter = t12_matrix_.begin(); iter < t12_matrix_.end(); ++iter)
    *iter = *H++;
  return *this;
}

template <class T>
vgl_h_matrix_2d<T>&
vgl_h_matrix_2d<T>::set(vnl_matrix_fixed<T,3,3> const& H)
{
  t12_matrix_ = H;
  return *this;
}

//-------------------------------------------------------------------
template <class T>
bool vgl_h_matrix_2d<T>::
projective_basis(std::vector<vgl_homg_point_2d<T> > const& points)
{
  if (points.size()!=4)
    return false;
  vnl_vector_fixed<T,3> p0(points[0].x(), points[0].y(), points[0].w());
  vnl_vector_fixed<T,3> p1(points[1].x(), points[1].y(), points[1].w());
  vnl_vector_fixed<T,3> p2(points[2].x(), points[2].y(), points[2].w());
  vnl_vector_fixed<T,3> p3(points[3].x(), points[3].y(), points[3].w());
  vnl_matrix_fixed<T,3,4> point_matrix;
  point_matrix.set_column(0, p0);
  point_matrix.set_column(1, p1);
  point_matrix.set_column(2, p2);
  point_matrix.set_column(3, p3);

  if (! point_matrix.is_finite() || point_matrix.has_nans())
  {
    std::cerr << "vgl_h_matrix_2d<T>::projective_basis():\n"
             << " given points have infinite or NaN values\n";
    this->set_identity();
    return false;
  }
  vnl_svd<T> svd1(point_matrix.as_ref(), 1e-8); // size 3x4
  if (svd1.rank() < 3)
  {
    std::cerr << "vgl_h_matrix_2d<T>::projective_basis():\n"
             << " At least three out of the four points are nearly collinear\n";
    this->set_identity();
    return false;
  }

  vnl_matrix_fixed<T,3,3> back_matrix;
  back_matrix.set_column(0, p0);
  back_matrix.set_column(1, p1);
  back_matrix.set_column(2, p2);

  vnl_vector_fixed<T,3> scales_vector = vnl_inverse(back_matrix) * p3;

  back_matrix.set_column(0, scales_vector[0] * p0);
  back_matrix.set_column(1, scales_vector[1] * p1);
  back_matrix.set_column(2, scales_vector[2] * p2);

  if (! back_matrix.is_finite() || back_matrix.has_nans())
  {
    std::cerr << "vgl_h_matrix_2d<T>::projective_basis():\n"
             << " back matrix has infinite or NaN values\n";
    this->set_identity();
    return false;
  }
  this->set(vnl_inverse(back_matrix));
  return true;
}

template <class T>
bool vgl_h_matrix_2d<T>::is_rotation() const
{
  return t12_matrix_.get(0,2) == (T)0
      && t12_matrix_.get(1,2) == (T)0
      && this->is_euclidean();
}

template <class T>
bool vgl_h_matrix_2d<T>::is_euclidean() const
{
  if ( t12_matrix_.get(2,0) != (T)0 ||
       t12_matrix_.get(2,1) != (T)0 ||
       t12_matrix_.get(2,2) != (T)1 )
    return false; // should not have a translation part

  // use an error tolerance on the orthonormality constraint
  vnl_matrix_fixed<T,2,2> R = get_upper_2x2_matrix();
  R *= R.transpose();
  R(0,0) -= T(1);
  R(1,1) -= T(1);
  return R.absolute_value_max() <= 10*std::numeric_limits<T>::epsilon();
}

template <class T>
bool vgl_h_matrix_2d<T>::is_identity() const
{
  return t12_matrix_.is_identity();
}

//-------------------------------------------------------------------
template <class T>
bool vgl_h_matrix_2d<T>::projective_basis(std::vector<vgl_homg_line_2d<T> > const& lines)
{
  if (lines.size()!=4)
    return false;
  vnl_vector_fixed<T,3> l0(lines[0].a(), lines[0].b(), lines[0].c());
  vnl_vector_fixed<T,3> l1(lines[1].a(), lines[1].b(), lines[1].c());
  vnl_vector_fixed<T,3> l2(lines[2].a(), lines[2].b(), lines[2].c());
  vnl_vector_fixed<T,3> l3(lines[3].a(), lines[3].b(), lines[3].c());
  vnl_matrix_fixed<T,3,4> line_matrix;
  line_matrix.set_column(0, l0);
  line_matrix.set_column(1, l1);
  line_matrix.set_column(2, l2);
  line_matrix.set_column(3, l3);

  if (! line_matrix.is_finite() || line_matrix.has_nans())
  {
    std::cerr << "vgl_h_matrix_2d<T>::projective_basis():\n"
             << " given lines have infinite or NaN values\n";
    this->set_identity();
    return false;
  }
  vnl_svd<T> svd1(line_matrix.as_ref(), 1e-8); // size 3x4
  if (svd1.rank() < 3)
  {
    std::cerr << "vgl_h_matrix_2d<T>::projective_basis():\n"
             << " At least three out of the four lines are nearly concurrent\n";
    this->set_identity();
    return false;
  }

  vnl_matrix_fixed<T,3,3> back_matrix;
  back_matrix.set_column(0, l0);
  back_matrix.set_column(1, l1);
  back_matrix.set_column(2, l2);
  vnl_vector_fixed<T,3> scales_vector = vnl_inverse(back_matrix) * l3;
  back_matrix.set_row(0, scales_vector[0] * l0);
  back_matrix.set_row(1, scales_vector[1] * l1);
  back_matrix.set_row(2, scales_vector[2] * l2);

  if (! back_matrix.is_finite() || back_matrix.has_nans())
  {
    std::cerr << "vgl_h_matrix_2d<T>::projective_basis():\n"
             << " back matrix has infinite or NaN values\n";
    this->set_identity();
    return false;
  }
  this->set(back_matrix);
  return true;
}

template <class T>
vgl_h_matrix_2d<T>
vgl_h_matrix_2d<T>::get_inverse() const
{
  return vgl_h_matrix_2d<T>(vnl_inverse(t12_matrix_));
}


template <class T>
vgl_h_matrix_2d<T>&
vgl_h_matrix_2d<T>::set_translation(T tx, T ty)
{
  t12_matrix_[0][2] = tx;   t12_matrix_[1][2] = ty;
  return *this;
}

template <class T>
vgl_h_matrix_2d<T>&
vgl_h_matrix_2d<T>::set_rotation(T theta)
{
  double theta_d = (double)theta;
  double c = std::cos(theta_d), s = std::sin(theta_d);
  t12_matrix_[0][0] = (T)c;   t12_matrix_[0][1] = -(T)s;
  t12_matrix_[1][0] = (T)s;   t12_matrix_[1][1] = (T)c;
  return *this;
}

template <class T>
vgl_h_matrix_2d<T>&
vgl_h_matrix_2d<T>::set_scale(T scale)
{
  for (unsigned r = 0; r<2; ++r)
    for (unsigned c = 0; c<3; ++c)
      t12_matrix_[r][c]*=scale;
  return *this;
}

template <class T>
vgl_h_matrix_2d<T>&
vgl_h_matrix_2d<T>::set_similarity(T s, T theta,
                                   T tx, T ty)
{
  T a=s*std::cos(theta);
  T b=s*std::sin(theta);
  t12_matrix_[0][0] = a; t12_matrix_[0][1] = -b; t12_matrix_[0][2] = tx;
  t12_matrix_[1][0] = b; t12_matrix_[1][1] =  a; t12_matrix_[1][2] = ty;
  t12_matrix_[2][0]=T(0); t12_matrix_[2][1]=T(0); t12_matrix_[2][2] = T(1);
  return *this;
}

template <class T>
vgl_h_matrix_2d<T>&
vgl_h_matrix_2d<T>::set_aspect_ratio(T aspect_ratio)
{
  for (unsigned c = 0; c<3; ++c)
    t12_matrix_[1][c]*=aspect_ratio;
  return *this;
}


template <class T>
vgl_h_matrix_2d<T>&
vgl_h_matrix_2d<T>::set_affine(vnl_matrix_fixed<T,2,3> const& M23)
{
  for (unsigned r = 0; r<2; ++r)
    for (unsigned c = 0; c<3; ++c)
      t12_matrix_[r][c] = M23[r][c];
  t12_matrix_[2][0] = T(0); t12_matrix_[2][1] = T(0);  t12_matrix_[2][2] = T(1);
  return *this;
}

template <class T>
vgl_h_matrix_2d<T>&
vgl_h_matrix_2d<T>::set_affine(vnl_matrix<T> const& M23)
{
  VXL_DEPRECATED_MACRO("vgl_h_matrix_2d<T>::set_affine(vnl_matrix<T> const&)");
  assert (M23.rows()==2 && M23.columns()==3);
  for (unsigned r = 0; r<2; ++r)
    for (unsigned c = 0; c<3; ++c)
      t12_matrix_[r][c] = M23[r][c];
  t12_matrix_[2][0] = T(0); t12_matrix_[2][1] = T(0);  t12_matrix_[2][2] = T(1);
  return *this;
}

template <class T>
vgl_h_matrix_2d<T>
vgl_h_matrix_2d<T>::get_upper_2x2() const
{
  //only sensible for affine transformations
  T d = t12_matrix_[2][2];
  assert(d<-1e-9 || d>1e-9);
  vnl_matrix_fixed<T,3,3> m(0.0);
  for (unsigned r = 0; r<2; ++r)
    for (unsigned c = 0; c<2; ++c)
      m[r][c] = t12_matrix_[r][c]/d;
  m[2][2]=1.0;
  return vgl_h_matrix_2d<T>(m);
}

template <class T>
vnl_matrix_fixed<T,2,2>
vgl_h_matrix_2d<T>::get_upper_2x2_matrix() const
{
  vnl_matrix_fixed<T,2,2> R;
  vgl_h_matrix_2d<T> m = this->get_upper_2x2();
  for (unsigned r = 0; r<3; ++r)
    for (unsigned c = 0; c<3; ++c)
      R[r][c] = m.get(r,c);
  return R;
}

template <class T>
vgl_homg_point_2d<T>
vgl_h_matrix_2d<T>::get_translation() const
{
  //only sensible for affine transformations
  T d = t12_matrix_[2][2];
  assert(d<-1e-9 || d>1e-9);
  return vgl_homg_point_2d<T>(t12_matrix_[0][2]/d,
                              t12_matrix_[1][2]/d,
                              (T)1);
}

template <class T>
vnl_vector_fixed<T,2>
vgl_h_matrix_2d<T>::get_translation_vector() const
{
  vgl_homg_point_2d<T> p = this->get_translation();
  return vnl_vector_fixed<T,2>(p.x(), p.y());
}

//----------------------------------------------------------------------------
#undef VGL_H_MATRIX_2D_INSTANTIATE
#define VGL_H_MATRIX_2D_INSTANTIATE(T) \
template class vgl_h_matrix_2d<T >; \
template std::ostream& operator << (std::ostream& s, vgl_h_matrix_2d<T > const& h); \
template std::istream& operator >> (std::istream& s, vgl_h_matrix_2d<T >& h)

#endif // vgl_h_matrix_2d_hxx_
