// This is core/vgl/algo/vgl_h_matrix_2d.txx
#ifndef vgl_h_matrix_2d_txx_
#define vgl_h_matrix_2d_txx_
//:
// \file

#include "vgl_h_matrix_2d.h"
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_transpose.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/algo/vnl_svd.h>
#include <vcl_fstream.h>

//--------------------------------------------------------------
//

//: Default constructor
template <class T>
vgl_h_matrix_2d<T>::vgl_h_matrix_2d()
{
}

//: Copy constructor
template <class T>
vgl_h_matrix_2d<T>::vgl_h_matrix_2d(const vgl_h_matrix_2d<T>& M)
{
  t12_matrix_ = M.t12_matrix_;
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
//: Constructor
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


//: Destructor
template <class T>
vgl_h_matrix_2d<T>::~vgl_h_matrix_2d()
{
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
  vnl_vector_fixed<T,3> v2 = vnl_transpose(t12_matrix_) * v;
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
  Mp = vnl_transpose(t12_matrix_)*M*t12_matrix_;
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
  return t12_matrix_.read_ascii(s);
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
  for (int row_index = 0; row_index < 3; row_index++)
    for (int col_index = 0; col_index < 3; col_index++)
      *H++ = t12_matrix_.get(row_index, col_index);
}

//: Fill H with contents of this
template <class T>
void vgl_h_matrix_2d<T>::get(vnl_matrix<T>* H) const
{
  *H = t12_matrix_;
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
  for (int row_index = 0; row_index < 3; row_index++)
    for (int col_index = 0; col_index < 3; col_index++)
      t12_matrix_.put(row_index, col_index, *H++);
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
  vnl_svd<T> svd1(point_matrix, 1e-8);
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

//-------------------------------------------------------------------
template <class T>
bool vgl_h_matrix_2d<T>::
projective_basis(vcl_vector<vgl_homg_line_2d<T> > const& lines
#ifdef VCL_VC60
    , int dummy // parameter to help useless compiler disambiguate different functions
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
  vnl_svd<T> svd1(line_matrix, 1e-8);
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

  vnl_svd<T> svd(back_matrix);

  vnl_vector_fixed<T, 3> scales_vector = svd.solve(l3);

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
const vgl_h_matrix_2d<T> vgl_h_matrix_2d<T>::get_inverse() const
{
  vnl_matrix_fixed<T, 3, 3> temp = vnl_inverse(t12_matrix_);
  return vgl_h_matrix_2d<T>(temp);
}


//----------------------------------------------------------------------------
#undef VGL_H_MATRIX_2D_INSTANTIATE
#define VGL_H_MATRIX_2D_INSTANTIATE(T) \
template class vgl_h_matrix_2d<T >; \
template vcl_ostream& operator << (vcl_ostream& s, const vgl_h_matrix_2d<T >& h); \
template vcl_istream& operator >> (vcl_istream& s, vgl_h_matrix_2d<T >& h)

#endif // vgl_h_matrix_2d_txx_
