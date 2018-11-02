// This is core/vpgl/vpgl_proj_camera.h
#ifndef vpgl_proj_camera_h_
#define vpgl_proj_camera_h_
//:
// \file
// \brief A camera model using the standard 3x4 matrix representation.
// \author Thomas Pollard
// \date January 28, 2005
// \author Joseph Mundy, Matt Leotta, Vishal Jain
//
// \verbatim
//  Modifications
//  May 6, 2005  Ricardo Fabbri   Added binary I/O
//  March 14, 2010 J.L. Mundy made some methods virtual to handle affine case
// \endverbatim
//
// This is the most general camera class based around the 3x4 matrix camera model.
// In reality the 3x4 matrix should be rank 3, but this is only checked when an action
// needing an SVD decomposition is called, and only gives a warning.
//
// Once the camera is constructed, the camera matrix can only be accessed through
// the "get_matrix" and "set_matrix" functions. These are also the only ways for
// subclasses to access the matrix, as the automatic SVD handling is done in them.
//
// Some camera operations require an SVD decomposition of the camera matrix.  When
// such a function is first called, an SVD is automatically computed and cached for
// all future calls.  When the camera matrix is changed by "set_matrix", the cached SVD
// is automatically nulled and will only be recomputed when another function that
// needs it is called.  The SVD can be viewed at any time via the "svd" function.
//
// Only elementary methods on the camera are included in the class itself.  In addition,
// there several external functions at the end of the file for important camera operations
// deemed too specialized to be included in the vpgl_proj_camera class itself.  Some
// functions lifted from vgl_p_matrix.h.
//
// NOTE FOR DEVELOPERS:  If you write any member functions that change the
// underlying matrix P_ you should call set_matrix to change it, rather than
// changing P_ itself.  The automatic SVD caching will be screwed up otherwise.

#include <iosfwd>
#include <vnl/vnl_fwd.h>
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_line_segment_3d.h>
#include <vgl/vgl_infinite_line_3d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif



#include "vpgl_camera.h"

template <class T>
class vpgl_perspective_camera;


template <class T>
class vpgl_proj_camera : public vpgl_camera<T>
{
 public:
  // ----------------- Constructors:----------------------

  //: Default constructor makes an identity camera.
  vpgl_proj_camera();

  //: Construct from a vnl_matrix.
  vpgl_proj_camera( const vnl_matrix_fixed<T,3,4>& camera_matrix );

  //: Construct from an array.  The array should be in the order row1, row2, row3.
  vpgl_proj_camera( const T* camera_matrix );

  //: Copy constructor.
  vpgl_proj_camera( const vpgl_proj_camera& cam );

  std::string type_name() const override { return "vpgl_proj_camera"; }

  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  virtual vpgl_proj_camera<T>* clone(void) const;

  //: Assignment.
  const vpgl_proj_camera<T>& operator=( const vpgl_proj_camera& cam );

  ~vpgl_proj_camera() override;

  //: Equality test
  inline bool operator==(vpgl_proj_camera<T> const &that) const
  { return this == &that || this->get_matrix()==that.get_matrix(); }

  // ----------------- Projections and Backprojections:------------------------

  //: Projection from base class
  void project(const T x, const T y, const T z, T& u, T& v) const override;

  //: Project a point in world coordinates onto the image plane.
  virtual vgl_homg_point_2d<T> project( const vgl_homg_point_3d<T>& world_point ) const;

  //: Non-homogeneous version of the above.
  vgl_homg_point_2d<T> project( const vgl_point_3d<T>& world_point ) const {
    return project( vgl_homg_point_3d<T>( world_point ) ); }

  //: A shortcut to the above function.
  vgl_homg_point_2d<T> operator()( const vgl_homg_point_3d<T>& world_point ) const {
    return this->project( world_point ); }

  //: Project a line in the world onto a line in the image plane.
  vgl_line_segment_2d<T> project( const vgl_line_segment_3d<T>& world_line ) const;

  //: Standard () forward projection operator
  vgl_line_segment_2d<T> operator()( const vgl_line_segment_3d<T>& world_line ) const
  { return project( world_line ); }

  //: Project an infinite line in the world onto an infinite line in the image plane.
  vgl_line_2d<T> project( const vgl_infinite_line_3d<T>& world_line ) const;

  //: Standard () forward projection operator
  vgl_line_2d<T> operator()( const vgl_infinite_line_3d<T>& world_line ) const
  { return project( world_line ); }

  //: Find the 3d ray that goes through the camera center and the provided image point.
  virtual vgl_ray_3d<T> backproject_ray( const vgl_homg_point_2d<T>& image_point ) const;

  //: Find the 3d ray that goes through the camera center and the provided image point.
  virtual vgl_homg_line_3d_2_points<T> backproject( const vgl_homg_point_2d<T>& image_point ) const;

  //: Find the 3d plane that contains the camera center and the provided line in the image plane.
  vgl_homg_plane_3d<T> backproject( const vgl_homg_line_2d<T>& image_line ) const;

  // --------------------- Misc Camera Functions:-------------------

  //: Find the 3d coordinates of the center of the camera.
  virtual vgl_homg_point_3d<T> camera_center() const;

  //: Find the world plane parallel to the image plane intersecting the camera center.
  virtual  vgl_homg_plane_3d<T> principal_plane() const{ return vgl_homg_plane_3d<T>( P_[2] ); }

  //: Find the image coordinates of the vanishing points of the world coordinate axes.
  vgl_homg_point_2d<T> x_vanishing_point() const{ return vgl_homg_point_2d<T>( P_(0,0), P_(1,0), P_(2,0) ); }
  vgl_homg_point_2d<T> y_vanishing_point() const{ return vgl_homg_point_2d<T>( P_(0,1), P_(1,1), P_(2,1) ); }
  vgl_homg_point_2d<T> z_vanishing_point() const{ return vgl_homg_point_2d<T>( P_(0,2), P_(1,2), P_(2,2) ); }

  // --------------------- Getters and Setters:---------------------

  //: Return a copy of the camera matrix.
  const vnl_matrix_fixed<T,3,4>& get_matrix() const{ return P_; }

  //: Get a copy of the svd of the get_matrix.
  // The svd is cached when first computed and automatically recomputed when the matrix is changed.
  vnl_svd<T>* svd() const;

  //: Setters mirror the constructors and return true if the setting was successful.
  // In subclasses these should be redefined so that they won't allow setting of
  // matrices with improper form.
  virtual bool set_matrix( const vnl_matrix_fixed<T,3,4>& new_camera_matrix );
  virtual bool set_matrix( const T* new_camera_matrix ); // i.e., T new_camera_matrix[12]

  // --------------------- I/O :---------------------

  //: Save in ascii format
  virtual void save(std::string cam_path);

 private:
  //: The internal representation of the get_matrix.
  // It is private so subclasses will need to access it through "get_matrix" and "set_matrix".
  vnl_matrix_fixed<T,3,4> P_;

  mutable vnl_svd<T>* cached_svd_;
};


// External Functions:-------------------------------------------------------------

//: Return the 3D H-matrix s.t. P * H = [I 0].
template <class T>
vgl_h_matrix_3d<T> get_canonical_h( vpgl_proj_camera<T>& camera );

//: Scale the camera matrix so determinant of first 3x3 is 1.
template <class T>
void fix_cheirality( vpgl_proj_camera<T>& camera );

//: Set the camera matrix to [ I | 0 ].
template <class T>
void make_canonical( vpgl_proj_camera<T>& camera );

//: Pre-multiply this projection matrix with a 2-d projective transform.
template <class T>
vpgl_proj_camera<T> premultiply( const vpgl_proj_camera<T>& in_camera,
                                 const vnl_matrix_fixed<T,3,3>& transform );

//: Pre-multiply this projection matrix with a 2-d projective transform.
template <class T>
vpgl_proj_camera<T> premultiply( const vpgl_proj_camera<T>& in_camera,
                                 const vgl_h_matrix_2d<T>& transform )
{
  return premultiply(in_camera, transform.get_matrix());
}


//: Post-multiply this projection matrix with a 3-d projective transform.
template <class T>
vpgl_proj_camera<T> postmultiply( const vpgl_proj_camera<T>& in_camera,
                                  const vnl_matrix_fixed<T,4,4>& transform );

//: Post-multiply this projection matrix with a 3-d projective transform.
template <class T>
vpgl_proj_camera<T> postmultiply( const vpgl_proj_camera<T>& in_camera,
                                  const vgl_h_matrix_3d<T>& transform )
{
  return postmultiply(in_camera, transform.get_matrix());
}
//: Linearly intersect two camera rays to form a 3-d point
template <class T>
vgl_point_3d<T> triangulate_3d_point(const vpgl_proj_camera<T>& c1,
                                     const vgl_point_2d<T>& x1,
                                     const vpgl_proj_camera<T>& c2,
                                     const vgl_point_2d<T>& x2);

//: Compute the image projection Jacobians at each point
//  The returned matrices map a differential change in 3D
//  to a differential change in the 2D image at each specified 3D point
template <class T>
std::vector<vnl_matrix_fixed<T,2,3> >
image_jacobians(const vpgl_proj_camera<T>& camera,
                const std::vector<vgl_point_3d<T> >& pts);


// I/O ---

//: Write vpgl_perspective_camera to stream
template <class Type>
std::ostream&  operator<<(std::ostream& s, vpgl_proj_camera<Type> const& p);

//: Read vpgl_perspective_camera  from stream
template <class Type>
std::istream&  operator>>(std::istream& s, vpgl_proj_camera<Type>& p);

#endif // vpgl_proj_camera_h_
