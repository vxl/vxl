// This is core/vpgl/vpgl_calibration_matrix.h
#ifndef vpgl_calibration_matrix_h_
#define vpgl_calibration_matrix_h_
//:
// \file
// \brief A class for the calibration matrix component of a perspective camera matrix.
// \author Thomas Pollard
// \date January 28, 2005
// \author Joseph Mundy, Matt Leotta, Vishal Jain
//
// \verbatim
//  Modifications
//   May 08, 2004  Ricardo Fabbri  Added binary I/O support
//   May 08, 2004  Ricardo Fabbri  Added == operator
// \endverbatim
//

#include <iostream>
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_point_2d.h>

// not used? #include <vcl_compiler.h>

//:  A class representing the "K" matrix of a perspective camera matrix as described in
//   Hartley and Zisserman, "Multiple View Geometry".
template <class T>
class vpgl_calibration_matrix
{
 public:
  //: Default constructor makes an identity matrix.
  vpgl_calibration_matrix();

  //: Destructor
  virtual ~vpgl_calibration_matrix() = default;

  //: Construct using all of the camera parameters.
  // Must satisfy the following requirements: x,y_scales must be > 0, focal_length must be not equal to 0.
  vpgl_calibration_matrix( T focal_length, const vgl_point_2d<T>& principal_point,
                           T x_scale = (T)1, T y_scale = (T)1, T skew = (T)0 );

  //: Construct from a right upper triangular matrix whose decomposition into the calibration components makes sense.
  //  The supplied matrix can be a scalar multiple of such a matrix.
  vpgl_calibration_matrix( const vnl_matrix_fixed<T,3,3>& K );

  //: Get the calibration matrix.
  vnl_matrix_fixed<T,3,3> get_matrix() const;

  //: Getters and setters for all of the parameters.
  void set_focal_length( T new_focal_length );
  void set_principal_point( const vgl_point_2d<T>& new_principal_point );
  void set_x_scale( T new_x_scale );
  void set_y_scale( T new_y_scale );
  void set_skew( T new_skew );

  T focal_length() const { return focal_length_; }
  vgl_point_2d<T> principal_point() const { return principal_point_; }
  T x_scale() const { return x_scale_; }
  T y_scale() const { return y_scale_; }
  T skew() const { return skew_; }

  //: Equality tests
  bool operator==(vpgl_calibration_matrix<T> const &that) const;
  bool operator!=(vpgl_calibration_matrix<T> const &that) const
    {return !(*this==that);}

  //: Maps to and from the focal plane
  vgl_point_2d<T> map_to_focal_plane(vgl_point_2d<T> const& p_image) const;

  vgl_point_2d<T> map_to_image(vgl_point_2d<T> const& p_focal_plane) const;

 protected:
  //: The following is a list of the parameters in the calibration matrix.
  T focal_length_;
  vgl_point_2d<T> principal_point_;
  T x_scale_, y_scale_, skew_;
};



#endif // vpgl_calibration_matrix_h_
