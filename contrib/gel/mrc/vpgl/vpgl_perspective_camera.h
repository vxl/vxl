// This is gel/mrc/vpgl/vpgl_perspective_camera.h
#ifndef vpgl_perspective_camera_h_
#define vpgl_perspective_camera_h_
//:
// \file
// \brief A class for the perspective camera model.
// \author Thomas Pollard
// \date 01/28/05
// \author Joseph Mundy, Matt Leotta, Vishal Jain
//
// \verbatim
//  Modifications
//   5/08/2005  Ricardo Fabbri  Added binary I/O support
//   5/08/2005  Ricardo Fabbri  Added == operator
// \endverbatim


#include <vnl/vnl_fwd.h>
#include <vgl/vgl_fwd.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vcl_iosfwd.h>
#include <vcl_cassert.h>

#include "vpgl_proj_camera.h"
#include "vpgl_calibration_matrix.h"
#include <vsl/vsl_binary_io.h>


//: This class implements the perspective camera class as described in Hartley & Zisserman as a finite camera.
//  This is the camera model based on three objects:
//  the camera calibration matrix (see "vpgl_calibration_matrix.h"), the camera center,
//  and the rotation of the camera from its canonical orientation staring down the
//  positive z axis.
//
//  All rotation matrices entered will be checked that they are indeed rotations, i.e.
//  that R.transpose()*R = Identity and in the form:
//  \verbatim
//   [ R 0 ]
//   [ 0 1 ]
//  \endverbatim
//
//  For adding to this class:
//
//  Be sure to call recompute_matrix in your member functions any time you change any of the
//  camera parameters.
template <class T>
class vpgl_perspective_camera : public vpgl_proj_camera<T>
{
 public:
  //: Default constructor
  // Makes a camera at the origin with no rotation and default calibration matrix.
  vpgl_perspective_camera();

  //: Main constructor takes all of the camera parameters.
  vpgl_perspective_camera( const vpgl_calibration_matrix<T>& K,
                            const vgl_point_3d<T>& camera_center,
                            const vgl_h_matrix_3d<T>& R );

  //: Copy constructor
  vpgl_perspective_camera( const vpgl_perspective_camera& cam );

  //: Destructor
  virtual ~vpgl_perspective_camera(){}

  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  virtual vpgl_proj_camera<T>* clone(void) const;

  //: Compute the principal axis.
  // i.e. the vector parallel to the image plane pointing towards the front of the camera.
  vgl_vector_3d<T> principal_axis() const;

  //: Determine whether the given point lies in front of the principal plane.
  bool is_behind_camera( const vgl_homg_point_3d<T>& world_point ) const;

  //: Setters and getters.
  void set_calibration( const vpgl_calibration_matrix<T>& K );
  void set_camera_center( const vgl_point_3d<T>& camera_center );
  void set_rotation_matrix( const vgl_h_matrix_3d<T>& R );
  const vpgl_calibration_matrix<T>& get_calibration() const{ return K_; }
  const vgl_point_3d<T>& get_camera_center() const { return camera_center_; }
  const vgl_h_matrix_3d<T>& get_rotation_matrix() const{ return R_; }

  //: Rotate the camera about its center such that it looks at the given point
  //  The camera should also be rotated about its principle axis such that
  //  the vertical image direction is closest to \p up in the world
  void look_at(const vgl_homg_point_3d<T>& point,
               const vgl_vector_3d<double>& up = vgl_vector_3d<double>(0,0,1));

  // Redefined virtual functions -------------------------------------------

  //: Return the known camera center instead of computing it in the base class
  virtual vgl_homg_point_3d<T> camera_center() const
  { return vgl_homg_point_3d<T>(camera_center_); }

  // static public functions -----------------------------------------------

  //: Post-multiply this perspective camera with a 3-d Euclidean transformation
  static  vpgl_perspective_camera<T>
   postmultiply( const vpgl_perspective_camera<T>& in_cam,
                 const vgl_h_matrix_3d<T>& euclid_trans);

  //: Equality test
  inline bool operator==(vpgl_perspective_camera<T> const &that) const
  { return this == &that ||
    (K_ == that.K_ && this->get_matrix()== that.get_matrix() &&
     camera_center_ == that.camera_center_ && this->R_.get_matrix() == that.R_.get_matrix()); }

  // I/O :---------------------

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

  //: IO version number
  short version() const {return 1;}

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const { os << *this; }

  //: Return a platform independent string identifying the class.
  // This is used by e.g. polymorphic binary i/o
  virtual vcl_string is_a() const { return vcl_string("vpgl_perspective_camera"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(vcl_string const& cls) const
  { return cls==is_a() || vpgl_proj_camera<double>::is_class(cls); }

  //: Return `this' if `this' is a vpgl_perspective_camera, 0 otherwise
  // This is used by e.g. the storage class
  // \todo code for affine camera and other children
  virtual vpgl_perspective_camera<T> *cast_to_perspective_camera() {return this;}
  virtual const vpgl_perspective_camera<T> *cast_to_perspective_camera() const {return this;}

 protected:
  //: Recalculate the 3x4 camera matrix from the parameters.
  void recompute_matrix();

  vpgl_calibration_matrix<T> K_;
  vgl_point_3d<T> camera_center_;
  vgl_h_matrix_3d<T> R_;
};

// External Functions:-------------------------------------------------------------

//: Write vpgl_perspective_camera to stream
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, vpgl_perspective_camera<Type> const& p);

//: Read vpgl_perspective_camera  from stream
template <class Type>
vcl_istream&  operator>>(vcl_istream& s, vpgl_perspective_camera<Type>& p)
{
  //not yet implemented - FIXME
  return s;
}

//: Check that the matrix in question is a rotation.  This probably belongs in vgl.
template <class T>
bool vpgl_is_rotation( const vgl_h_matrix_3d<T>& H );

//: Check that the matrix in question is Euclidean.  This probably belongs in vgl.
template <class T>
bool vpgl_is_euclidean( const vgl_h_matrix_3d<T>& H );


//: Decompose camera into parameter blocks.
// Attempts to decompose a 3x4 camera matrix into the parameter blocks that describe
// a perspective camera, but will only work if the supplied matrix has a left 3x3
// submatrix with rank 3.
template <class T>
bool vpgl_perspective_decomposition( const vnl_matrix_fixed<T,3,4>& camera_matrix,
                                     vpgl_perspective_camera<T>& p_camera );

//: Changes the coordinate system of camera p1 such that the same change would transform p0 to K[I|0].
template <class T>
vpgl_perspective_camera<T> vpgl_align_down( const vpgl_perspective_camera<T>& p0,
                                            const vpgl_perspective_camera<T>& p1 );

//: Changes the coordinate system of camera p1 such that the same change would transform K[I|0] to p0.
template <class T>
vpgl_perspective_camera<T> vpgl_align_up( const vpgl_perspective_camera<T>& p0,
                                          const vpgl_perspective_camera<T>& p1 );

template <class T>
vpgl_perspective_camera<T>
postmultiply( const vpgl_perspective_camera<T>& in_cam,
              const vgl_h_matrix_3d<T>& euclid_trans);

//: Binary save
template <class T>
void vsl_b_write(vsl_b_ostream &os, const vpgl_perspective_camera<T>* p);


//: Binary read
template <class T>
void vsl_b_read(vsl_b_istream &is, vpgl_perspective_camera<T>* &p);


#endif // vpgl_perspective_camera_h_
