// This is core/vpgl/vpgl_RSM_camera.h
#ifndef vpgl_RSM_camera_h_
#define vpgl_RSM_camera_h_
//:
// \file
// \brief A camera model based on the replacement sensor model
// \author Joseph Mundy
// \date October 2023
//
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
// ----------------------------------------
// Represent scale and offset transformations used in normalization
//
template <class T>
class vpgl_scale_offset
{
 public:
  vpgl_scale_offset() :
    scale_(1), offset_(0) {}
  vpgl_scale_offset(const T scale, const T offset) :
    scale_(scale), offset_(offset) {}

  //mutators/accessors
  void set_scale(const T scale) {scale_ = scale;}
  void set_offset(const T offset) {offset_ = offset;}
  T scale() const {return scale_;}
  T offset() const {return offset_;}

  // normalize a coordinate value
  T normalize(const T value) const
  {
    if (scale_==0)
      return 0;
    else
      return (value-offset_)/scale_;
  }

  // un-normalize a coordinate value
  T un_normalize(const T value) const
  {
    T temp = value*scale_;
    return temp + offset_;
  }
  //: Equality test
  inline bool operator==(vpgl_scale_offset<T> const &that) const
  { return this == &that ||
           (this->scale()==that.scale() &&
            this->offset() == that.offset() );
  }
 private:
  //members
  T scale_;
  T offset_;
};


//--------------------=== rational camera ===---------------------------
//
template <class T>
class vpgl_RSM_camera : public vpgl_camera<T>
{
public:
    //: enumeration for indexing coordinates
    enum coor_index { X_INDX = 0, Y_INDX, Z_INDX, U_INDX, V_INDX };
    //: enumeration for indexing polynomials
    enum poly_index { NEU_U = 0, DEN_U, NEU_V, DEN_V };

    //: default constructor
    vpgl_RSM_camera();

    //: Constructor from 4 coefficient vectors and 5 scale, offset pairs.
    vpgl_RSM_camera(std::vector<T> const& neu_u,
        std::vector<T> const& den_u,
        std::vector<T> const& neu_v,
        std::vector<T> const& den_v,
        const T x_scale, const T x_off,
        const T y_scale, const T y_off,
        const T z_scale, const T z_off,
        const T u_scale, const T u_off,
        const T v_scale, const T v_off
        );

    //: Constructor from 4 coefficient arrays and 5 scale, offset pairs.
    vpgl_RSM_camera(const double* neu_u,
        const double* den_u,
        const double* neu_v,
        const double* den_v,
        const T x_scale, const T x_off,
        const T y_scale, const T y_off,
        const T z_scale, const T z_off,
        const T u_scale, const T u_off,
        const T v_scale, const T v_off
        );

    //: Constructor with everything wrapped up in an array and vector.
    vpgl_RSM_camera(std::vector<std::vector<T> > const& RSM_coeffs,
        std::vector<vpgl_scale_offset<T> > const& scale_offsets
        );
#if 0
    //: Constructor with a coefficient matrix
    vpgl_RSM_camera(vnl_matrix_fixed<T, 4, 20> const& RSM_coeffs,
        std::vector<vpgl_scale_offset<T> > const& scale_offsets
        );
#endif
    ~vpgl_RSM_camera() override = default;

    std::string type_name() const override { return "vpgl_RSM_camera"; }

    //: Clone `this': creation of a new object and initialization
    // legal C++ because the return type is covariant with vpgl_camera<T>*
    vpgl_RSM_camera<T>* clone() const override;

    //: Equality test
    inline bool operator==(vpgl_RSM_camera<T> const& that) const
    {
        return this == &that ||
            ((this->coefficient_matrix() == that.coefficient_matrix()) &&
                (this->scale_offsets() == that.scale_offsets()));
    }

    // --- Mutators/Accessors ---

    //: set RSM polynomial coefficients
    void set_coefficients(
        std::vector<T> const& neu_u,
        std::vector<T> const& den_u,
        std::vector<T> const& neu_v,
        std::vector<T> const& den_v
        );

    void set_coefficients(
        const double* neu_u,
        const double* den_u,
        const double* neu_v,
        const double* den_v
        );

    void set_coefficients(
        std::vector<std::vector<T> > const& RSM_coeffs
        );
#if 0
    void set_coefficients(
        vnl_matrix_fixed<T, 4, 20> const& RSM_coeffs
        );
#endif
    //: get the RSM polynomial coefficients in a vnl matrix
    vnl_matrix_fixed<T, 4, 20> coefficient_matrix(
        
    ) const;

    //: get the RSM polynomial coefficients in std vector of vectors
    std::vector<std::vector<T> > coefficients(
        
    ) const;

    //: set all coordinate scale and offsets
    void set_scale_offsets(const T x_scale, const T x_off,
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

    // --- print & save camera ---

    //: print camera parameters
    virtual void print(
        std::ostream& s = std::cout
    ) const;

    //: save camera parameters to a file
    virtual bool save(
        std::string cam_path
        ) const;

    //: write PVL (parameter) to output stream
    virtual void write_pvl(std::ostream& s) const;

    // --- read camera ---

    //: read from PVL (parameter value language) file/stream
    bool read_pvl(std::string cam_path);
    virtual bool read_pvl(std::istream& istr);

    //: read from TXT file/stream
    bool read_txt(std::string cam_path);
    virtual bool read_txt(std::istream& istr);

protected:
    vnl_vector_fixed<T, 20>
        vpgl_RSM_camera<T>::power_vector(const T x, const T y, const T z) const;
    std::vector<vpgl_scale_offset<T> > scale_offsets_;
};
    //: Write to stream
    // \relatesalso vpgl_RSM_camera
    template <class T>
    std::ostream& operator<<(std::ostream& s, const vpgl_RSM_camera<T>& p);

    //: Read from stream
    // \relatesalso vpgl_RSM_camera
    template <class T>
    std::istream& operator>>(std::istream& is, vpgl_RSM_camera<T>& p);

    //: Creates a RSM camera from a PVL file
    // \relatesalso vpgl_RSM_camera
    template <class T>
    vpgl_RSM_camera<T>* read_RSM_camera(std::string cam_path);

    //: Creates a RSM camera from a PVL input stream
    // \relatesalso vpgl_RSM_camera
    template <class T>
    vpgl_RSM_camera<T>* read_RSM_camera(std::istream& istr);

    //: Creates a RSM camera from a TXT file
    // \relatesalso vpgl_RSM_camera
    template <class T>
    vpgl_RSM_camera<T>* read_RSM_camera_from_txt(std::string cam_path);

    //: Creates a RSM camera from a TXT input stream
    // \relatesalso vpgl_RSM_camera
    template <class T>
    vpgl_RSM_camera<T>* read_RSM_camera_from_txt(std::istream& istr);

#define VPGL_RSM_CAMERA_INSTANTIATE(T) extern "please include vgl/vpgl_RSM_camera.hxx first"


#endif // vpgl_RSM_camera_h_
