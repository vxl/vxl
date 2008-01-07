#include "vpgl_project.h"
//:
// \file
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>

//: project a 3-d bounding box and derive the corresponding 2-d bounding box
vgl_box_2d<double> vpgl_project::
project_bounding_box(vpgl_perspective_camera<double>   cam,
                     vgl_box_3d<double> const & box)
{
  vpgl_proj_camera<double> pcam = static_cast<vpgl_proj_camera<double> >(cam);
  return vpgl_project::project_bounding_box(pcam, box);
}

vgl_box_2d<double> vpgl_project::project_bounding_box(vpgl_proj_camera<double>  cam,
                                        vgl_box_3d<double> const & box)
{
  //compute the vertices of the box
  double min_x = box.min_x(), max_x = box.max_x();
  double min_y = box.min_y(), max_y = box.max_y();
  double min_z = box.min_z(), max_z = box.max_z();
  vgl_homg_point_3d<double > p[8];
  p[0].set(min_x, min_y, min_z);  p[1].set(max_x, min_y, min_z);
  p[2].set(min_x, max_y, min_z);  p[3].set(max_x, max_y, min_z);
  p[4].set(min_x, min_y, max_z);  p[5].set(max_x, min_y, max_z);
  p[6].set(min_x, max_y, max_z);  p[7].set(max_x, max_y, max_z);

  //project the vertices and form the 2-d bounds
  vgl_box_2d<double>  box_2d;
  for (unsigned i =0; i<8; ++i)
  {
    vgl_homg_point_2d<double> hp2d = cam.project(p[i]);
    box_2d.add(vgl_point_2d<double>(hp2d));
  }
  return box_2d;
}
