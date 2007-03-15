// This is gel/mrc/vpgl/vgl_rotation_3d.h
#ifndef vgl_rotation_3d_h_
#define vgl_rotation_3d_h_
//:
// \file
// \brief A class representing a 3d rotation.
// \author Thomas Pollard
// \date 03/13/05
//
// This is a class for storing and doing conversions with 3d rotation transforms specified
// by any of the following parameters: quaternions, Euler angles, Rodriques vector, an
// orthonormal 3x3 matrix (with determinant = 1), or a vgl_h_matrix of proper form.
//
// \verbatim
//  Modifications
//   M. J. Leotta   3/14/07   Moved from VPGL and implemented member functions
// \endverbatim

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_quaternion.h>
#include <vgl/vgl_fwd.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vcl_vector.h>



template <class T>
class vgl_rotation_3d
{
 public:
  // Constructors:-------------------------------------

  //: Construct the identity rotation
  vgl_rotation_3d() : q_(0,0,0,1) {}

  //: Construct from a quaternion.
  vgl_rotation_3d( const vnl_quaternion<T>& q ) : q_(q) { q_.normalize(); }

  //: Construct from Euler angles.
  vgl_rotation_3d( const T& rx, const T& ry, const T& rz ) : q_(rx,ry,rz) {}

  //: Construct from a Rodrigues vector.
  explicit vgl_rotation_3d( const vnl_vector_fixed<T,3>& rvector )
    : q_(rvector/rvector.magnitude(),rvector.magnitude()) {}

  //: Construct from a 3x3 rotation matrix
  explicit vgl_rotation_3d( const vnl_matrix_fixed<T,3,3>& matrix )
    : q_(matrix.transpose()) {}

  //: Construct from a vgl_h_matrix_3d.
  explicit vgl_rotation_3d( const vgl_h_matrix_3d<T>& h )
    : q_(h.get_upper_3x3_matrix().transpose()) { assert(h.is_rotation()); }


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
    return q_.axis()*q_.angle();
  }

  //: Output vgl_h_matrix_3d.
  vgl_h_matrix_3d<T> as_h_matrix() const
  {
    return vgl_h_matrix_3d<T>(this->as_4matrix());
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
  vnl_vector_fixed<T,3> axis() const
  {
    return q_.axis();
  }

  //: Returns the magnitude of the angle of rotation 
  T angle() const
  {
    return q_.angle();
  }

  // Operations:----------------------------------------

  //: Make the rotation the identity (i.e. no rotation)
  void set_identity() { q_[0]=0; q_[1]=0; q_[2]=0; q_[3]=1; }

  //: The inverse rotation
  vgl_rotation_3d<T> inverse() const { return vgl_rotation_3d<T>(q_.conjugate()); }

  //: Composition of two rotations.
  vgl_rotation_3d<T> operator*( const vgl_rotation_3d<T>& first_rotation ) const
  {
    return vgl_rotation_3d<T>( q_*first_rotation.q_ );
  }

  //: Composition of two rotations.
  vgl_rotation_3d<T>& operator*=( const vgl_rotation_3d<T>& first_rotation )
  {
    q_ *= first_rotation.q_;
    return *this;
  }

  //: Rotate a homogeneous point.
  vgl_homg_point_3d<T> operator*( const vgl_homg_point_3d<T>& p ) const
  {
    vnl_vector_fixed<T,3> rp = q_.rotate(vnl_vector_fixed<T,3>(p.x(),p.y(),p.z()));
    return vgl_homg_point_3d<T>(rp[0],rp[1],rp[2],p.w());
  }

  //: Rotate a homogeneous plane.
  vgl_homg_plane_3d<T> operator*( const vgl_homg_plane_3d<T>& p ) const
  {
    vnl_vector_fixed<T,3> rp = q_.rotate(vnl_vector_fixed<T,3>(p.a(),p.b(),p.c()));
    return vgl_homg_plane_3d<T>(rp[0],rp[1],rp[2],p.d());
  }

  //: Rotate a homogeneous line.
  vgl_homg_line_3d_2_points<T> operator*( const vgl_homg_line_3d_2_points<T>& l ) const
  {
    return vgl_homg_line_3d_2_points<T>(this->operator*(l.point_finite()),
                                        this->operator*(l.point_infinite()));
  }

  //: Rotate a point.
  vgl_point_3d<T> operator*( const vgl_point_3d<T>& p ) const
  {
    vnl_vector_fixed<T,3> rp = q_.rotate(vnl_vector_fixed<T,3>(p.x(),p.y(),p.z()));
    return vgl_point_3d<T>(rp[0],rp[1],rp[2]);
  }

  //: Rotate a plane.
  vgl_plane_3d<T> operator*( const vgl_plane_3d<T>& p ) const
  {
    vnl_vector_fixed<T,3> rp = q_.rotate(vnl_vector_fixed<T,3>(p.a(),p.b(),p.c()));
    return vgl_plane_3d<T>(rp[0],rp[1],rp[2],p.d());
  }

  //: Rotate a line.
  vgl_line_3d_2_points<T> operator*( const vgl_line_3d_2_points<T>& l ) const
  {
    return vgl_line_3d_2_points<T>(this->operator*(l.point1()),
                                   this->operator*(l.point2()));
  }

  //: Rotate a line segment.
  vgl_line_segment_3d<T> operator*( const vgl_line_segment_3d<T>& l ) const
  {
    return vgl_line_segment_3d<T>(this->operator*(l.point1()),
                                  this->operator*(l.point2()));
  }

  //: Rotate a vector.
  vgl_vector_3d<T> operator*( const vgl_vector_3d<T>& v ) const
  {
    vnl_vector_fixed<T,3> rv = q_.rotate(vnl_vector_fixed<T,3>(v.x(),v.y(),v.z()));
    return vgl_vector_3d<T>(rv[0],rv[1],rv[2]);
  }

 protected:
  //: The internal representation of the rotation is a quaternion.
  vnl_quaternion<T> q_;
};


// External methods for stream I/O
// ----------------------------------------------------------------

template <class T>
vcl_ostream& operator<<(vcl_ostream& s, vgl_rotation_3d<T> const& R)
{
  return s << R.as_quaternion();
}

// External methods for more efficient rotation of multiple objects
// ----------------------------------------------------------------

//: In-place rotation of a vector of homogeneous points
// \note This is more efficient than calling vgl_rotation_3d::rotate() on each
template <class T> inline
void vgl_rotate_3d(const vgl_rotation_3d<T>& rot, vcl_vector<vgl_homg_point_3d<T> >& pts)
{
  vnl_matrix_fixed<T,3,3> R = rot.as_3matrix();
  for (typename vcl_vector<vgl_homg_point_3d<T> >::iterator itr = pts.begin();
       itr != pts.end();  ++itr)
  {
     vgl_homg_point_3d<T>& p = *itr;
     p.set(R[0][0]*p.x()+R[0][1]*p.y()+R[0][2]*p.z(),
           R[1][0]*p.x()+R[1][1]*p.y()+R[1][2]*p.z(),
           R[2][0]*p.x()+R[2][1]*p.y()+R[2][2]*p.z(), p.w());
  }
}


#endif // vgl_rotation_3d_h_
