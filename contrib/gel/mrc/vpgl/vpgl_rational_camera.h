// This is gel/mrc/vpgl/vpgl_rational_camera.h
#ifndef vpgl_rational_camera_h_
#define vpgl_rational_camera_h_
//:
// \file
// \brief A camera model based on ratios of cubic polynomials
// \author Joseph Mundy
// \date Oct 2006
//
//
//  A camera that projects 3-d world points according to ratios of
//  cubic polynomials. That is,

//           neu_u(X,Y,Z)      neu_v(X,Y,Z)
//       u = ------------  v = ------------
//           den_u(X,Y,Z)      den_v(X,Y,X)
//
//  where u is the image column index and v is the image row index.
//
//  neu_u(X,Y,Z),den_u(X,Y,Z), neu_v(X,Y,Z), den_v(X,Y,Z) are
//  cubic polynomials in three variables and there are 20 coefficients each,
//  e.g.,
//
//  R(X,Y,Z) = a300 x^3 + a210 x^2y + a201 x^2z + a200 x^2 + ... + a001z + a000
//
//  The normal ordering of multi-variate polynomial coefficients is as
//  shown above, the highest powers in x followed by highest powers in y,
//  followed by powers in z. The full monomial sequence is:
//   0   1    2    3   4    5  6   7   8  9 10  11   12   13  14 15 16 17 18 19
//  x^3 x^2y x^2z x^2 xy^2 xyz xy xz^2 xz x y^3 y^2z y^2 yz^2 yz y z^3 z^2 z 1
//  The highest powers are in the lowest index of the coefficient vector.
//
//  Polynomial calculations are often ill-conditioned if the variables are not
//  normalized. Common practice is to normalize all variables to the
//  range [-1, 1]. This normalization requires 10 additional offset and scale
//  parameters for a total of 90 parameters.
//
//  The scale and offset transformation is applied to (X,Y,Z)
//  before appying the polynomial mapping. The resulting (u,v) is normalized
//  and must be mapped back (un-normalized) to the image coordinate
//  system to obtain the actual projection.
//
//  In order to facilitate the scale offset transformation process, a helper
//  class, scale_offset, is defined to process the forward and reverse
//  normalizations.
//
#include <vgl/vgl_fwd.h>
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vpgl/vpgl_camera.h>

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
    return (value-offset_)/scale_;
  }

  // un-normalize a coordinate value
  T un_normalize(const T value) const
  {
    T temp = value*scale_;
    return temp + offset_;
  }

 private:
  //members
  T scale_;
  T offset_;
};

//
//--------------------=== rational camera ===---------------------------
//
template <class T>
class vpgl_rational_camera : public vpgl_camera<T>
{
 public:
  //: enumeration for indexing coordinates
  enum coor_index{X_INDX = 0, Y_INDX, Z_INDX, U_INDX, V_INDX};
  //: enumeration for indexing polynomials
  enum poly_index{NEU_U = 0, DEN_U, NEU_V, DEN_V};

  //: default constructor
  vpgl_rational_camera();
  //: Constructor from 4 coefficient vectors and 5 scale, offset pairs.
  vpgl_rational_camera(vcl_vector<T> const& neu_u,
                       vcl_vector<T> const& den_u,
                       vcl_vector<T> const& neu_v,
                       vcl_vector<T> const& den_v,
                       const T x_scale, const T x_off,
                       const T y_scale, const T y_off,
                       const T z_scale, const T z_off,
                       const T u_scale, const T u_off,
                       const T v_scale, const T v_off
                      );

  //: Constructor from 4 coefficient arrays and 5 scale, offset pairs.
  vpgl_rational_camera(const double*  neu_u,
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
  vpgl_rational_camera(vcl_vector<vcl_vector<T> > const& rational_coeffs,
                       vcl_vector<vpgl_scale_offset<T> > const& scale_offsets);

  //: Constructor with a coefficient matrix
  vpgl_rational_camera(vnl_matrix_fixed<T, 4, 20> const& rational_coeffs,
                       vcl_vector<vpgl_scale_offset<T> > const& scale_offsets)
    : rational_coeffs_(rational_coeffs), scale_offsets_(scale_offsets) {}

  virtual ~vpgl_rational_camera() {}

  virtual vcl_string type_name() const { return "vpgl_rational_camera"; }

  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  virtual vpgl_rational_camera<T>* clone(void) const;

        // Mutators/Accessors

  //: set rational polynomial coefficients
  void set_coefficients(vcl_vector<vcl_vector<T> > const& rational_coeffs);
  void set_coefficients(vnl_matrix_fixed<T, 4, 20> const& rational_coeffs)
    {rational_coeffs_ = rational_coeffs;}
  //: set coordinate scale and offsets
  void set_scale_offsets(vcl_vector<vpgl_scale_offset<T> > const& scale_offsets);
  //: get the rational polynomial coefficients in a vnl matrix
  vnl_matrix_fixed<T, 4, 20> coefficient_matrix() const
    {return rational_coeffs_;}
  //: get the rational polynomial coefficients in a vcl array
  vcl_vector<vcl_vector<T> > coefficients() const;
  //: get the scale and offsets in a vector
  vcl_vector<vpgl_scale_offset<T> > scale_offsets() const
    {return scale_offsets_;}
  //:set a specific scale value
  void set_scale(const coor_index coor_index, const T scale)
    {scale_offsets_[coor_index].set_scale(scale);}
  //:set a specific scale value
  void set_offset(const coor_index coor_index, const T offset)
    {scale_offsets_[coor_index].set_offset(offset);}
  //: get a specific scale value
  T scale(const coor_index coor_index) const
    {return scale_offsets_[coor_index].scale();}
  //: get a specific offset value
  T offset(const coor_index coor_index) const
    {return scale_offsets_[coor_index].offset();}
  //: get a specific scale_offset
  vpgl_scale_offset<T> scl_off(const coor_index coor_index) const
    {return scale_offsets_[coor_index];}

        // Often useful for adjusting the camera

  //:set u-v translation offset
  void set_image_offset(const T u_off, const T v_off)
  { scale_offsets_[U_INDX].set_offset(u_off);
    scale_offsets_[V_INDX].set_offset(v_off); }

  //:get u-v translation offset
  void image_offset(T& u_off, T& v_off)
    {u_off = offset(U_INDX); v_off = offset(V_INDX);}

  //: The generic camera interface. u represents image column, v image row.
  virtual void project(const T x, const T y, const T z, T& u, T& v) const;

        // Interface for vnl

  //: Project a world point onto the image
  virtual vnl_vector_fixed<T, 2> project(vnl_vector_fixed<T, 3> const& world_point) const;

        // Interface for vgl

  //: Project a world point onto the image
  virtual vgl_point_2d<T> project(vgl_point_3d<T> world_point) const;

  //: print the camera parameters
  virtual void print(vcl_ostream& s = vcl_cout) const;

  bool save(vcl_string cam_path);

  // binary IO

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

 protected:
  // utilities
  vnl_vector_fixed<T, 20> power_vector(const T x, const T y, const T z) const;
  // members
  vnl_matrix_fixed<T, 4, 20> rational_coeffs_;
  vcl_vector<vpgl_scale_offset<T> > scale_offsets_;
};

//: Write to stream
// \relates vpgl_rational_camera
template <class T>
vcl_ostream& operator<<(vcl_ostream& s, const vpgl_rational_camera<T>& p);

//: Read from stream
// \relates vpgl_rational_camera
template <class T>
vcl_istream& operator>>(vcl_istream& is, vpgl_rational_camera<T>& p);

//: Creates a rational camera from a file
// \relates vpgl_rational_camera
template <class T> 
vpgl_rational_camera<T>* read_rational_camera(vcl_string cam_path);

//: Creates a rational camera from a stream
// \relates vpgl_rational_camera
template <class T> 
vpgl_rational_camera<T>* read_rational_camera(vcl_istream& istr);

#define VPGL_RATIONAL_CAMERA_INSTANTIATE(T) extern "please include vgl/vpgl_rational_camera.txx first"


#endif // vpgl_rational_camera_h_

