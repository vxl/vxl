// This is core/vgl/algo/vgl_rotation_3d.h
#ifndef vgl_rotation_3d_h_
#define vgl_rotation_3d_h_
//:
// \file
// \brief A class representing a 3d rotation.
// \author Thomas Pollard
// \date 2005-03-13
//
// This is a class for storing and doing conversions with 3d rotation transforms
// specified by any of the following parameters: quaternions, Euler angles,
// Rodrigues vector, an orthonormal 3x3 matrix (with determinant = 1), or a
// vgl_h_matrix of proper form.
//
// \verbatim
//  Modifications
//   M. J. Leotta   2007-03-14 Moved from VPGL and implemented member functions
//   Peter Vanroose 2009-06-11 Robustified the 2-vector constructors: input no longer needs to be unit-norm
//   Peter Vanroose 2010-10-24 mutators and setters now return *this
// \endverbatim

#include <cmath>
#include <vector>
#include <iostream>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_cross.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_fwd.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vgl/vgl_tolerance.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
class vgl_rotation_3d
{
 public:
  // Constructors:-------------------------------------

  //: Construct the identity rotation
  vgl_rotation_3d() : q_(0,0,0,1) {}

  //: Construct from a quaternion.
  vgl_rotation_3d( vnl_quaternion<T> const& q ) : q_(q) { q_.normalize(); }

  //: Construct from Euler angles.
  vgl_rotation_3d( T const& rx, T const& ry, T const& rz ) : q_(rx,ry,rz) {}

  //: Construct from a Rodrigues vector.
  explicit vgl_rotation_3d( vnl_vector_fixed<T,3> const& rvector )
  {
    T mag = rvector.magnitude();
    if (mag > T(0))
      q_ = vnl_quaternion<T>(rvector/mag,mag);
    else // identity rotation is a special case
      q_ = vnl_quaternion<T>(0,0,0,1);
  }

  //: Construct from a Rodrigues vector.
  vgl_rotation_3d& operator=( vnl_vector_fixed<T,3> const& rvector )
  {
    T mag = rvector.magnitude();
    if (mag > T(0))
      q_ = vnl_quaternion<T>(rvector/mag,mag);
    else // identity rotation is a special case
      q_ = vnl_quaternion<T>(0,0,0,1);
    return *this;
  }

  //: Construct from a 3x3 rotation matrix
  explicit vgl_rotation_3d( vnl_matrix_fixed<T,3,3> const& matrix )
    : q_(matrix.transpose()) {}

  //: Construct from a 3x3 rotation matrix
  vgl_rotation_3d& operator=( vnl_matrix_fixed<T,3,3> const& matrix )
  { q_ = matrix.transpose(); return *this; }

  //: Construct from a vgl_h_matrix_3d.
  explicit vgl_rotation_3d( vgl_h_matrix_3d<T> const& h )
    : q_(h.get_upper_3x3_matrix().transpose()) { assert(h.is_rotation()); }

  //: Construct from a vgl_h_matrix_3d.
  vgl_rotation_3d& operator=( vgl_h_matrix_3d<T> const& h )
  { assert(h.is_rotation()); q_ = h.get_upper_3x3_matrix().transpose(); return *this; }

  //: Construct to rotate (direction of) vector a to vector b
  //  The input vectors need not be of unit length
  vgl_rotation_3d(vnl_vector_fixed<T,3> const& a,
                  vnl_vector_fixed<T,3> const& b)
  {
    vnl_vector_fixed<T,3> c = vnl_cross_3d(a, b);
    double aa = 0.0; if (dot_product(a, b) < 0) { aa = vnl_math::pi; c=-c; }
    double cmag = static_cast<double>(c.magnitude())
                / static_cast<double>(a.magnitude())
                / static_cast<double>(b.magnitude());
    // cross product of unit vectors is at most a unit vector:
    if (cmag>1.0) cmag=1.0;
    // if the vectors have the same direction, then set to identity rotation:
    if (cmag<vgl_tolerance<double>::position)
    {
      if (aa!=vnl_math::pi) {
        q_ = vnl_quaternion<T>(0, 0, 0, 1);
        return;
      }
      else { // rotation axis not defined for rotation of pi
        // construct a vector v along the min component axis of a
        double ax = std::fabs(static_cast<double>(a[0]));
        double ay = std::fabs(static_cast<double>(a[1]));
        double az = std::fabs(static_cast<double>(a[2]));
        vnl_vector_fixed<T,3> v(T(0));
        double amin = ax; v[0]=T(1);
        if (ay<amin) { amin = ay; v[0]=T(0); v[1]=T(1); }
        if (az<amin) { v[0]=T(0); v[1]=T(0); v[2]=T(1); }
        // define the pi rotation axis perpendicular to both v and a
        vnl_vector_fixed<T,3> pi_axis = vnl_cross_3d(v, a);
        q_ = vnl_quaternion<T>(pi_axis/pi_axis.magnitude(), aa);
        return;
      }
    }
    double angl = std::asin(cmag)+aa;
    q_ = vnl_quaternion<T>(c/c.magnitude(), angl);
  }

  //: Construct to rotate (direction of) vector a to vector b
  //  The input vectors need not be of unit length
  vgl_rotation_3d(vgl_vector_3d<T> const& a,
                  vgl_vector_3d<T> const& b)
  {
    vnl_vector_fixed<T,3> na(a.x(), a.y(), a.z());
    vnl_vector_fixed<T,3> nb(b.x(), b.y(), b.z());
    *this = vgl_rotation_3d<T>(na, nb);
  }

  // Conversions:--------------------------------------

  //: Output unit quaternion.
  vnl_quaternion<T> as_quaternion() const
  {
    return q_;
  }

  //: Output Euler angles.
  //  The first element is the rotation about the x-axis
  //  The second element is the rotation about the y-axis
  //  The third element is the rotation about the z-axis
  //  The total rotation is a composition of these rotations in this order
  vnl_vector_fixed<T,3> as_euler_angles() const
  {
    return q_.rotation_euler_angles();
  }

  //: Output Rodrigues vector.
  //  The direction of this vector is the axis of rotation
  //  The length of this vector is the angle of rotation in radians
  vnl_vector_fixed<T,3> as_rodrigues() const
  {
    double ang = q_.angle();
    if (ang == 0.0)
      return vnl_vector_fixed<T,3>(T(0));
    return q_.axis()*T(ang);
  }

  //: Output the matrix representation of this rotation in 3x3 form.
  vnl_matrix_fixed<T,3,3> as_matrix() const
  {
    return q_.rotation_matrix_transpose().transpose();
  }

  //: Output the matrix representation of this rotation in 4x4 form.
  vgl_h_matrix_3d<T> as_h_matrix_3d() const
  {
    return vgl_h_matrix_3d<T>(q_.rotation_matrix_transpose_4().transpose());
  }

  //: Returns the axis of rotation (unit vector)
  vnl_vector_fixed<T,3> axis() const { return q_.axis(); }

  //: Returns the angle of rotation on the range [ 0  360 ] in radians
  double angle() const { return q_.angle(); }

  // Operations:----------------------------------------

  //: Make the rotation the identity (i.e. no rotation)
  vgl_rotation_3d& set_identity() { q_[0]=0; q_[1]=0; q_[2]=0; q_[3]=1; return *this; }

  //: The inverse rotation
  vgl_rotation_3d<T> inverse() const { return vgl_rotation_3d<T>(q_.conjugate()); }

  //: The transpose or conjugate of the rotation
  vgl_rotation_3d<T> transpose() const { return this->inverse(); }

  //: Composition of two rotations.
  vgl_rotation_3d<T> operator*( vgl_rotation_3d<T> const& first_rotation ) const
  {
    return vgl_rotation_3d<T>( q_*first_rotation.q_ );
  }

  //: Rotate a homogeneous point.
  vgl_homg_point_3d<T> operator*( vgl_homg_point_3d<T> const& p ) const
  {
    vnl_vector_fixed<T,3> rp = q_.rotate(vnl_vector_fixed<T,3>(p.x(),p.y(),p.z()));
    return vgl_homg_point_3d<T>(rp[0],rp[1],rp[2],p.w());
  }

  //: Rotate a homogeneous plane.
  vgl_homg_plane_3d<T> operator*( vgl_homg_plane_3d<T> const& p ) const
  {
    vnl_vector_fixed<T,3> rp = q_.rotate(vnl_vector_fixed<T,3>(p.a(),p.b(),p.c()));
    return vgl_homg_plane_3d<T>(rp[0],rp[1],rp[2],p.d());
  }

  //: Rotate a homogeneous line.
  vgl_homg_line_3d_2_points<T> operator*( vgl_homg_line_3d_2_points<T> const& l ) const
  {
    return vgl_homg_line_3d_2_points<T>(this->operator*(l.point_finite()),
                                        this->operator*(l.point_infinite()));
  }

  //: Rotate a point.
  vgl_point_3d<T> operator*( vgl_point_3d<T> const& p ) const
  {
    vnl_vector_fixed<T,3> rp = q_.rotate(vnl_vector_fixed<T,3>(p.x(),p.y(),p.z()));
    return vgl_point_3d<T>(rp[0],rp[1],rp[2]);
  }

  //: Rotate a plane.
  vgl_plane_3d<T> operator*( vgl_plane_3d<T> const& p ) const
  {
    vnl_vector_fixed<T,3> rp = q_.rotate(vnl_vector_fixed<T,3>(p.a(),p.b(),p.c()));
    return vgl_plane_3d<T>(rp[0],rp[1],rp[2],p.d());
  }

  //: Rotate a line.
  vgl_line_3d_2_points<T> operator*( vgl_line_3d_2_points<T> const& l ) const
  {
    return vgl_line_3d_2_points<T>(this->operator*(l.point1()),
                                   this->operator*(l.point2()));
  }

  //: Rotate a line segment.
  vgl_line_segment_3d<T> operator*( vgl_line_segment_3d<T> const& l ) const
  {
    return vgl_line_segment_3d<T>(this->operator*(l.point1()),
                                  this->operator*(l.point2()));
  }

  //: Rotate a vgl vector.
  vgl_vector_3d<T> operator*( vgl_vector_3d<T> const& v ) const
  {
    vnl_vector_fixed<T,3> rv = q_.rotate(vnl_vector_fixed<T,3>(v.x(),v.y(),v.z()));
    return vgl_vector_3d<T>(rv[0],rv[1],rv[2]);
  }

  //: Rotate a vnl vector.
  vnl_vector_fixed<T, 3> operator*( vnl_vector_fixed<T,3> const& v ) const
  {
    return q_.rotate(v);
  }

  //: comparison operator
  bool operator==(vgl_rotation_3d<T> const& r) const { return q_ == r.as_quaternion(); }

 protected:
  //: The internal representation of the rotation is a quaternion.
  vnl_quaternion<T> q_;
};


// External methods for stream I/O
// ----------------------------------------------------------------

template <class T>
std::istream& operator>>(std::istream& s, vgl_rotation_3d<T> &R)
{
  vnl_quaternion<T> q;
  s >> q;
  R = vgl_rotation_3d<T>(q);
  return s;
}

template <class T>
std::ostream& operator<<(std::ostream& s, vgl_rotation_3d<T> const& R)
{
  return s << R.as_quaternion();
}

// External methods for more efficient rotation of multiple objects
// ----------------------------------------------------------------

//: In-place rotation of a vector of homogeneous points
// \note This is more efficient than calling vgl_rotation_3d::rotate() on each
template <class T> inline
void vgl_rotate_3d(vgl_rotation_3d<T> const& rot, std::vector<vgl_homg_point_3d<T> >& pts)
{
  vnl_matrix_fixed<T,3,3> R = rot.as_3matrix();
  for (typename std::vector<vgl_homg_point_3d<T> >::iterator itr = pts.begin();
       itr != pts.end();  ++itr)
  {
    vgl_homg_point_3d<T>& p = *itr;
    p.set(R[0][0]*p.x()+R[0][1]*p.y()+R[0][2]*p.z(),
          R[1][0]*p.x()+R[1][1]*p.y()+R[1][2]*p.z(),
          R[2][0]*p.x()+R[2][1]*p.y()+R[2][2]*p.z(), p.w());
  }
}

#endif // vgl_rotation_3d_h_
