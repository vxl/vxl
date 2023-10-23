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
#include "vpgl_rational_camera.h"//for scale and offset class

//--------------------=== replacement sensor model (RSM)  camera ===-----------------------
//
template <class T>
class vpgl_RSM_camera : public vpgl_camera<T>
{
public:
  //: enumeration for indexing coordinates
  enum coor_index { X_INDX = 0, Y_INDX, Z_INDX, U_INDX, V_INDX };
  //: enumeration for indexing polynomials
  enum poly_index { NEU_U = 0, DEN_U, NEU_V, DEN_V };
  //: enumeration for computing polys
  enum poly_comp_index { P_NEU_U = 0, P_DEN_U, P_NEU_V, P_DEN_V};
  //: default constructor
  vpgl_RSM_camera();

  //: Constructor with everything wrapped up in an array and vector.
 vpgl_RSM_camera(std::vector<std::vector<int> >const& powers,
                 std::vector<std::vector<T> > const& coeffs,
                 std::vector<vpgl_scale_offset<T> > const& scale_offsets
                 ): powers_(powers), coeffs_(coeffs), scale_offsets_(scale_offsets){}
  
  ~vpgl_RSM_camera() override = default;

  std::string type_name() const override { return "vpgl_RSM_camera"; }

  //: Clone `this': creation of a new object and initialization
  // legal C++ because the return type is covariant with vpgl_camera<T>*
  vpgl_RSM_camera<T>* clone() const override;

    //: Equality test
  inline bool operator==(vpgl_RSM_camera<T> const& that) const
    {
      return this == &that ||
      ((this->coefficients() == that.coefficients()) && (this->powers() == that.powers()) &&
       (this->scale_offsets() == that.scale_offsets()));
      return true;
    }

    // --- Mutators/Accessors ---

    //: set RSM polynomial coefficients
    void set_coefficients(
        std::vector<T> const& neu_u,
        std::vector<T> const& den_u,
        std::vector<T> const& neu_v,
        std::vector<T> const& den_v
        );

    // In the order neu_u, den_u, neu_v, den_v
    void set_coefficients(std::vector<std::vector<T> > const& RSM_coeffs);

    //: get the RSM polynomial coefficients in std vector of vectors
    // In the order neu_u, den_u, neu_v, den_v
    std::vector<std::vector<T> > coefficients(
        
    ) const;

    //: set the maximum power of each of x, y, z
    //  for each of the four polynomials in the order,
    //  neu_u, den_u, neu_v, den_v
    void set_powers(std::vector<std::vector<int> >const& powers){
      powers_ = powers;
    }

    void set_powers(
        std::vector<int> const& neu_u_powers,
        std::vector<int> const& den_u_powers,
        std::vector<int> const& neu_v_powers,
        std::vector<int> const& den_v_powers
    );
    //: In the order neu_u, den_u, neu_v, den_v
    std::vector<std::vector<int> > powers() const {
        return powers_;
    }
    
    //: set all coordinate scale and offsets
    void set_scale_offsets(
        const T x_scale, const T x_off,
        const T y_scale, const T y_off,
        const T z_scale, const T z_off,
        const T u_scale, const T u_off,
        const T v_scale, const T v_off
    );

    void set_scale_offsets(std::vector<vpgl_scale_offset<T> > const& scale_offsets);
    
    //: get the scale and offsets in a vector
    std::vector<vpgl_scale_offset<T> > scale_offsets() const
    {
        return scale_offsets_;
    }

    //:set a specific scale value
    void set_scale(const coor_index coor_index, const T scale)
    {
        scale_offsets_[coor_index].set_scale(scale);
    }

    //:set a specific offset value
    void set_offset(const coor_index coor_index, const T offset)
    {
        scale_offsets_[coor_index].set_offset(offset);
    }

    //: get a specific scale value
    T scale(const coor_index coor_index) const
    {
        return scale_offsets_[coor_index].scale();
    }

    //: get a specific offset value
    T offset(const coor_index coor_index) const
    {
        return scale_offsets_[coor_index].offset();
    }

    //: get a specific scale_offset
    vpgl_scale_offset<T> scl_off(const coor_index coor_index) const
    {
        return scale_offsets_[coor_index];
    }

    // --- Often useful for adjusting the camera ---

    //:set u-v translation offset
    void set_image_offset(const T u_off, const T v_off)
    {
        scale_offsets_[U_INDX].set_offset(u_off);
        scale_offsets_[V_INDX].set_offset(v_off);
    }

    //:get u-v translation offset
    void image_offset(T& u_off, T& v_off) const
    {
        u_off = offset(U_INDX); v_off = offset(V_INDX);
    }

    //:set u-v scale
    void set_image_scale(const T u_scale, const T v_scale)
    {
        scale_offsets_[U_INDX].set_scale(u_scale);
        scale_offsets_[V_INDX].set_scale(v_scale);
    }

    //:get u-v  scale
    void image_scale(T& u_scale, T& v_scale)
    {
        u_scale = scale(U_INDX); v_scale = scale(V_INDX);
    }

    // --- project 3D world point to 2D image point --

    //: The generic camera interface. u represents image column, v image row.
    void project(const T x, const T y, const T z, T& u, T& v) const override;

    //: Project a world point onto the image (vnl interface)
    vnl_vector_fixed<T, 2> project(vnl_vector_fixed<T, 3> const& world_point) const;

    //: Project a world point onto the image (vgl interface)
    vgl_point_2d<T> project(vgl_point_3d<T> world_point) const;

protected:
    std::vector<std::vector<int> > powers_;
    std::vector<std::vector<double> > coeffs_;
    std::vector<vpgl_scale_offset<T> > scale_offsets_;
};

#define VPGL_RSM_CAMERA_INSTANTIATE(T) extern "please include vgl/vpgl_RSM_camera.hxx first"


#endif // vpgl_RSM_camera_h_
