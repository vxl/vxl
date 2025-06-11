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
#include <vnl/vnl_math.h>
#include <vpgl/vpgl_earth_constants.h>

//==================== ground domain =====================

// reset ground domain
template <class T>
void
vpgl_ground_domain<T>::reset()
{
  id_ = vpgl_ground_domain_id::G;
  translation_.fill(T(0));
  rotation_.set_identity();
}

// convert geodetic to Earth Centered Earth Fixed (ECEF) coordinate
template <class T>
void
geodetic_to_ecef(const T lon_deg, const T lat_deg, const T elev,
                 T & x, T & y, T & z)
{
  // longitude in radians
  double lambda = lon_deg / vnl_math::deg_per_rad;
  double sl = sin(lambda), cl = cos(lambda);

  // latitude in radians
  double phi = lat_deg / vnl_math::deg_per_rad;
  double sp = sin(phi), cp = cos(phi);

  // ellipsoid radius of curvature at (lon, lat)
  double a = GRS84_a, b = GRS84_b;
  double rad_curv = (a*a) / sqrt((a*a*cp*cp) + (b*b*sp*sp));

  // ECEF coordinates
  x = (rad_curv + elev) * cp * cl;
  y = (rad_curv + elev) * cp * sl;
  z = (rad_curv * ((b*b) / (a*a)) + elev) * sp;
}

// convert world WGS84 coordinate to ground domain coordinate
// Inputs are as follows:
// - longitude in degrees on range (-180, 180)
// - latitude in degrees on range (-90, 90)
// - elevation in meters
template <class T>
void
vpgl_ground_domain<T>::world_to_ground(const T lon_deg, const T lat_deg, const T elev_m,
                                       T & x, T & y, T & z) const
{
  // process inputs according to id_
  switch (id_)
  {
    // geodetic coordinates
    // - convert lon/lat to radians
    // - "G": longitude on the range (-pi, pi)
    // - "H": longitude on the range (0, 2pi)
    case vpgl_ground_domain_id::G:
    case vpgl_ground_domain_id::H:
    {
      x = lon_deg / vnl_math::deg_per_rad;
      if ((id_ == vpgl_ground_domain_id::H) && (x < 0)) {
        x += vnl_math::twopi;
      }
      y = lat_deg / vnl_math::deg_per_rad;
      z = elev_m;
      break;
    }

    // rectangular coordinates
    // convert to Earth centered Earth fixed (ECEF), then apply
    // translation & rotation
    case vpgl_ground_domain_id::R:
    {
      vnl_vector_fixed<T, 3> ecef;
      geodetic_to_ecef(lon_deg, lat_deg, elev_m, ecef[0], ecef[1], ecef[2]);
      auto result = rotation_ * (ecef - translation_);
      x = result[0], y = result[1], z = result[2];
      break;
    }

    // unrecognized ground domain
    default:
      throw std::runtime_error("Unrecognized ground_domain");
  }
}

// VNL form: convert world WGS84 coordinate to ground domain coordinate
template <class T>
vnl_vector_fixed<T, 3>
vpgl_ground_domain<T>::world_to_ground(const vnl_vector_fixed<T, 3> & world_point) const
{
  vnl_vector_fixed<T, 3> gd_point;
  this->world_to_ground(world_point[0], world_point[1], world_point[2],
                        gd_point[0], gd_point[1], gd_point[2]);
  return gd_point;
}

// VGL form: convert world WGS84 coordinate to ground domain coordinate
template <class T>
vgl_point_3d<T>
vpgl_ground_domain<T>::world_to_ground(const vgl_point_3d<T> & world_point) const
{
  T x, y, z;
  this->world_to_ground(world_point.x(), world_point.y(), world_point.z(),
                        x, y, z);
  return vgl_point_3d<T>(x, y, z);
}

// print ground domain
template <class T>
std::ostream &
vpgl_ground_domain<T>::print(std::ostream& os) const
{
  os << "<vpgl_ground_domain \"" << id_ << "\"";
  if (id_ == vpgl_ground_domain_id::R) {
    os << "\ntranslation=" << translation_
       << "\nrotation=" << rotation_;
  }
  os << ">";
  return os;
}


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
  // world to ground domain
  T xgd, ygd, zgd;
  ground_domain_.world_to_ground(x, y, z, xgd, ygd, zgd);

  // select polycam
  size_t row, col;
  region_selector_.select(xgd, ygd, zgd, row, col);

  // polycam project
  T uu, vv;
  polycams_[row - 1][col - 1].project(xgd, ygd, zgd, uu, vv);

  // adjust
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
  template class vpgl_ground_domain<T>; \
  template class vpgl_polycam<T>;      \
  template class vpgl_RSM_camera<T>

#endif // vpgl_RSM_camera_hxx_
