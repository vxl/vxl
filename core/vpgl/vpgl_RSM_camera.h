// This is core/vpgl/vpgl_RSM_camera.h
#ifndef vpgl_RSM_camera_h_
#define vpgl_RSM_camera_h_
//:
// \file
// \brief A camera model based on the replacement sensor model
// \author Joseph Mundy
// \date October 2023
//
//  The replacement sensor model (RSM) camera projects 3-d world points according to ratios of
//  cubic polynomials. That is,
// \verbatim
//           neu_u(X,Y,Z)      neu_v(X,Y,Z)
//       u = ------------  v = ------------
//           den_u(X,Y,Z)      den_v(X,Y,X)
// \endverbatim
//  where u is the image column index and v is the image row index, and X, Y are
//  longitude and latitude in radians, and Z is in meters above the ellipsoid
//
//  neu_u(X,Y,Z),den_u(X,Y,Z), neu_v(X,Y,Z), den_v(X,Y,Z) are
//  polynomials in three variables with maximum power of 5 for each of X,Y,Z.
//  e.g., for power 3 for each of X, Y, Z and monomial coeffients C:
// \verbatim
//                  k=3   j=3   i=3 (      i  j  k )
//  Poly(X,Y,Z) = Sum   Sum   Sum   ( C   x  y  z  )
//                  k=0   j=0   i=0 (  ijk         )
// \endverbatim
//  In this case, there are 64 monomial terms in the order C   , C   , ... C
//                                                          000   100       333
//
//  Polynomial calculations are often ill-conditioned if the variables are not
//  normalized. All 5 variables, u,v,X,Y,Z, are normalized to the range [-1, 1].
//  E.g., Xnorm = (X - Xoffset)/Xscale
//  This normalization requires 10 additional offset and scale
//  parameters for a total of 266 parameters for max power 3.

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <stdexcept>
#include <array>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include "vpgl_camera.h"
#include "vpgl_rational_camera.h" //for scale and offset class


//========================= RSM ground domain =================================
// Convert from WGS84 lon (degrees), lat (degrees), elevation (meters) to
// RSM ground domain coordinates. Ground domain may be a geodetic ("G" or "H")
// or a local retangular ("R") coordinate system.

// ground domain identifier & functions
enum class vpgl_ground_domain_id
{
  G,
  H,
  R,
};

class vpgl_ground_domain_id_func
{
public:
  static std::string
  to_string(vpgl_ground_domain_id id);
  static vpgl_ground_domain_id
  from_string(const std::string & buf);

private:
  vpgl_ground_domain_id_func() = delete;
};

std::ostream &
operator<<(std::ostream & os, const vpgl_ground_domain_id & id);

// ground domain
template <class T>
class vpgl_ground_domain
{
public:

  // constructors
  vpgl_ground_domain()
  {}

  vpgl_ground_domain(vpgl_ground_domain_id id)
    : id_(id)
  {}

  vpgl_ground_domain(std::string id)
    : id_(vpgl_ground_domain_id_func::from_string(id))
  {}

  // reset
  void
  reset();

  // convert world WGS84 coordinate to ground domain coordinate
  void
  world_to_ground(const T lon_deg, const T lat_deg, const T elev_m,
                  T & x, T & y, T & z) const;

  vnl_vector_fixed<T, 3>
  world_to_ground(const vnl_vector_fixed<T, 3> & world_point) const;

  vgl_point_3d<T>
  world_to_ground(const vgl_point_3d<T> & world_point) const;

  // print
  std::ostream &
  print(std::ostream& os) const;

  // properties
  vpgl_ground_domain_id id_ = vpgl_ground_domain_id::G;
  vnl_vector_fixed<T, 3> translation_{0, 0, 0};
  vnl_matrix_fixed<T, 3, 3> rotation_{vnl_matrix_fixed<T, 3, 3>().set_identity()};

};

// ground_domain operator<<
template <class T>
std::ostream &
operator<<(std::ostream & os, const vpgl_ground_domain<T> & gd)
{
  return gd.print(os);
}


//======================= RSM region selector =================================
template <class T>
class vpgl_region_selector
{
public:
  vpgl_region_selector()
    : rnis_(1)
    , cnis_(1)
    , tnis_(0)
    , minr_(0)
    , maxr_(0)
    , minc_(0)
    , maxc_(0)
    , rssiz_(0.0)
    , cssiz_(0.0)
  {
    row_coefs_.resize(10);
    col_coefs_.resize(10);
  }

  vpgl_region_selector(std::vector<T> row_coefs,
                       std::vector<T> col_coefs,
                       size_t minr,
                       size_t maxr,
                       size_t minc,
                       size_t maxc,
                       size_t rnis,
                       size_t cnis,
                       size_t tnis,
                       size_t rssiz,
                       size_t cssiz)
    : row_coefs_(row_coefs)
    , col_coefs_(col_coefs)
    , minr_(minr)
    , maxr_(maxr)
    , minc_(minc)
    , maxc_(maxc)
    , rnis_(rnis)
    , cnis_(cnis)
    , tnis_(tnis)
    , rssiz_(rssiz)
    , cssiz_(cssiz)
  {}

  void
  select(T X, T Y, T Z, size_t & region_row, size_t & region_col) const;
  std::vector<T> row_coefs_;
  std::vector<T> col_coefs_;
  size_t minr_;
  size_t maxr_;
  size_t minc_;
  size_t maxc_;
  size_t rnis_;
  size_t cnis_;
  size_t tnis_;
  T rssiz_;
  T cssiz_;
};


//================================ POLYCAM ====================================
template <class T>
class vpgl_polycam
{
public:
  //: enumeration for indexing coordinates
  enum coor_index
  {
    X_INDX = 0,
    Y_INDX,
    Z_INDX,
    U_INDX,
    V_INDX
  };
  //: enumeration for indexing polynomials
  enum poly_index
  {
    NEU_U = 0,
    DEN_U,
    NEU_V,
    DEN_V
  };
  //: enumeration for computing polys
  enum poly_comp_index
  {
    P_NEU_U = 0,
    P_DEN_U,
    P_NEU_V,
    P_DEN_V
  };
  vpgl_polycam();
  vpgl_polycam(size_t ridx, size_t cidx)
    : ridx_(ridx)
    , cidx_(cidx)
  {}
  vpgl_polycam(size_t ridx,
               size_t cidx,
               const std::vector<std::vector<int>> & powers,
               const std::vector<std::vector<T>> & coeffs,
               const std::vector<vpgl_scale_offset<T>> & scale_offsets)
    : ridx_(ridx)
    , cidx_(cidx)
    , powers_(powers)
    , coeffs_(coeffs)
    , scale_offsets_(scale_offsets)
  {}

  size_t
  ridx() const
  {
    return ridx_;
  }
  size_t
  cidx() const
  {
    return cidx_;
  }

  //: set RSM polynomial coefficients
  void
  set_coefficients(const std::vector<T> & neu_u,
                   const std::vector<T> & den_u,
                   const std::vector<T> & neu_v,
                   const std::vector<T> & den_v);

  // In the order neu_u, den_u, neu_v, den_v
  void
  set_coefficients(const std::vector<std::vector<T>> & RSM_coeffs);

  //: get the RSM polynomial coefficients in std vector of vectors
  // In the order neu_u, den_u, neu_v, den_v
  std::vector<std::vector<T>>
  coefficients() const;

  //: set the maximum power of each of x, y, z
  //  for each of the four polynomials in the order,
  //  neu_u, den_u, neu_v, den_v
  void
  set_powers(const std::vector<std::vector<int>> & powers)
  {
    powers_ = powers;
  }

  void
  set_powers(const std::vector<int> & neu_u_powers,
             const std::vector<int> & den_u_powers,
             const std::vector<int> & neu_v_powers,
             const std::vector<int> & den_v_powers);

  std::vector<std::vector<int>>
  powers() const
  {
    return powers_;
  }

  //: set all coordinate scale and offsets
  void
  set_scale_offsets(const T x_scale,
                    const T x_off,
                    const T y_scale,
                    const T y_off,
                    const T z_scale,
                    const T z_off,
                    const T u_scale,
                    const T u_off,
                    const T v_scale,
                    const T v_off);

  void
  set_scale_offsets(const std::vector<vpgl_scale_offset<T>> & scale_offsets);

  //: get the scale and offsets in a vector
  std::vector<vpgl_scale_offset<T>>
  scale_offsets() const
  {
    return scale_offsets_;
  }
  //: Equality test
  inline bool
  operator==(const vpgl_polycam<T> & that) const
  {
    return this == &that || ((this->coefficients() == that.coefficients()) && (this->powers() == that.powers()) &&
                             (this->scale_offsets() == that.scale_offsets()));
    return true;
  }

  //: The generic camera interface. u represents image column, v image row.
  void
  project(const T x, const T y, const T z, T & u, T & v) const;

  //: Project a world point onto the image (vnl interface)
  vnl_vector_fixed<T, 2>
  project(const vnl_vector_fixed<T, 3> & world_point) const;

  //: Project a world point onto the image (vgl interface)
  vgl_point_2d<T>
  project(vgl_point_3d<T> world_point) const;

private:
  size_t ridx_;
  size_t cidx_;
  std::vector<std::vector<int>> powers_;
  std::vector<std::vector<T>> coeffs_;
  std::vector<vpgl_scale_offset<T>> scale_offsets_;
};
//
//--------------------=== replacement sensor model (RSM)  camera ===-----------------------
//
template <class T>
class vpgl_RSM_camera : public vpgl_camera<T>
{
public:
  //: default constructor
  vpgl_RSM_camera()
    : adj_u_(T(0))
    , adj_v_(T(0))
  {}

  ~vpgl_RSM_camera() = default;

  std::string
  type_name() const override
  {
    return "vpgl_RSM_camera";
  }

  //: Clone `this': creation of a new object and initialization
  // legal C++ because the return type is covariant with vpgl_camera<T>*
  vpgl_RSM_camera<T> *
  clone() const override;

  //: A single polynomial model
  vpgl_RSM_camera(const vpgl_polycam<T> & pcam)
    : adj_u_(T(0))
    , adj_v_(T(0))
  {
    std::vector<vpgl_polycam<T>> temp;
    temp.push_back(pcam);
    polycams_.push_back(temp);
  }
  // set region selector
  vpgl_RSM_camera(const vpgl_region_selector<T> & region_selector)
    : adj_u_(T(0))
    , adj_v_(T(0))
  {
    region_selector_ = region_selector;
    polycams_.resize(region_selector.rnis_, std::vector<vpgl_polycam<T>>(region_selector.cnis_));
  }

  // ground domain
  void
  set_ground_domain(const vpgl_ground_domain<T> & ground_domain)
  {
    ground_domain_ = ground_domain;
  }

  vpgl_ground_domain<T>
  ground_domain() const
  {
    return ground_domain_;
  }

  //: Set a polycam for a specified region
  void
  set_polycam(size_t row, size_t col, const vpgl_polycam<T> & pcam)
  {
    polycams_[row - 1][col - 1] = pcam;
  };

  //: Set all polycams       row           col
  void
  set_all_polycams(const std::vector<std::vector<vpgl_polycam<T>>> & pcams,
                   const vpgl_region_selector<T> & region_selector)
  {
    region_selector_ = region_selector;
    polycams_ = pcams;
  }
  std::vector<std::vector<vpgl_polycam<T>>> &
  polycams()
  {
    return polycams_;
  }

  // --- project 3D world point to 2D image point --

  //: The generic camera interface. u represents image column, v image row.
  void
  project(const T x, const T y, const T z, T & u, T & v) const override;

  //: Project a world point onto the image (vnl interface)
  vnl_vector_fixed<T, 2>
  project(const vnl_vector_fixed<T, 3> & world_point) const;

  //: Project a world point onto the image (vgl interface)
  vgl_point_2d<T>
  project(vgl_point_3d<T> world_point) const;

  vpgl_region_selector<T> &
  reg_sel()
  {
    return region_selector_;
  }
  void
  set_adjustable_parameters(T adj_u, T adj_v)
  {
    adj_u_ = adj_u;
    adj_v_ = adj_v;
  }
  std::pair<T, T>
  adjustable_parameters() const
  {
    std::pair<T, T> ret(adj_u_, adj_v_);
    return ret;
  }

  size_t
  n_regions()
  {
    return region_selector_.tnis_;
  }

protected:
  T adj_u_; // image column adjustable parameter
  T adj_v_; // image row adjustable parameter
  vpgl_ground_domain<T> ground_domain_;
  vpgl_region_selector<T> region_selector_;
  std::vector<std::vector<vpgl_polycam<T>>> polycams_;
};


template <class T>
void
vpgl_region_selector<T>::select(T x, T y, T z, size_t & region_row, size_t & region_col) const
{
  // a single region so don't compute
  if (rnis_ == 1 && cnis_ == 1)
  {
    region_row = 1;
    region_col = 1;
    return;
  }
  std::vector<T> power_vector = { 1.0, x, y, z, x * x, x * y, x * z, y * y, y * z, z * z };
  double rd = 0.0, cd = 0.0;
  for (size_t i = 0; i < 10; ++i)
  {
    rd += power_vector[i] * row_coefs_[i];
    cd += power_vector[i] * col_coefs_[i];
  }
  double rrd = std::floor((rd - minr_) / rssiz_) + 1;
  double rcd = std::floor((cd - minc_) / cssiz_) + 1;
  if (rrd < 1.0)
    rrd = 1.0;
  if (rrd > rnis_)
    rrd = rnis_;
  if (rcd < 1.0)
    rcd = 1.0;
  if (rcd > cnis_)
    rcd = cnis_;
  region_row = rrd;
  region_col = rcd;
}


#define VPGL_RSM_CAMERA_INSTANTIATE(T) extern "please include vgl/vpgl_RSM_camera.hxx first"

#endif // vpgl_RSM_camera_h_
