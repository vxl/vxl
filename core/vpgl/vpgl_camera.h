// This is core/vpgl/vpgl_camera.h
#ifndef vpgl_camera_h_
#define vpgl_camera_h_
//:
// \file
// \brief A general camera class.
// \author Thomas Pollard
// \date January 28, 2005
// \author Joseph Mundy, Matt Leotta, Vishal Jain
//
//   A basic abstract camera class on which all specific cameras are based.
//   As such there is only one operation it performs:
//   project a 3d world point into a 2d image point.  The class is templated
//   over T which had better be an algebraic field. Since camera operations are
//   based on projective geometry, it could be that case that T is complex
//   in order to account for projecting quadric surfaces.
//
// \verbatim
//  Modifications
//   October 26, 2006 - Moved homogeneous methods to projective camera, since
//                      projective geometry may not apply in the most general case, e.g. rational cameras. - JLM
// \endverbatim

#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>

template <class T>
class vpgl_camera : public vbl_ref_count
{
 public:

  vpgl_camera() = default;
  ~vpgl_camera() override = default;

  //: class identity functions for casting
  virtual std::string type_name() const { return "vpgl_camera"; }
  std::string is_a() const {return type_name();}
  bool is_class(std::string const& name) const {return type_name() == name;}

  //: The generic camera interface. u represents image column, v image row.
  virtual void project(const T x, const T y, const T z, T& u, T& v) const = 0;
};

// convenience typedefs for smart pointers to abstract cameras
#include "vpgl_camera_double_sptr.h"
#include "vpgl_camera_float_sptr.h"

#endif // vpgl_camera_h_
