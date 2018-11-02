// This is bbas/bpgl/bpgl_rolling_shutter_camera.h
#ifndef bpgl_rolling_shutter_camera_h_
#define bpgl_rolling_shutter_camera_h_
//:
// \file
// \brief A camera model simulates rolling shutter
// \author Vishal Jain
// \date Feb 08, 2011
//
#include <iostream>
#include <vector>
#include <string>
#include <vgl/vgl_fwd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vnl/vnl_fwd.h>
#include <vpgl/vpgl_camera.h>

template <class T>
class bpgl_rolling_shutter_camera : public vpgl_camera<T>
{
 public:
  //: default constructor
  bpgl_rolling_shutter_camera()= default;

  ~bpgl_rolling_shutter_camera() override = default;

  std::string type_name() const override { return "bpgl_rolling_shutter_camera"; }

  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  virtual bpgl_rolling_shutter_camera<T>* clone(void) const=0;

  //: Project a world point onto the image
  virtual vnl_vector_fixed<T, 2> project(vnl_vector_fixed<T, 3> const& world_point) const=0;

  //: Project a world point onto the image
  virtual vgl_point_2d<T> project(vgl_point_3d<T> world_point) const=0;

  //: print the camera parameters
  virtual void print(std::ostream& s = std::cout) const=0;

  virtual bool save(std::string cam_path)=0;

  // --- binary IO ---

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const=0;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is)=0;
};


#endif // bpgl_rolling_shutter_camera_h_
