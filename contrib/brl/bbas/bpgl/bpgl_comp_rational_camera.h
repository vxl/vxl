// This is bbas/bpgl/bpgl_comp_rational_camera.h
#ifndef bpgl_comp_rational_camera_h_
#define bpgl_comp_rational_camera_h_
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
#include <iostream>
#include <string>
#include <vgl/vgl_fwd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vpgl/vpgl_rational_camera.h>

//
//--------------------=== composite rational camera ===---------------------------
//
template <class T>
class bpgl_comp_rational_camera : public vpgl_rational_camera<T>
{
 public:
  //: default constructor
  bpgl_comp_rational_camera();

  //: Constructor from a rational camera and an affine matrix
  bpgl_comp_rational_camera(vnl_matrix_fixed<T, 3,3> const& M,
                            vpgl_rational_camera<T> const& rcam);

  //: Constructor from translation only
  bpgl_comp_rational_camera(const T tu, const T tv,
                            vpgl_rational_camera<T> const& rcam);

  //: Constructor from translation rotation only (first rotate then translate)
  bpgl_comp_rational_camera(const T tu, const T tv, const T angle_in_radians,
                            vpgl_rational_camera<T> const& rcam);

  //: Constructor with all transform parameters (scale,then rotate,then trans)
  // Note, scale factors are non-negative
  bpgl_comp_rational_camera(const T tu, const T tv, const T angle_in_radians,
                            const T su, const T sv,
                            vpgl_rational_camera<T> const& rcam);

  //: Constructor, create from a camera file given by cam_path
  bpgl_comp_rational_camera(std::string cam_path);

  virtual ~bpgl_comp_rational_camera() = default;

  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  virtual bpgl_comp_rational_camera<T>* clone(void) const;

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
  virtual void print(std::ostream& s = std::cout) const;
  //: save to file (the affine map is after the end of the rational definition)
  bool save(std::string cam_path);

 protected:
  vnl_matrix_fixed<T, 3,3> matrix_;
};

//: Write to stream
// \relatesalso bpgl_comp_rational_camera
template <class T>
std::ostream& operator<<(std::ostream& s, const bpgl_comp_rational_camera<T>& p);

//: Read from stream
// \relatesalso bpgl_comp_rational_camera
template <class T>
std::istream& operator>>(std::istream& is, bpgl_comp_rational_camera<T>& p);

#define BPGL_COMP_RATIONAL_CAMERA_INSTANTIATE(T) extern "please include vgl/bpgl_comp_rational_camera.txx first"


#endif // bpgl_comp_rational_camera_h_
