#include "boxm_utils.h"
#include <boct/boct_tree_cell.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vpgl/vpgl_perspective_camera.h>


bool is_visible(vgl_box_3d<double> const& bbox, 
                vpgl_camera_double_sptr const& camera, 
                unsigned int img_ni, unsigned int img_nj, 
                bool do_front_test)
{
  if (camera->type_name().compare("vpgl_perspective_camera")) {
    // make a test for vertices for behind-front case
    vpgl_perspective_camera<double>* cam = static_cast<vpgl_perspective_camera<double>*>(camera.ptr());
    if (do_front_test) {
      if (cam->is_behind_camera(vgl_homg_point_3d<double>(bbox.min_x(),bbox.min_y(),bbox.min_z())))
        return false;
      if (cam->is_behind_camera(vgl_homg_point_3d<double>(bbox.min_x(),bbox.min_y(),bbox.max_z())))
        return false;
      if (cam->is_behind_camera(vgl_homg_point_3d<double>(bbox.min_x(),bbox.max_y(),bbox.min_z())))
        return false;
      if (cam->is_behind_camera(vgl_homg_point_3d<double>(bbox.min_x(),bbox.max_y(),bbox.max_z())))
        return false;
      if (cam->is_behind_camera(vgl_homg_point_3d<double>(bbox.max_x(),bbox.min_y(),bbox.min_z())))
        return false;
      if (cam->is_behind_camera(vgl_homg_point_3d<double>(bbox.max_x(),bbox.min_y(),bbox.max_z())))
        return false;
      if (cam->is_behind_camera(vgl_homg_point_3d<double>(bbox.max_x(),bbox.max_y(),bbox.min_z())))
        return false;
      if (cam->is_behind_camera(vgl_homg_point_3d<double>(bbox.max_x(),bbox.max_y(),bbox.max_z())))
        return false;
    }
  } 
   
  // make sure corners project into image bounds
  vgl_box_2d<double> cube_proj_bb;
  vgl_box_2d<double> img_bb;
  img_bb.add(vgl_point_2d<double>(0.0, 0.0));
  img_bb.add(vgl_point_2d<double>(img_ni, img_nj));
  // project corners of block into image
  double u,v;
  camera->project(bbox.min_x(),bbox.min_y(),bbox.min_z(),u,v);
  cube_proj_bb.add(vgl_point_2d<double>(u,v));
  camera->project(bbox.min_x(),bbox.min_y(),bbox.max_z(),u,v);
  cube_proj_bb.add(vgl_point_2d<double>(u,v));
  camera->project(bbox.min_x(),bbox.max_y(),bbox.min_z(),u,v);
  cube_proj_bb.add(vgl_point_2d<double>(u,v));
  camera->project(bbox.min_x(),bbox.max_y(),bbox.max_z(),u,v);
  cube_proj_bb.add(vgl_point_2d<double>(u,v));
  camera->project(bbox.max_x(),bbox.min_y(),bbox.min_z(),u,v);
  cube_proj_bb.add(vgl_point_2d<double>(u,v));
  camera->project(bbox.max_x(),bbox.min_y(),bbox.max_z(),u,v);
  cube_proj_bb.add(vgl_point_2d<double>(u,v));
  camera->project(bbox.max_x(),bbox.max_y(),bbox.min_z(),u,v);
  cube_proj_bb.add(vgl_point_2d<double>(u,v));
  camera->project(bbox.max_x(),bbox.max_y(),bbox.max_z(),u,v);
  cube_proj_bb.add(vgl_point_2d<double>(u,v));
  // check for intersection 
  vgl_box_2d<double> intersection =  vgl_intersection(cube_proj_bb, img_bb);
  if (intersection.is_empty()){
    // add to list
    return false;
  }
  return true;
}

boct_face_idx visible_faces(vgl_box_3d<double> &bbox, vpgl_camera_double_sptr camera)
{
  boct_face_idx faces = boct_cell_face::NONE;
  if (camera->type_name().compare("vpgl_perspective_camera")) {
    vpgl_perspective_camera<double>* cam = static_cast<vpgl_perspective_camera<double>*>(camera.ptr());
    vgl_point_3d<double> const& cam_center = cam->camera_center();

    if (cam_center.x() > bbox.max_x()) {
      faces |= boct_cell_face::X_HIGH;
    }
    else if (cam_center.x() < bbox.min_x()) {
      faces |= boct_cell_face::X_LOW;
    }
    if (cam_center.y() > bbox.max_y()) {
      faces |= boct_cell_face::Y_HIGH;
    }
    else if (cam_center.y() < bbox.min_y()) {
      faces |= boct_cell_face::Y_LOW;
    }
    if (cam_center.z() > bbox.max_x()) {
      faces |= boct_cell_face::Z_HIGH;
    }
    else if (cam_center.z() < bbox.min_z()) {
      faces |= boct_cell_face::Z_LOW;
    }
  }

  return faces;
}
