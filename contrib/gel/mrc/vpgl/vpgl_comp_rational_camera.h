// This is gel/mrc/vpgl/vpgl_comp_rational_camera.h
#ifndef vpgl_comp_rational_camera_h_
#define vpgl_comp_rational_camera_h_
//:
// \file
// \brief A composite rational camera model
// \author Joseph Mundy
// \date August 4, 2007
//
// Rational camera models often have positioning
// errors that can be corrected by a rigid body transformation
// of the image coordinate system. There also exist anamorphic image
// formats that require anisotropic scaling of the image coordinates.
// In order to support these image transformations, the rational camera
// class is augmented by a five-parameter affine transformation
// that includes: translation(2 dof); rotation(1 dof);
// and scale in u and v (2 dof).
//
#include <vgl/vgl_fwd.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vpgl/vpgl_rational_camera.h>

//
//--------------------=== composite rational camera ===---------------------------
//
template <class T>
class vpgl_comp_rational_camera : public vpgl_rational_camera<T>
{
 public:
  //: default constructor
  vpgl_comp_rational_camera();

  //: Constructor from a rational camera and an affine matrix
  vpgl_comp_rational_camera(vnl_matrix_fixed<T, 3,3> const& M,
                            vpgl_rational_camera<T> const& rcam);

  //: Constructor from translation only
  vpgl_comp_rational_camera(const T tu, const T tv,
                            vpgl_rational_camera<T> const& rcam);

  //: Constructor from translation rotation only (first rotate then translate)
  vpgl_comp_rational_camera(const T tu, const T tv, const T angle_in_radians,
                            vpgl_rational_camera<T> const& rcam);

  //: Constructor with all transform parameters (scale,then rotate,then trans)
  // Note, scale factors are non-negative
  vpgl_comp_rational_camera(const T tu, const T tv, const T angle_in_radians,
                            const T su, const T sv,
                            vpgl_rational_camera<T> const& rcam);

  //: Constructor, create from a camera file given by cam_path
  vpgl_comp_rational_camera(vcl_string cam_path);

  virtual ~vpgl_comp_rational_camera() {}

  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  virtual vpgl_comp_rational_camera<T>* clone(void) const;

        // Mutators/Accessors

  //: set the full affine transform matrix
  void set_transform(vnl_matrix_fixed<T, 3,3> const& M);
  //: set individual parameters
  void set_translation(const T tu, const T tv);
  void set_trans_rotation(const T tu, const T tv, const T angle_in_radians);

  //: note, scale factors su and sv are non-negative
  void set_all(const T tu, const T tv, const T angle_in_radians,
               const T su, const T sv);

  //: the full affine transform matrix
  vnl_matrix_fixed<T, 3,3> transform();
  //: image translation
  void translation(T& tu, T& tv);
  //: angle in radians
  T rotation_in_radians();
  //: image scale (note, scale factors are non-negative)
  void image_scale(T& su, T& sv);

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
  //: save to file (the affine map is after the end of the rational definition)
  bool save(vcl_string cam_path);

 protected:
  vnl_matrix_fixed<T, 3,3> matrix_;
};

//: Write to stream
// \relates vpgl_comp_rational_camera
template <class T>
vcl_ostream& operator<<(vcl_ostream& s, const vpgl_comp_rational_camera<T>& p);

//: Read from stream
// \relates vpgl_comp_rational_camera
template <class T>
vcl_istream& operator>>(vcl_istream& is, vpgl_comp_rational_camera<T>& p);

#define VPGL_COMP_RATIONAL_CAMERA_INSTANTIATE(T) extern "please include vgl/vpgl_comp_rational_camera.txx first"


#endif // vpgl_comp_rational_camera_h_

