// This is core/vgl/algo/vgl_h_matrix_2d.txx
#ifndef vgl_h_matrix_2d_txx_
#define vgl_h_matrix_2d_txx_
//:
// \file

#include "vgl_h_matrix_2d.h"
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/algo/vnl_svd.h>
#include <vcl_limits.h>
#include <vcl_cstdlib.h> // for exit()
#include <vcl_fstream.h>
#include <vcl_cassert.h>

//: Copy constructor
template <class T>
vgl_h_matrix_2d<T>::vgl_h_matrix_2d(const vgl_h_matrix_2d<T>& M)
  : t12_matrix_(M.get_matrix())
{
}


//: Constructor from vcl_istream
template <class T>
vgl_h_matrix_2d<T>::vgl_h_matrix_2d(vcl_istream& s)
{
  t12_matrix_.read_ascii(s);
}

//: Constructor from file
template <class T>
vgl_h_matrix_2d<T>::vgl_h_matrix_2d(char const* filename)
{
  vcl_ifstream f(filename);
  if (!f.good())
    vcl_cerr << "vgl_h_matrix_2d::read: Error opening " << filename << vcl_endl;
  else
    t12_matrix_.read_ascii(f);
}

//--------------------------------------------------------------
//: Constructor, and implicit cast from vnl_matrix_fixed<T,3,3>
template <class T>
vgl_h_matrix_2d<T>::vgl_h_matrix_2d(vnl_matrix_fixed<T,3,3> const& M):
  t12_matrix_(M)
{
}

//--------------------------------------------------------------
//: Constructor
template <class T>
vgl_h_matrix_2d<T>::vgl_h_matrix_2d(const T* H)
  : t12_matrix_(H)
{
}

//--------------------------------------------------------------
//
//: Constructor - calculate homography between two sets of 2D points (minimum 4)
template <class T>
vgl_h_matrix_2d<T>::vgl_h_matrix_2d(vcl_vector<vgl_homg_point_2d<T> > const& points1,
                                    vcl_vector<vgl_homg_point_2d<T> > const& points2)
{
  vnl_matrix<T> W;
  assert(points1.size() == points2.size());
  unsigned int numpoints = points1.size();
  if (numpoints < 4)
  {
    vcl_cerr << "\nvhl_h_matrix_2d - minimum of 4 points required\n";
    vcl_exit(0);
  }

  W.set_size(2*numpoints, 9);

  for (unsigned int i = 0; i < numpoints; i++)
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

//--------------------------------------------------------------
//
//: Construct an affine vgl_h_matrix_2d from 2x2 M and 2x1 m.
//
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
  vnl_vector_fixed<T, 3> v(p.x(), p.y(), p.w());
  vnl_vector_fixed<T,3> v2 = t12_matrix_ * v;
  return vgl_homg_point_2d<T>(v2[0], v2[1], v2[2]);
}

template <class T>
vgl_homg_line_2d<T>
vgl_h_matrix_2d<T>::preimage(vgl_homg_line_2d<T> const& l) const
{
  vnl_vector_fixed<T,3> v(l.a(), l.b(), l.c());
  vnl_vector_fixed<T,3> v2 = t12_matrix_.transpose() * v;
  return vgl_homg_line_2d<T>(v2[0], v2[1], v2[2]);
}

template <class T>
vgl_homg_line_2d<T>
vgl_h_matrix_2d<T>::correlation(vgl_homg_point_2d<T> const& p) const
{
  vnl_vector_fixed<T, 3> v(p.x(), p.y(), p.w());
  vnl_vector_fixed<T,3> v2 = t12_matrix_ * v;
  return vgl_homg_line_2d<T>(v2[0], v2[1], v2[2]);
}

template <class T>
vgl_homg_point_2d<T>
vgl_h_matrix_2d<T>::correlation(vgl_homg_line_2d<T> const& l) const
{
  vnl_vector_fixed<T,3> v(l.a(), l.b(), l.c());
  vnl_vector_fixed<T,3> v2 = t12_matrix_ * v;
  return vgl_homg_point_2d<T>(v2[0], v2[1], v2[2]);
}

template <class T>
vgl_conic<T> vgl_h_matrix_2d<T>::operator() (vgl_conic<T> const& C) const
{
  T a=C.a(), b=C.b()/2, c = C.c(), d = C.d()/2, e = C.e()/2, f = C.f();
  vnl_matrix_fixed<T, 3, 3> M, Mp;
  M(0,0) = a;  M(0,1) = b;  M(0,2) = d;
  M(1,0) = b;  M(1,1) = c;  M(1,2) = e;
  M(2,0) = d;  M(2,1) = e;  M(2,2) = f;
  Mp = (t12_matrix_.transpose())*M*t12_matrix_;
  return   vgl_conic<T>(Mp(0,0),(Mp(0,1)+Mp(1,0)),Mp(1,1),(Mp(0,2)+Mp(2,0)),
                        (Mp(1,2)+Mp(2,1)), Mp(2,2));
}

template <class T>
vgl_conic<T> vgl_h_matrix_2d<T>::preimage(vgl_conic<T> const& C) const
{
  T a=C.a(), b=C.b()/2, c = C.c(), d = C.d()/2, e = C.e()/2, f = C.f();
  vnl_matrix_fixed<T, 3, 3> M, Mp;
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
  vnl_vector_fixed<T,3> v(p.x(), p.y(), p.w());
  v = vnl_inverse(t12_matrix_) * v;
  return vgl_homg_point_2d<T>(v[0], v[1], v[2]);
}

template <class T>
vgl_homg_line_2d<T>
vgl_h_matrix_2d<T>::operator()(vgl_homg_line_2d<T> const& l) const
{
  vnl_vector_fixed<T,3> v(l.a(), l.b(), l.c());
  v = vnl_inverse_transpose(t12_matrix_) * v;
  return vgl_homg_line_2d<T>(v[0], v[1], v[2]);
}

//-----------------------------------------------------------------------------
//: Print H on vcl_ostream
template <class T>
vcl_ostream& operator<<(vcl_ostream& s, const vgl_h_matrix_2d<T>& h)
{
  return s << h.get_matrix();
}

//: Read H from vcl_istream
template <class T>
vcl_istream& operator >> (vcl_istream& s, vgl_h_matrix_2d<T>& H)
{
  H.read(s);
  return s;
}

//: Read H from vcl_istream
template <class T>
bool vgl_h_matrix_2d<T>::read(vcl_istream& s)
{
  t12_matrix_.read_ascii(s);
  return s.good() || s.eof();
}

//: Read H from file
template <class T>
bool vgl_h_matrix_2d<T>::read(char const* filename)
{
  vcl_ifstream f(filename);
  if (!f.good())
    vcl_cerr << "vgl_h_matrix_2d::read: Error opening " << filename << vcl_endl;
  return read(f);
}

// == DATA ACCESS ==

//-----------------------------------------------------------------------------
//: Get matrix element at (row_index, col_index)
template <class T>
T vgl_h_matrix_2d<T>::get(unsigned int row_index, unsigned int col_index) const
{
  return t12_matrix_. get(row_index, col_index);
}

//: Fill H with contents of this
template <class T>
void vgl_h_matrix_2d<T>::get(T* H) const
{
  T const* data = t12_matrix_.data_block();
  for (int index = 0; index < 9; ++index)
    *H++ = data[index];
}

//: Fill H with contents of this
template <class T>
void vgl_h_matrix_2d<T>::get(vnl_matrix_fixed<T,3,3>* H) const
{
  *H = t12_matrix_;
}

//: Fill H with contents of this
template <class T>
void vgl_h_matrix_2d<T>::get(vnl_matrix<T>* H) const
{
  *H = t12_matrix_.as_ref(); // size 3x3
}

//: Set to identity
template <class T>
void vgl_h_matrix_2d<T>::set_identity()
{
  t12_matrix_.set_identity();
}

//: Set to 3x3 row-stored matrix
template <class T>
void vgl_h_matrix_2d<T>::set(const T* H)
{
  T* data = t12_matrix_.data_block();
  for (int index = 0; index < 9; ++index)
    data[index] = *H++;
}

//: Set to given vnl_matrix
template <class T>
void vgl_h_matrix_2d<T>::set(vnl_matrix_fixed<T,3,3> const& H)
{
  t12_matrix_ = H;
}

//-------------------------------------------------------------------
template <class T>
bool vgl_h_matrix_2d<T>::
projective_basis(vcl_vector<vgl_homg_point_2d<T> > const& points)
{
  if (points.size()!=4)
    return false;
  vnl_vector_fixed<T, 3> p0(points[0].x(), points[0].y(), points[0].w());
  vnl_vector_fixed<T, 3> p1(points[1].x(), points[1].y(), points[1].w());
  vnl_vector_fixed<T, 3> p2(points[2].x(), points[2].y(), points[2].w());
  vnl_vector_fixed<T, 3> p3(points[3].x(), points[3].y(), points[3].w());
  vnl_matrix_fixed<T, 3, 4> point_matrix;
  point_matrix.set_column(0, p0);
  point_matrix.set_column(1, p1);
  point_matrix.set_column(2, p2);
  point_matrix.set_column(3, p3);

  if (! point_matrix.is_finite() || point_matrix.has_nans())
  {
    vcl_cerr << "vgl_h_matrix_2d<T>::projective_basis():\n"
             << " given points have infinite or NaN values\n";
    this->set_identity();
    return false;
  }
  vnl_svd<T> svd1(point_matrix.as_ref(), 1e-8); // size 3x4
  if (svd1.rank() < 3)
  {
    vcl_cerr << "vgl_h_matrix_2d<T>::projective_basis():\n"
             << " At least three out of the four points are nearly collinear\n";
    this->set_identity();
    return false;
  }

  vnl_matrix_fixed<T, 3, 3> back_matrix;
  back_matrix.set_column(0, p0);
  back_matrix.set_column(1, p1);
  back_matrix.set_column(2, p2);

  vnl_vector_fixed<T, 3> scales_vector = vnl_inverse(back_matrix) * p3;

  back_matrix.set_column(0, scales_vector[0] * p0);
  back_matrix.set_column(1, scales_vector[1] * p1);
  back_matrix.set_column(2, scales_vector[2] * p2);

  if (! back_matrix.is_finite() || back_matrix.has_nans())
  {
    vcl_cerr << "vgl_h_matrix_2d<T>::projective_basis():\n"
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
       t12_matrix_.get(2,3) != (T)1 )
    return false; // should not have a translation part

  // use an error tolerance on the orthonormality constraint
  vnl_matrix_fixed<T, 2,2> R = get_upper_2x2_matrix();
  R *= R.transpose();
  R(0,0) -= T(1);
  R(1,1) -= T(1);
  return R.absolute_value_max() <= 10*vcl_numeric_limits<T>::epsilon();
}

template <class T>
bool vgl_h_matrix_2d<T>::is_identity() const
{
  return t12_matrix_.is_identity();
      
}

//-------------------------------------------------------------------
template <class T>
bool vgl_h_matrix_2d<T>::
projective_basis(vcl_vector<vgl_homg_line_2d<T> > const& lines
#ifdef VCL_VC_6
                ,int dummy // parameter to help useless compiler disambiguate different functions
#endif
                )
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
    vcl_cerr << "vgl_h_matrix_2d<T>::projective_basis():\n"
             << " given lines have infinite or NaN values\n";
    this->set_identity();
    return false;
  }
  vnl_svd<T> svd1(line_matrix.as_ref(), 1e-8); // size 3x4
  if (svd1.rank() < 3)
  {
    vcl_cerr << "vgl_h_matrix_2d<T>::projective_basis():\n"
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
    vcl_cerr << "vgl_h_matrix_2d<T>::projective_basis():\n"
             << " back matrix has infinite or NaN values\n";
    this->set_identity();
    return false;
  }
  this->set(back_matrix);
  return true;
}

//: Return the inverse
template <class T>
vgl_h_matrix_2d<T> vgl_h_matrix_2d<T>::get_inverse() const
{
  return vgl_h_matrix_2d<T>(vnl_inverse(t12_matrix_));
}


//: Set the (0,2) and (1,2) elements of the transform matrix
template <class T>
void vgl_h_matrix_2d<T>::set_translation(const T tx, const T ty)
{
  t12_matrix_[0][2] = tx;   t12_matrix_[1][2] = ty;
}

//: Set the upper 2x2 submatrix to a rotation matrix defined by theta
template <class T>
void vgl_h_matrix_2d<T>::set_rotation(const T theta)
{
  double theta_d = (double)theta;
  double c = vcl_cos(theta_d), s = vcl_sin(theta_d);
  t12_matrix_[0][0] = (T)c;   t12_matrix_[0][1] = -(T)s;
  t12_matrix_[1][0] = (T)s;   t12_matrix_[1][1] = (T)c;
}

//: Compose the existing matrix with a uniform scale transformation
// \verbatim
//        _     _
//       |s  0  0|
//  Hs = |0  s  0| Hinitial
//       |0  0  1|
//        -     -
// \endverbatim
template <class T>
void vgl_h_matrix_2d<T>::set_scale(const T scale)
{
  for (unsigned r = 0; r<2; ++r)
    for (unsigned c = 0; c<3; ++c)
      t12_matrix_[r][c]*=scale;
}
//: set the transform to a similarity mapping
//        _                         _
//       |sCos(theta) -sSin(theta) tx|
// Sim = |sSin(theta)  sCos(theta) ty|
//       |   0            0        1 |
//        -                         -
template <class T>
void vgl_h_matrix_2d<T>::set_similarity(const T s, const T theta,
                                        const T tx, const T ty)
{
  T a=s*vcl_cos(theta);
  T b=s*vcl_sin(theta);
  t12_matrix_[0][0] = a; t12_matrix_[0][1] = -b; t12_matrix_[0][2] = tx;
  t12_matrix_[1][0] = b; t12_matrix_[1][1] =  a; t12_matrix_[1][2] = ty;
  t12_matrix_[2][0]=T(0); t12_matrix_[2][1]=T(0); t12_matrix_[2][2] = T(1);
}

//: Compose the existing matrix with an aspect ratio transform
// \verbatim
//        _     _
//       |1  0  0|
//  Hs = |0  r  0| Hinitial
//       |0  0  1|
//        -     -
// \endverbatim
template <class T>
void vgl_h_matrix_2d<T>::set_aspect_ratio(const T aspect_ratio)
{
  for (unsigned c = 0; c<3; ++c)
    t12_matrix_[1][c]*=aspect_ratio;
}


template <class T>
void vgl_h_matrix_2d<T>::set_affine(const vnl_matrix<T>& M23)
{
  for(unsigned r = 0; r<2; ++r)
    for(unsigned c = 0; c<3; ++c)
      t12_matrix_[r][c] = M23[r][c];
  t12_matrix_[2][0] = T(0); t12_matrix_[2][1] = T(0);  t12_matrix_[2][2] = T(1);
}

template <class T>
vgl_h_matrix_2d<T> 
vgl_h_matrix_2d<T>::get_upper_2x2() const
{
  //only sensible for affine transformations
  T d = t12_matrix_[2][2];
  assert(d<-1e-9 || d>1e-9);
  vnl_matrix_fixed<T,3,3> m(0.0);
  for (unsigned r = 0; r<2; r++)
    for (unsigned c = 0; c<2; c++)
      m[r][c] = t12_matrix_[r][c]/d;
  m[2][2]=1.0;
  return vgl_h_matrix_2d<T>(m);
}

template <class T>
vnl_matrix_fixed<T, 2,2> vgl_h_matrix_2d<T>::get_upper_2x2_matrix() const
{
  vnl_matrix_fixed<T,2,2> R;
  vgl_h_matrix_2d<T> m = this->get_upper_2x2();
  for (unsigned r = 0; r<3; r++)
    for (unsigned c = 0; c<3; c++)
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
vnl_vector_fixed<T, 2>
vgl_h_matrix_2d<T>::get_translation_vector() const
{
  vgl_homg_point_2d<T> p = this->get_translation();
  return vnl_vector_fixed<T,2>(p.x(), p.y());
}

//----------------------------------------------------------------------------
#undef VGL_H_MATRIX_2D_INSTANTIATE
#define VGL_H_MATRIX_2D_INSTANTIATE(T) \
template class vgl_h_matrix_2d<T >; \
template vcl_ostream& operator << (vcl_ostream& s, const vgl_h_matrix_2d<T >& h); \
template vcl_istream& operator >> (vcl_istream& s, vgl_h_matrix_2d<T >& h)

#endif // vgl_h_matrix_2d_txx_
