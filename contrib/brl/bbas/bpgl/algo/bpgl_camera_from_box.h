// This is bbas/bpgl/algo/bpgl_camera_from_box.h
#ifndef bpgl_camera_from_box_h_
#define bpgl_camera_from_box_h_
//:
// \file
// \brief Methods for computing a camera that looks at a box
// \author J. L. Mundy
// \date March 13, 2010

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

// The cameras are constructed so that the image size specified just
// contains the box. The centroid of the box projects to the center
// of the image.
class bpgl_camera_from_box
{
 public:
  static vpgl_perspective_camera<double>
    persp_camera_from_box(vgl_box_3d<double> const & box,
                          vgl_point_3d<double> const& camera_center,
                          unsigned ni,
                          unsigned nj);

  static vpgl_affine_camera<double>
    affine_camera_from_box(vgl_box_3d<double> const & box,
                           vgl_vector_3d<double> const& ray,
                           unsigned ni,
                           unsigned nj);

#if 0 // Implement later if needed
  static vpgl_proj_camera<double>
    proj_camera_from_box(vgl_box_3d<double> const & box);
#endif

 private:
  //: constructor private - static methods only
  bpgl_camera_from_box() = delete;
  ~bpgl_camera_from_box() = delete;
};

#endif // bpgl_camera_from_box_h_
