// This is brl/bbas/bpgl/algo/bpgl_transform_camera.h
#ifndef bpgl_transform_camera_h_
#define bpgl_transform_camera_h_
//:
// \file
// \brief Methods for Transforming Camera
// \author J. L. Mundy, Vishal Jain
// \date May 22, 2012

#include <vpgl/vpgl_perspective_camera.h>
#include <vgl/vgl_fwd.h>
#include <vgl/algo/vgl_rotation_3d.h>

class bpgl_transform_camera
{
 public:
  ~bpgl_transform_camera();

  // A similarity or Euclidean transformation for perspective camera
  static vpgl_perspective_camera<double> transform_perspective_camera(vpgl_perspective_camera<double> const& cam,
                                                                      vgl_rotation_3d<double> const& Rs,
                                                                      vnl_vector_fixed<double, 3> const& ts,
                                                                      const double scale=1.0);
 private:
  //: constructor private - static methods only
  bpgl_transform_camera();
};

#endif // bpgl_transform_camera_h_
