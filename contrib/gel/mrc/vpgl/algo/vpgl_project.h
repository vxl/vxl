// This is gel/mrc/vpgl/algo/vpgl_project.h
#ifndef vpgl_project_h_
#define vpgl_project_h_
//:
// \file
// \brief Methods for projecting geometric structures onto the image
// \author J. L. Mundy
// \date Feb 23, 2005

#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

class vpgl_project
{
 public:

    ~vpgl_project();
    static vgl_box_2d<double> project_bounding_box(vpgl_perspective_camera<double>  cam,
                                                   vgl_box_3d<double> const & box);
    static vgl_box_2d<double> project_bounding_box(vpgl_proj_camera<double>  cam,
                                                   vgl_box_3d<double> const & box);
 private: 
  //: constructor private - static methods only
  vpgl_project();
};

#endif // vpgl_project_h_
