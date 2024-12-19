// This is core/vpgl/vpgl_RSM_camera.hxx
#ifndef vpgl_RSM_camera_hxx_
#define vpgl_RSM_camera_hxx_
//:
// \file

#include <vector>
#include <fstream>
#include <iomanip>
#include "vpgl_RSM_camera.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
// #include <vnl/io/vnl_io_matrix_fixed.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

//
//==================== polycam =====================
//
//--------------------------------------
// Constructors

// Create an identity projection, i.e. (x,y) identically maps to (u,v)
template <class T>
vpgl_polycam<T>::vpgl_polycam()
  : ridx_(0)
  , cidx_(0)
{
  vpgl_scale_offset<T> soff;
  scale_offsets_.resize(5, soff);
}


//--------------------------------------
// Set coefficient matrix

// set coefficients from 4 vectors
template <class T>
void
vpgl_polycam<T>::set_coefficients(const std::vector<T> & neu_u,
                                  const std::vector<T> & den_u,
                                  const std::vector<T> & neu_v,
                                  const std::vector<T> & den_v)
{
  coeffs_.clear();
  coeffs_.push_back(neu_u);
  coeffs_.push_back(den_u);
  coeffs_.push_back(neu_v);
  coeffs_.push_back(den_v);
}

// set coefficients from array encoding
template <class T>
void
vpgl_polycam<T>::set_coefficients(const std::vector<std::vector<T>> & coeffs)
{
  coeffs_ = coeffs;
}

// get coefficients as std vector of vectors
template <class T>
std::vector<std::vector<T>>
vpgl_polycam<T>::coefficients() const
{
  return coeffs_;
}
template <class T>
void
vpgl_polycam<T>::set_powers(const std::vector<int> & neu_u_powers,
                            const std::vector<int> & den_u_powers,
                            const std::vector<int> & neu_v_powers,
                            const std::vector<int> & den_v_powers)
{
  powers_.clear();
  powers_.push_back(neu_u_powers);
  powers_.push_back(den_u_powers);
  powers_.push_back(neu_v_powers);
  powers_.push_back(den_v_powers);
}

//--------------------------------------
// Set scale/offset values

// set all scale offsets from individual values
template <class T>
void
vpgl_polycam<T>::set_scale_offsets(const T x_scale,
                                   const T x_off,
                                   const T y_scale,
                                   const T y_off,
                                   const T z_scale,
                                   const T z_off,
                                   const T u_scale,
                                   const T u_off,
                                   const T v_scale,
                                   const T v_off)
{
  scale_offsets_.resize(5);
  scale_offsets_[X_INDX] = vpgl_scale_offset<T>(x_scale, x_off);
  scale_offsets_[Y_INDX] = vpgl_scale_offset<T>(y_scale, y_off);
  scale_offsets_[Z_INDX] = vpgl_scale_offset<T>(z_scale, z_off);
  scale_offsets_[U_INDX] = vpgl_scale_offset<T>(u_scale, u_off);
  scale_offsets_[V_INDX] = vpgl_scale_offset<T>(v_scale, v_off);
}

// set all scale offsets from vector
template <class T>
void
vpgl_polycam<T>::set_scale_offsets(const std::vector<vpgl_scale_offset<T>> & scale_offsets)
{
  scale_offsets_ = scale_offsets;
}

//--------------------------------------
// Project 3D world point into 2D image space

// generic interface
template <class T>
void
vpgl_polycam<T>::project(const T x, const T y, const T z, T & u, T & v) const
{
  // scale, offset the world point before projection
  T sx = scale_offsets_[X_INDX].normalize(x);
  T sy = scale_offsets_[Y_INDX].normalize(y);
  T sz = scale_offsets_[Z_INDX].normalize(z);

  // projection
  // ==== u neumerator ====
  double num_u = 0.0;
  int c = 0;
  for (int k = 0; k <= powers_[P_NEU_U][Z_INDX]; ++k)
    for (int j = 0; j <= powers_[P_NEU_U][Y_INDX]; ++j)
      for (int i = 0; i <= powers_[P_NEU_U][X_INDX]; ++i)
        num_u += coeffs_[P_NEU_U][c++] * pow(sx, i) * pow(sy, j) * pow(sz, k);

  double den_u = 0.0;
  c = 0;
  // ==== u denominator ====
  for (int k = 0; k <= powers_[P_DEN_U][Z_INDX]; ++k)
    for (int j = 0; j <= powers_[P_DEN_U][Y_INDX]; ++j)
      for (int i = 0; i <= powers_[P_DEN_U][X_INDX]; ++i)
        den_u += coeffs_[P_DEN_U][c++] * pow(sx, i) * pow(sy, j) * pow(sz, k);

  // ==== v neumerator ====
  double num_v = 0.0;
  c = 0;
  for (int k = 0; k <= powers_[P_NEU_V][Z_INDX]; ++k)
    for (int j = 0; j <= powers_[P_NEU_V][Y_INDX]; ++j)
      for (int i = 0; i <= powers_[P_NEU_V][X_INDX]; ++i)
        num_v += coeffs_[P_NEU_V][c++] * pow(sx, i) * pow(sy, j) * pow(sz, k);

  // ==== v denominator ====
  double den_v = 0.0;
  c = 0;
  for (int k = 0; k <= powers_[P_DEN_V][Z_INDX]; ++k)
    for (int j = 0; j <= powers_[P_DEN_V][Y_INDX]; ++j)
      for (int i = 0; i <= powers_[P_DEN_V][X_INDX]; ++i)
        den_v += coeffs_[P_DEN_V][c++] * pow(sx, i) * pow(sy, j) * pow(sz, k);

  // ratios
  double su = num_u / den_u;
  double sv = num_v / den_v;
  // unscale the resulting image coordinates
  u = scale_offsets_[U_INDX].un_normalize(su);
  v = scale_offsets_[V_INDX].un_normalize(sv);
}
// vnl interface
template <class T>
vnl_vector_fixed<T, 2>
vpgl_polycam<T>::project(const vnl_vector_fixed<T, 3> & world_point) const
{
  vnl_vector_fixed<T, 2> image_point;
  this->project(world_point[0], world_point[1], world_point[2], image_point[0], image_point[1]);
  return image_point;
}

// vgl interface
template <class T>
vgl_point_2d<T>
vpgl_polycam<T>::project(vgl_point_3d<T> world_point) const
{
  T u = 0, v = 0;
  this->project(world_point.x(), world_point.y(), world_point.z(), u, v);
  return vgl_point_2d<T>(u, v);
}

//
//================ RSM_camera ===========================
//
//--------------------------------------
// Clone


template <class T>
vpgl_RSM_camera<T> *
vpgl_RSM_camera<T>::clone() const
{
  return new vpgl_RSM_camera<T>(*this);
}
// generic base interface
template <class T>
void
vpgl_RSM_camera<T>::project(const T x, const T y, const T z, T & u, T & v) const
{
  size_t row, col;
  region_selector_.select(x, y, z, row, col);
  T uu, vv;
  polycams_[row - 1][col - 1].project(x, y, z, uu, vv);
  u = uu - adj_u_;
  v = vv - adj_v_;
}

// vnl interface
template <class T>
vnl_vector_fixed<T, 2>
vpgl_RSM_camera<T>::project(const vnl_vector_fixed<T, 3> & world_point) const
{
  vnl_vector_fixed<T, 2> image_point;
  this->project(world_point[0], world_point[1], world_point[2], image_point[0], image_point[1]);
  return image_point;
}

// vgl interface
template <class T>
vgl_point_2d<T>
vpgl_RSM_camera<T>::project(vgl_point_3d<T> world_point) const
{
  T u = 0, v = 0;
  this->project(world_point.x(), world_point.y(), world_point.z(), u, v);
  return vgl_point_2d<T>(u, v);
}
// Code for easy instantiation.
#undef vpgl_RSM_CAMERA_INSTANTIATE
#define vpgl_RSM_CAMERA_INSTANTIATE(T) \
  template class vpgl_scale_offset<T>; \
  template class vpgl_polycam<T>;      \
  template class vpgl_RSM_camera<T>

#endif // vpgl_RSM_camera_hxx_
