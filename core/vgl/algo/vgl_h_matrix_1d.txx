// This is core/vgl/algo/vgl_h_matrix_1d.txx
#ifndef vgl_h_matrix_1d_txx_
#define vgl_h_matrix_1d_txx_
//:
// \file

#include "vgl_h_matrix_1d.h"
#include <vgl/vgl_homg_point_1d.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/algo/vnl_svd.h>
#include <vcl_fstream.h>

//--------------------------------------------------------------
//: Default constructor
template <class T>
vgl_h_matrix_1d<T>::vgl_h_matrix_1d()
{
}

//: Copy constructor
template <class T>
vgl_h_matrix_1d<T>::vgl_h_matrix_1d(const vgl_h_matrix_1d<T>& M)
  : t12_matrix_(M.t12_matrix_)
{
}

//--------------------------------------------------------------
//: Constructor
template <class T>
vgl_h_matrix_1d<T>::vgl_h_matrix_1d(vnl_matrix_fixed<T,2,2> const& M)
  : t12_matrix_(M)
{
}

//--------------------------------------------------------------------------------
template <class T>
vgl_h_matrix_1d<T>::vgl_h_matrix_1d(const vgl_h_matrix_1d<T>& A,const vgl_h_matrix_1d<T>& B)
  : t12_matrix_(A.t12_matrix_ * B.t12_matrix_)
{
}

//--------------------------------------------------------------
//: Constructor
template <class T>
vgl_h_matrix_1d<T>::vgl_h_matrix_1d(const T* H)
  : t12_matrix_(H)
{
}

template <class T>
vgl_h_matrix_1d<T>::vgl_h_matrix_1d(vcl_istream &is)
{
  t12_matrix_.read_ascii(is);
}

//: Destructor
template <class T>
vgl_h_matrix_1d<T>::~vgl_h_matrix_1d()
{
}

// == OPERATIONS ==

//: Return the transformed point given by $x_2 = {\tt H} x_1$
template <class T>
vgl_homg_point_1d<T> vgl_h_matrix_1d<T>::operator()(const vgl_homg_point_1d<T>& x1) const
{
  vnl_vector_fixed<T,2> v = t12_matrix_ * vnl_vector_fixed<T,2>(x1.x(),x1.w());
  return vgl_homg_point_1d<T>(v[0], v[1]);
}

//: Return the transformed point given by $x_1 = {\tt H}^{-1} x_2$
template <class T>
vgl_homg_point_1d<T> vgl_h_matrix_1d<T>::preimage(const vgl_homg_point_1d<T>& x2) const
{
  vnl_vector_fixed<T,2> v = vnl_inverse(t12_matrix_) *
                            vnl_vector_fixed<T,2>(x2.x(),x2.w());
  return vgl_homg_point_1d<T>(v[0], v[1]);
}

template <class T>
vgl_homg_point_1d<T> vgl_h_matrix_1d<T>::operator*(const vgl_homg_point_1d<T>& x1) const
{
  return (*this)(x1);
}

//: Return the inverse
//
template <class T>
const vgl_h_matrix_1d<T> vgl_h_matrix_1d<T>::get_inverse() const
{
  vnl_matrix_fixed<T,2,2> temp = vnl_inverse(t12_matrix_);
  return vgl_h_matrix_1d<T>(temp);
}

//-----------------------------------------------------------------------------
//: Print H on vcl_ostream
template <class T>
vcl_ostream& operator<<(vcl_ostream& s, const vgl_h_matrix_1d<T>& h)
{
  return s << h.get_matrix();
}

//: Read H from vcl_istream
template <class T>
vcl_istream& operator >> (vcl_istream& s, vgl_h_matrix_1d<T>& H)
{
  H = vgl_h_matrix_1d<T>(s);
  return s;
}

//: Read H from vcl_istream
template <class T>
vgl_h_matrix_1d<T> vgl_h_matrix_1d<T>::read(vcl_istream& s)
{
  return vgl_h_matrix_1d<T>(s);
}


//: Read H from file
template <class T>
vgl_h_matrix_1d<T> vgl_h_matrix_1d<T>::read(char const* filename)
{
  vcl_ifstream f(filename);
  if (!f.good())
    vcl_cerr << "vgl_h_matrix_1d::read: Error opening " << filename << vcl_endl;
  return vgl_h_matrix_1d<T>(f);
}

// == DATA ACCESS ==

//-----------------------------------------------------------------------------
//: Get matrix element at (row_index, col_index)
template <class T>
T vgl_h_matrix_1d<T>::get(unsigned int row_index, unsigned int col_index) const
{
  return t12_matrix_. get(row_index, col_index);
}

//: Fill H with contents of this
template <class T>
void vgl_h_matrix_1d<T>::get(T *H) const
{
  for (int row_index = 0; row_index < 2; row_index++)
    for (int col_index = 0; col_index < 2; col_index++)
      *H++ = t12_matrix_.get(row_index, col_index);
}

//: Fill H with contents of this
template <class T>
void vgl_h_matrix_1d<T>::get(vnl_matrix<T>* H) const
{
  *H = t12_matrix_;
}

//: Set to 2x2 row-stored matrix
template <class T>
void vgl_h_matrix_1d<T>::set(const T* H)
{
  for (int row_index = 0; row_index < 2; row_index++)
    for (int col_index = 0; col_index < 2; col_index++)
      t12_matrix_.put(row_index, col_index, *H++);
}

//: Set to given vnl_matrix
template <class T>
void vgl_h_matrix_1d<T>::set(vnl_matrix_fixed<T,2,2> const& H)
{
  t12_matrix_ = H;
}

//-------------------------------------------------------------------
template <class T>
bool vgl_h_matrix_1d<T>::
projective_basis(vcl_vector<vgl_homg_point_1d<T> > const& points)
{
  if (points.size()!=3)
    return false;
  vnl_vector_fixed<T, 2> p0(points[0].x(), points[0].w());
  vnl_vector_fixed<T, 2> p1(points[1].x(), points[1].w());
  vnl_vector_fixed<T, 2> p2(points[2].x(), points[2].w());
  vnl_matrix_fixed<T, 2, 3> point_matrix;
  point_matrix.set_column(0, p0);
  point_matrix.set_column(1, p1);
  point_matrix.set_column(2, p2);

  if (! point_matrix.is_finite() || point_matrix.has_nans())
  {
    vcl_cerr << "vgl_h_matrix_1d<T>::projective_basis():\n"
             << " given points have infinite or NaN values\n";
    this->set_identity();
    return false;
  }
  vnl_svd<T> svd1(point_matrix, 1e-8);
  if (svd1.rank() < 2)
  {
    vcl_cerr << "vgl_h_matrix_1d<T>::projective_basis():\n"
             << " At least two out of the three points are nearly identical\n";
    this->set_identity();
    return false;
  }

  vnl_matrix_fixed<T, 2, 2> back_matrix;
  back_matrix.set_column(0, p0);
  back_matrix.set_column(1, p1);

  vnl_vector_fixed<T, 2> scales_vector = vnl_inverse(back_matrix) * p2;

  back_matrix.set_column(0, scales_vector[0] * p0);
  back_matrix.set_column(1, scales_vector[1] * p1);

  if (! back_matrix.is_finite() || back_matrix.has_nans())
  {
    vcl_cerr << "vgl_h_matrix_1d<T>::projective_basis():\n"
             << " back matrix has infinite or NaN values\n";
    this->set_identity();
    return false;
  }
  this->set(vnl_inverse(back_matrix));
  return true;
}


//----------------------------------------------------------------------------
#undef VGL_H_MATRIX_1D_INSTANTIATE
#define VGL_H_MATRIX_1D_INSTANTIATE(T) \
template class vgl_h_matrix_1d<T >; \
template vcl_ostream& operator << (vcl_ostream& s, const vgl_h_matrix_1d<T >& h); \
template vcl_istream& operator >> (vcl_istream& s, vgl_h_matrix_1d<T >& h)

#endif // vgl_h_matrix_1d_txx_
