// This is core/vgl/algo/vgl_p_matrix.txx
#ifndef vgl_p_matrix_txx_
#define vgl_p_matrix_txx_
//:
// \file

#include "vgl_p_matrix.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_inverse.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_qr.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_point_3d.h>

//--------------------------------------------------------------
//
template <class T>
vgl_p_matrix<T>::vgl_p_matrix() :
  svd_(0)
{
  for (int row_index = 0; row_index < 3; row_index++)
    for (int col_index = 0; col_index < 4; col_index++)
      if (row_index == col_index)
        p_matrix_. put (row_index, col_index, 1);
      else
        p_matrix_. put (row_index, col_index, 0);
}

//--------------------------------------------------------------
//
template <class T>
vgl_p_matrix<T>::vgl_p_matrix(vcl_istream& i) :
  svd_(0)
{
  read_ascii(i);
}

//--------------------------------------------------------------
//
template <class T>
vgl_p_matrix<T>::vgl_p_matrix(vnl_matrix_fixed<T, 3, 4> const& pmatrix) :
  p_matrix_(pmatrix), svd_(0)
{
}

//--------------------------------------------------------------
//
template <class T>
vgl_p_matrix<T>::vgl_p_matrix(const vnl_matrix<T>& A, const vnl_vector<T>& a) :
  svd_(0)
{
  set(A,a);
}

//--------------------------------------------------------------
//
template <class T>
vgl_p_matrix<T>::vgl_p_matrix(const T *c_matrix) :
  p_matrix_(c_matrix), svd_(0)
{
}

//--------------------------------------------------------------
//
// - Copy ctor
template <class T>
vgl_p_matrix<T>::vgl_p_matrix(const vgl_p_matrix& that) :
  p_matrix_(that.get_matrix()), svd_(0)
{
}

// - Assignment
template <class T>
vgl_p_matrix<T>& vgl_p_matrix<T>::operator=(const vgl_p_matrix& that)
{
  p_matrix_ = that.get_matrix();
  svd_ = 0;
  return *this;
}

//--------------------------------------------------------------
//
// - Destructor
template <class T>
vgl_p_matrix<T>::~vgl_p_matrix()
{
  delete svd_; svd_ = 0;
}

// OPERATIONS


//-----------------------------------------------------------------------------
//
template <class T>
vgl_homg_line_2d<T> vgl_p_matrix<T>::operator () (const vgl_homg_line_3d_2_points<T>& L) const
{
  return vgl_homg_line_2d<T>((*this)(L.point_finite()), (*this)(L.point_infinite()));
}

//-----------------------------------------------------------------------------
//
template <class T>
vgl_line_segment_2d<T> vgl_p_matrix<T>::operator ()(vgl_line_segment_3d<T> const& L) const
{
  vgl_point_3d<T> p1 = L.point1(), p2 = L.point2();
  vgl_homg_point_3d<T> q1 (p1.x(),p1.y(),p1.z()), q2 (p2.x(),p2.y(),p2.z());
  return vgl_line_segment_2d<T>((*this)(q1), (*this)(q2));
}

//-----------------------------------------------------------------------------
//
template <class T>
vgl_homg_point_3d<T> vgl_p_matrix<T>::backproject_pseudoinverse (const vgl_homg_point_2d<T>& x) const
{
  //compute with double precision but cast back to type T
  vnl_vector_fixed<double,4> p = svd()->solve(vnl_vector_fixed<double,3>(x.x(),x.y(),x.w()));
  return vgl_homg_point_3d<T>(p[0],p[1],p[2],p[3]);
}

//-----------------------------------------------------------------------------
//
template <class T>
vgl_homg_line_3d_2_points<T> vgl_p_matrix<T>::backproject (const vgl_homg_point_2d<T>& x) const
{
  return vgl_homg_line_3d_2_points<T>(get_focal(), backproject_pseudoinverse(x));
}

//-----------------------------------------------------------------------------
//
template <class T>
vgl_homg_plane_3d<T> vgl_p_matrix<T>::backproject (const vgl_homg_line_2d<T>& l) const
{
  return p_matrix_.transpose() * l;
}

//-----------------------------------------------------------------------------
template <class T>
vcl_ostream& operator<<(vcl_ostream& s, const vgl_p_matrix<T>& p)
{
  return s << p.get_matrix();
}

//-----------------------------------------------------------------------------
template <class T>
vcl_istream& operator>>(vcl_istream& i, vgl_p_matrix<T>& p)
{
  p.read_ascii(i);
  return i;
}

static bool ok(vcl_istream& f) { return f.good() || f.eof(); }

template <class T>
bool vgl_p_matrix<T>::read_ascii(vcl_istream& f)
{
  vnl_matrix_ref<T> ref = this->p_matrix_;
  f >> ref;
  clear_svd();

  if (!ok(f)) {
    vcl_cerr << "vgl_p_matrix::read_ascii: Failed to load P matrix\n";
    return false;
  }

  return true;
}

template <class T>
vgl_p_matrix<T> vgl_p_matrix<T>::read(const char* filename)
{
  vcl_ifstream f(filename);
  if (!ok(f)) {
    vcl_cerr << "vgl_p_matrix::read: Failed to open P matrix file " << filename << vcl_endl;
    return vgl_p_matrix<T>();
  }

  vgl_p_matrix<T> P;
  if (!P.read_ascii(f))
    vcl_cerr << "vgl_p_matrix::read: Failed to read P matrix file " << filename << vcl_endl;

  return P;
}

template <class T>
vgl_p_matrix<T> vgl_p_matrix<T>::read(vcl_istream& s)
{
  vgl_p_matrix<T> P;
  s >> P;
  return P;
}

// COMPUTATIONS

//-----------------------------------------------------------------------------
//
template <class T>
vnl_svd<T>* vgl_p_matrix<T>::svd() const
{
  if (svd_ == 0) {
    // Need to make svd_ volatile for SGI g++ 2.7.2 optimizer bug.
    svd_ = new vnl_svd<T>(p_matrix_); // mutable const
  }
  return svd_;
}

template <class T>
void vgl_p_matrix<T>::clear_svd() const
{
  delete svd_; svd_ = 0;
}

//-----------------------------------------------------------------------------
//
template <class T>
vgl_homg_point_3d<T> vgl_p_matrix<T>::get_focal() const
{
  if (svd()->singularities() > 1) {
    vcl_cerr << "vgl_p_matrix::get_focal:\n"
             << "  Nullspace dimension is " << svd()->singularities()
             << "\n  Returning an invalid point (a vector of zeros)\n";
    return vgl_homg_point_3d<T>(0,0,0,0);
  }

  vnl_matrix<double> ns = svd()->nullspace();

  return vgl_homg_point_3d<T>(ns(0,0), ns(1,0), ns(2,0), ns(3,0));
}

template <class T>
vgl_h_matrix_3d<T> vgl_p_matrix<T>::get_canonical_H() const
{
  vnl_matrix_fixed<T,3,3> A;
  vnl_vector_fixed<T,3> a;
  this->get(&A, &a);
  return vgl_h_matrix_3d<T>(vnl_inverse(A), -vnl_inverse(A)*a);
}

template <class T>
bool vgl_p_matrix<T>::is_canonical(T tol) const
{
  for (int r = 0; r < 3; ++r)
    for (int c = 0; c < 4; ++c) {
      double d = r==c ? p_matrix_(r,c)-1 : p_matrix_(r,c);
      if (vcl_fabs(d) > tol)
        return false;
    }
  return true;
}

template <class T>
vgl_p_matrix<T> operator*(const vgl_p_matrix<T>& P, const vgl_h_matrix_3d<T>& H)
{
  vnl_matrix_fixed<T, 3, 4> M = P.get_matrix()*H.get_matrix();
  return vgl_p_matrix<T>(M);
}

// DATA ACCESS

//-----------------------------------------------------------------------------
//
template <class T>
T vgl_p_matrix<T>::get (unsigned int row_index, unsigned int col_index) const
{
  return p_matrix_. get (row_index, col_index);
}

//-----------------------------------------------------------------------------
//
template <class T>
void vgl_p_matrix<T>::get (T* c_matrix) const
{
  for (int row_index = 0; row_index < 3; row_index++)
    for (int col_index = 0; col_index < 4; col_index++)
      *c_matrix++ = p_matrix_. get (row_index, col_index);
}

//----------------------------------------------------------------
//
template <class T>
void
vgl_p_matrix<T>::get(vnl_matrix<T>* A, vnl_vector<T>* a) const
{
  A->put(0,0, p_matrix_(0,0));
  A->put(1,0, p_matrix_(1,0));
  A->put(2,0, p_matrix_(2,0));

  A->put(0,1, p_matrix_(0,1));
  A->put(1,1, p_matrix_(1,1));
  A->put(2,1, p_matrix_(2,1));

  A->put(0,2, p_matrix_(0,2));
  A->put(1,2, p_matrix_(1,2));
  A->put(2,2, p_matrix_(2,2));

  a->put(0, p_matrix_(0,3));
  a->put(1, p_matrix_(1,3));
  a->put(2, p_matrix_(2,3));
}

//----------------------------------------------------------------
//
template <class T>
void
vgl_p_matrix<T>::get (vnl_matrix_fixed<T,3,3>* A, vnl_vector_fixed<T,3>* a) const
{
  A->put(0,0, p_matrix_(0,0));
  A->put(1,0, p_matrix_(1,0));
  A->put(2,0, p_matrix_(2,0));

  A->put(0,1, p_matrix_(0,1));
  A->put(1,1, p_matrix_(1,1));
  A->put(2,1, p_matrix_(2,1));

  A->put(0,2, p_matrix_(0,2));
  A->put(1,2, p_matrix_(1,2));
  A->put(2,2, p_matrix_(2,2));

  a->put(0, p_matrix_(0,3));
  a->put(1, p_matrix_(1,3));
  a->put(2, p_matrix_(2,3));
}

//----------------------------------------------------------------
//
template <class T>
void
vgl_p_matrix<T>::get_rows (vnl_vector<T>* a, vnl_vector<T>* b, vnl_vector<T>* c) const
{
  a->put(0, p_matrix_(0, 0));
  a->put(1, p_matrix_(0, 1));
  a->put(2, p_matrix_(0, 2));
  a->put(3, p_matrix_(0, 3));

  b->put(0, p_matrix_(1, 0));
  b->put(1, p_matrix_(1, 1));
  b->put(2, p_matrix_(1, 2));
  b->put(3, p_matrix_(1, 3));

  c->put(0, p_matrix_(2, 0));
  c->put(1, p_matrix_(2, 1));
  c->put(2, p_matrix_(2, 2));
  c->put(3, p_matrix_(2, 3));
}

//----------------------------------------------------------------
//
template <class T>
void
vgl_p_matrix<T>::get_rows(vnl_vector_fixed<T,4>* a,
                          vnl_vector_fixed<T,4>* b,
                          vnl_vector_fixed<T,4>* c) const
{
  a->put(0, p_matrix_(0, 0));
  a->put(1, p_matrix_(0, 1));
  a->put(2, p_matrix_(0, 2));
  a->put(3, p_matrix_(0, 3));

  b->put(0, p_matrix_(1, 0));
  b->put(1, p_matrix_(1, 1));
  b->put(2, p_matrix_(1, 2));
  b->put(3, p_matrix_(1, 3));

  c->put(0, p_matrix_(2, 0));
  c->put(1, p_matrix_(2, 1));
  c->put(2, p_matrix_(2, 2));
  c->put(3, p_matrix_(2, 3));
}

//----------------------------------------------------------------
//
template <class T>
void
vgl_p_matrix<T>::set_rows (vnl_vector<T> const& a, vnl_vector<T> const& b, vnl_vector<T> const& c)
{
  p_matrix_.put(0, 0, a(0));
  p_matrix_.put(0, 1, a(1));
  p_matrix_.put(0, 2, a(2));
  p_matrix_.put(0, 3, a(3));

  p_matrix_.put(1, 0, b(0));
  p_matrix_.put(1, 1, b(1));
  p_matrix_.put(1, 2, b(2));
  p_matrix_.put(1, 3, b(3));

  p_matrix_.put(2, 0, c(0));
  p_matrix_.put(2, 1, c(1));
  p_matrix_.put(2, 2, c(2));
  p_matrix_.put(2, 3, c(3));
}

//-----------------------------------------------------------------------------
//
template <class T>
void
vgl_p_matrix<T>::set (const T p_matrix [3][4])
{
  for (int row_index = 0; row_index < 3; row_index++)
    for (int col_index = 0; col_index < 4; col_index++)
      p_matrix_. put (row_index, col_index, p_matrix [row_index][col_index]);
  clear_svd();
}

//-----------------------------------------------------------------------------
//
template <class T>
void
vgl_p_matrix<T>::set (const T *p)
{
  for (int row_index = 0; row_index < 3; row_index++)
    for (int col_index = 0; col_index < 4; col_index++)
      p_matrix_. put (row_index, col_index, *p++);
  clear_svd();
}


//----------------------------------------------------------------
//
template <class T>
void
vgl_p_matrix<T>::set (const vnl_matrix<T>& A, const vnl_vector<T>& a)
{
  p_matrix_(0,0) = A(0,0);
  p_matrix_(1,0) = A(1,0);
  p_matrix_(2,0) = A(2,0);

  p_matrix_(0,1) = A(0,1);
  p_matrix_(1,1) = A(1,1);
  p_matrix_(2,1) = A(2,1);

  p_matrix_(0,2) = A(0,2);
  p_matrix_(1,2) = A(1,2);
  p_matrix_(2,2) = A(2,2);

  p_matrix_(0,3) = a[0];
  p_matrix_(1,3) = a[1];
  p_matrix_(2,3) = a[2];
}

//----------------------------------------------------------------
//
template <class T>
void vgl_p_matrix<T>::set_identity()
{
  p_matrix_(0,0) = 1;
  p_matrix_(1,0) = 0;
  p_matrix_(2,0) = 0;

  p_matrix_(0,1) = 0;
  p_matrix_(1,1) = 1;
  p_matrix_(2,1) = 0;

  p_matrix_(0,2) = 0;
  p_matrix_(1,2) = 0;
  p_matrix_(2,2) = 1;

  p_matrix_(0,3) = 0;
  p_matrix_(1,3) = 0;
  p_matrix_(2,3) = 0;
}

template <class T>
void
vgl_p_matrix<T>::fix_cheirality()
{
  vnl_matrix_fixed<T,3,3> A;
  vnl_vector_fixed<T,3> a;
  this->get(&A, &a);
  double det = vnl_qr<double>(A).determinant();

  double scale = 1;
#if 0 // Used to scale by 1/det, but it's a bad idea if det is small
  if (vcl_fabs(det - 1) > 1e-8) {
    vcl_cerr << "vgl_p_matrix::fix_cheirality: Flipping, determinant is " << det << vcl_endl;
  }

  scale = 1/det;
#endif // 0
  if (det < 0)
    scale = -scale;

  p_matrix_ *= scale;
  if (svd_)
    svd_->W() *= scale;
}

template <class T>
bool vgl_p_matrix<T>::is_behind_camera(const vgl_homg_point_3d<T>& hX)
{
  vnl_vector_fixed<T,4> p = p_matrix_.get_row(2);
  double dot = hX.x()*p[0]+hX.y()*p[1]+hX.z()*p[2]+hX.w()*p[3];
  if (hX.w() < 0) dot = -dot;

  return dot < 0;
}

template <class T>
void vgl_p_matrix<T>::flip_sign()
{
  p_matrix_ *= -1;
  if (svd_)
    svd_->W() *= -1;
}

template <class T>
bool vgl_p_matrix<T>::looks_conditioned()
{
  double cond = svd()->W(0) / svd()->W(2);
  // vcl_cerr << "vgl_p_matrix::looks_conditioned: cond = " << cond << vcl_endl;
  return cond < 100;
}

template <class T>
vgl_p_matrix<T> vgl_p_matrix<T>::postmultiply(vnl_matrix_fixed<T,4,4> const& H) const
{
  return vgl_p_matrix<T>(p_matrix_ * H);
}

template <class T>
vgl_p_matrix<T> vgl_p_matrix<T>::premultiply(vnl_matrix_fixed<T,3,3> const& H) const
{
  return vgl_p_matrix<T>(H * p_matrix_);
}


//----------------------------------------------------------------------------
#undef VGL_P_MATRIX_INSTANTIATE
#define VGL_P_MATRIX_INSTANTIATE(T) \
template class vgl_p_matrix<T >; \
template vcl_ostream& operator << (vcl_ostream& s, const vgl_p_matrix<T >& h); \
template vcl_istream& operator >> (vcl_istream& s, vgl_p_matrix<T >& h)

#endif // vgl_p_matrix_txx_
