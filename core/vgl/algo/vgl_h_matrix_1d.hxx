// This is core/vgl/algo/vgl_h_matrix_1d.hxx
#ifndef vgl_h_matrix_1d_hxx_
#define vgl_h_matrix_1d_hxx_

#include <cstdlib>
#include <iostream>
#include <fstream>
#include "vgl_h_matrix_1d.h"
#include <vgl/vgl_homg_point_1d.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/algo/vnl_svd.h>
#include <vcl_compiler_detection.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
# include <vcl_deprecated.h>

//--------------------------------------------------------------------------------
template <class T>
vgl_h_matrix_1d<T>::vgl_h_matrix_1d(std::istream &is)
{
  t12_matrix_.read_ascii(is);
}

template <class T>
vgl_h_matrix_1d<T>::vgl_h_matrix_1d(std::vector<vgl_homg_point_1d<T> > const& points1,
                                    std::vector<vgl_homg_point_1d<T> > const& points2)
{
  vnl_matrix<T> W;
  assert(points1.size() == points2.size());
  unsigned int numpoints = points1.size();
  if (numpoints < 3)
  {
    std::cerr << "\nvhl_h_matrix_1d - minimum of 3 points required\n";
    std::exit(0);
  }

  W.set_size(numpoints, 4);

  for (unsigned int i = 0; i < numpoints; i++)
  {
    T x1 = points1[i].x(), w1 = points1[i].w();
    T x2 = points2[i].x(), w2 = points2[i].w();

    W[i][0]=x1*w2;    W[i][1]=w1*w2;
    W[i][2]=-x1*x2;   W[i][3]=-w1*x2;
  }

  vnl_svd<T> SVD(W);
  t12_matrix_ = vnl_matrix_fixed<T,2,2>(SVD.nullvector().data_block()); // 4-dim. nullvector
}

// == OPERATIONS ==

template <class T>
vgl_homg_point_1d<T>
vgl_h_matrix_1d<T>::operator()(vgl_homg_point_1d<T> const& p) const
{
  vnl_vector_fixed<T,2> v = t12_matrix_ * vnl_vector_fixed<T,2>(p.x(),p.w());
  return vgl_homg_point_1d<T>(v[0], v[1]);
}

template <class T>
vgl_homg_point_1d<T>
vgl_h_matrix_1d<T>::preimage(vgl_homg_point_1d<T> const& q) const
{
  vnl_vector_fixed<T,2> v = vnl_inverse(t12_matrix_) * vnl_vector_fixed<T,2>(q.x(),q.w());
  return vgl_homg_point_1d<T>(v[0], v[1]);
}

template <class T>
vgl_h_matrix_1d<T>
vgl_h_matrix_1d<T>::get_inverse() const
{
  return vgl_h_matrix_1d<T>(vnl_inverse(t12_matrix_));
}

//-----------------------------------------------------------------------------
template <class T>
std::ostream& operator<<(std::ostream& s, vgl_h_matrix_1d<T> const& H)
{
  return s << H.get_matrix();
}

template <class T>
bool vgl_h_matrix_1d<T>::read(std::istream& s)
{
  t12_matrix_.read_ascii(s);
  return s.good() || s.eof();
}


template <class T>
vgl_h_matrix_1d<T>::vgl_h_matrix_1d(char const* filename)
{
  std::ifstream f(filename);
  if (!f.good())
    std::cerr << "vgl_h_matrix_1d::read: Error opening " << filename << std::endl;
  else
    t12_matrix_.read_ascii(f);
}

template <class T>
bool vgl_h_matrix_1d<T>::read(char const* filename)
{
  std::ifstream f(filename);
  if (!f.good())
    std::cerr << "vgl_h_matrix_1d::read: Error opening " << filename << std::endl;
  return read(f);
}

// == DATA ACCESS ==

//-----------------------------------------------------------------------------
template <class T>
T vgl_h_matrix_1d<T>::get(unsigned int row_index, unsigned int col_index) const
{
  return t12_matrix_.get(row_index, col_index);
}

template <class T>
void vgl_h_matrix_1d<T>::get(T *H) const
{
  for (T const* iter = t12_matrix_.begin(); iter < t12_matrix_.end(); ++iter)
    *H++ = *iter;
}

template <class T>
void vgl_h_matrix_1d<T>::get(vnl_matrix_fixed<T,2,2>* H) const
{
  *H = t12_matrix_;
}

template <class T>
void vgl_h_matrix_1d<T>::get(vnl_matrix<T>* H) const
{
  VXL_DEPRECATED_MACRO("vgl_h_matrix_1d<T>::get(vnl_matrix<T>*) const");
  *H = t12_matrix_.as_ref(); // size 2x2
}

template <class T>
vgl_h_matrix_1d<T>&
vgl_h_matrix_1d<T>::set(const T* H)
{
  for (T* iter = t12_matrix_.begin(); iter < t12_matrix_.end(); ++iter)
    *iter = *H++;
  return *this;
}

template <class T>
vgl_h_matrix_1d<T>&
vgl_h_matrix_1d<T>::set(vnl_matrix_fixed<T,2,2> const& H)
{
  t12_matrix_ = H;
  return *this;
}

template <class T>
vgl_h_matrix_1d<T>&
vgl_h_matrix_1d<T>::set_identity()
{
  t12_matrix_.set_identity();
  return *this;
}

template <class T>
vgl_h_matrix_1d<T>&
vgl_h_matrix_1d<T>::set_scale(T scale)
{
  t12_matrix_[0][0]*=scale;
  t12_matrix_[0][1]*=scale;
  return *this;
}

template <class T>
vgl_h_matrix_1d<T>&
vgl_h_matrix_1d<T>::set_translation(T tx)
{
  t12_matrix_[0][1] = tx;
  return *this;
}

template <class T>
vgl_h_matrix_1d<T>&
vgl_h_matrix_1d<T>::set_affine(vnl_matrix_fixed<T,1,2> const& M12)
{
  for (unsigned r = 0; r<1; ++r)
    for (unsigned c = 0; c<2; ++c)
      t12_matrix_[r][c] = M12[r][c];
  t12_matrix_[1][0] = T(0); t12_matrix_[1][1] = T(1);
  return *this;
}

//-------------------------------------------------------------------
template <class T>
bool vgl_h_matrix_1d<T>::projective_basis(std::vector<vgl_homg_point_1d<T> > const& points)
{
  if (points.size()!=3)
    return false;
  vnl_vector_fixed<T,2> p0(points[0].x(), points[0].w());
  vnl_vector_fixed<T,2> p1(points[1].x(), points[1].w());
  vnl_vector_fixed<T,2> p2(points[2].x(), points[2].w());
  vnl_matrix_fixed<T,2,3> point_matrix;
  point_matrix.set_column(0, p0);
  point_matrix.set_column(1, p1);
  point_matrix.set_column(2, p2);

  if (! point_matrix.is_finite() || point_matrix.has_nans())
  {
    std::cerr << "vgl_h_matrix_1d<T>::projective_basis():\n"
             << " given points have infinite or NaN values\n";
    this->set_identity();
    return false;
  }
  vnl_svd<T> svd1(point_matrix.as_ref(), 1e-8); // size 2x3
  if (svd1.rank() < 2)
  {
    std::cerr << "vgl_h_matrix_1d<T>::projective_basis():\n"
             << " At least two out of the three points are nearly identical\n";
    this->set_identity();
    return false;
  }

  vnl_matrix_fixed<T,2,2> back_matrix;
  back_matrix.set_column(0, p0);
  back_matrix.set_column(1, p1);

  vnl_vector_fixed<T,2> scales_vector = vnl_inverse(back_matrix) * p2;

  back_matrix.set_column(0, scales_vector[0] * p0);
  back_matrix.set_column(1, scales_vector[1] * p1);

  if (! back_matrix.is_finite() || back_matrix.has_nans())
  {
    std::cerr << "vgl_h_matrix_1d<T>::projective_basis():\n"
             << " back matrix has infinite or NaN values\n";
    this->set_identity();
    return false;
  }
  this->set(vnl_inverse(back_matrix));
  return true;
}

template <class T>
bool vgl_h_matrix_1d<T>::is_identity() const
{
  return t12_matrix_.is_identity();
}

template <class T>
bool vgl_h_matrix_1d<T>::is_rotation() const
{
  // the only 1-D rotation is the identity transformation:
  return t12_matrix_.is_identity();
}


template <class T>
bool vgl_h_matrix_1d<T>::is_euclidean() const
{
  // no translational part, and scale 1:
  return t12_matrix_.get(0,0) == (T)1 &&
         t12_matrix_.get(1,0) == (T)0 &&
         t12_matrix_.get(1,1) == (T)1 ;
}


//----------------------------------------------------------------------------
#undef VGL_H_MATRIX_1D_INSTANTIATE
#define VGL_H_MATRIX_1D_INSTANTIATE(T) \
template class vgl_h_matrix_1d<T >; \
template std::ostream& operator << (std::ostream& s, vgl_h_matrix_1d<T > const& h); \
template std::istream& operator >> (std::istream& s, vgl_h_matrix_1d<T >& h)

#endif // vgl_h_matrix_1d_hxx_
