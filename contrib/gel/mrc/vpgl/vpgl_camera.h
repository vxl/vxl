// This is gel/mrc/vpgl/vpgl_camera.h
#ifndef vpgl_camera_h_
#define vpgl_camera_h_

//:
// \file
// \brief A general camera class.
// \author Thomas Pollard
// \date 01/28/05
// \author Joseph Mundy, Matt Leotta, Vishal Jain
//
//   A basic abstract camera class on which all specific cameras are based.
//   As such there is only one operation it performs:
//   project a 3d world point into a 2d image point.  The class is templated
//   over T which had better be an algebraic field.
//
// Modifications:  October 26, 2006 - Moved homogeneous methods to
// projective camera, since projective geometry may not apply in the
// most general case, e.g. rational cameras. - JLM
#include <vcl_string.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

template<class T>
class vpgl_camera : public vbl_ref_count
{
 public:

  vpgl_camera(){};
  virtual ~vpgl_camera(){};

  virtual vcl_string type_name() const { return "vpgl_camera"; }

  //: The generic camera interface. u represents image column, v image row.
  virtual void project(const T x, const T y, const T z, T& u, T& v) const = 0;
};

// convienance typedefs for smart pointers to generic cameras
typedef vbl_smart_ptr<vpgl_camera<double> > vpgl_camera_double_sptr;


#endif // vpgl_camera_h_
