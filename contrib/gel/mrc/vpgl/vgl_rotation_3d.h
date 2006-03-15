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

#include <vnl/vnl_fwd.h>
#include <vgl/vgl_fwd.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vcl_vector.h>


template <class T>
class vgl_rotation_3d
{
 public:
  // Constructors:-------------------------------------

  //: Construct from a quaternion.
  vgl_rotation_3d( const vnl_quaternion<T>& q );

  //: Construct from Euler angles.
  vgl_rotation_3d( const T& rx, const T& ry, const T& rz );

  //: Construct from a Rodriques vector.
  vgl_rotation_3d( const vnl_vector_fixed<T,3>& rvector );

  //: Construct from a 3x3 rotation matrix
  vgl_rotation_3d( const vnl_matrix_fixed<T,3,3>& matrix );

  //: Construct from a vgl_h_matrix_3d.
  vgl_rotation_3d( const vgl_h_matrix_3d<T>& h );


  // Conversions:--------------------------------------

  //: Output unit quaternion.
  vnl_quaternion<T> as_quaternion() const;

  //: Output Euler angles.
  //  The first element is the rotation about the x-axis
  //  The second element is the rotation about the y-axis
  //  The third element is the rotation about the z-axis
  //  The total rotation is a composition of these rotations in this order
  vnl_vector_fixed<T,3> as_euler_angles() const;

  //: Output Rodriques vector.
  //  The direction of this vector is the axis of rotation
  //  The length of this vector is the angle of rotation in radians
  vnl_vector_fixed<T,3> as_rodriques() const;

  //: Output vgl_h_matrix_3d.
  vgl_h_matrix_3d<T> as_h_matrix() const;

  //: Output the matrix representation of this rotation in 3x3 or 4x4 form.
  vnl_matrix_fixed<T,3,3> as_3matrix() const{
    return q_.rotation_matrix_transpose().transpose(); }
  vnl_matrix_fixed<T,4,4> as_4matrix() const{
    return q_.rotation_matrix_transpose_4().transpose(); }


  // Operations:----------------------------------------

  //: Composition of two rotations.
  vgl_rotation_3d<T> operator*( const vgl_rotation_3d<T>& first_rotation ) const{
    return vgl_rotation_3d<T>( q_*first_rotation.q_ ); }

  //: Composition of two rotations.
  vgl_rotation_3d<T>& operator*=( const vgl_rotation_3d<T>& first_rotation ){
    q_ = q_*first_rotation.q_; return *this;}

  //: Rotate a homogeneous point.
  vgl_homg_point_3d<T> rotate( const vgl_homg_point_3d<T>& p ) const;
  //: Rotate a homogeneous plane.
  vgl_homg_plane_3d<T> rotate( const vgl_homg_plane_3d<T>& p ) const;
  //: Rotate a homogeneous line.
  vgl_homg_line_3d_2_points<T> rotate( const vgl_homg_line_3d_2_points<T>& l ) const;
  //: Rotate a point.
  vgl_plane_3d<T> rotate( const vgl_point_3d<T>& p ) const;
  //: Rotate a plane.
  vgl_plane_3d<T> rotate( const vgl_plane_3d<T>& p ) const;
  //: Rotate a line.
  vgl_line_3d_2_points<T> rotate( const vgl_line_3d_2_points<T>& l ) const;
  //: Rotate a line segment.
  vgl_line_segment_3d<T> rotate( const vgl_line_segment_3d<T>& l ) const;
  //: Rotate a vector.
  vgl_vector_3d<T> rotate( const vgl_vector_3d<T>& v ) const;

 protected:
  //: The internal representation of the rotation is a quaternion.
  vnl_quaternion<T> q_;
};


// External methods for more efficient rotation of multiple objects
// ----------------------------------------------------------------

//: In-place rotation of a vector of homogeneous points
// \note This is more efficient than calling vgl_rotation_3d::rotate() on each
template <class T> inline
void vgl_rotate_3d(const vgl_rotation_3d<T>& rot, vcl_vector<vgl_homg_point_3d<T> >& pts)
{
  vnl_matrix_fixed<T,3,3> R = rot.as_3matrix();
  for (vcl_vector<vgl_homg_point_3d<T> >::iterator itr = pts.begin();
       itr != pts.end();  ++itr)
  {
     vgl_homg_point_3d<T>& p = *itr;
     p.set(R[0][0]*p.x()+R[0][1]*p.y()+R[0][2]*p.z(),
           R[1][0]*p.x()+R[1][1]*p.y()+R[1][2]*p.z(),
           R[2][0]*p.x()+R[2][1]*p.y()+R[2][2]*p.z(), p.w());
  }
}


//: In-place rotation of a vector of points
// \note This is more efficient than calling vgl_rotation_3d::rotate() on each
template <class T> inline
void vgl_rotate_3d(const vgl_rotation_3d<T>& rot, vcl_vector<vgl_point_3d<T> >& pts)
{
  vnl_matrix_fixed<T,3,3> R = rot.as_3matrix();
  for (vcl_vector<vgl_point_3d<T> >::iterator itr = pts.begin();
       itr != pts.end();  ++itr)
  {
     vgl_point_3d<T>& p = *itr;
     p.set(R[0][0]*p.x()+R[0][1]*p.y()+R[0][2]*p.z(),
           R[1][0]*p.x()+R[1][1]*p.y()+R[1][2]*p.z(),
           R[2][0]*p.x()+R[2][1]*p.y()+R[2][2]*p.z());
  }
}

#endif // vgl_rotation_3d_h_
