// This is brl/bbas/bpgl/algo/bpgl_project.h
#ifndef bpgl_project_h_
#define bpgl_project_h_
//:
// \file
// \brief Methods for projecting geometric structures onto the image
// \author J. L. Mundy
// \date Feb 23, 2005

#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

class bpgl_project
{
 public:

    ~bpgl_project();
    static vgl_box_2d<double> project_bounding_box(const vpgl_perspective_camera<double>&  cam,
                                                   vgl_box_3d<double> const & box);
    static vgl_box_2d<double> project_bounding_box(vpgl_affine_camera<double>  cam,
                                                   vgl_box_3d<double> const & box);
    static vgl_box_2d<double> project_bounding_box(const vpgl_proj_camera<double>&  cam,
                                                   vgl_box_3d<double> const & box);
 private:
  //: constructor private - static methods only
  bpgl_project();
};

#endif // bpgl_project_h_
