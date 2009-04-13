#include "boxm_utils.h"
#include <boct/boct_tree_cell.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polygon_3d.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vcl_cassert.h>

#define DEBUG 1

bool boxm_utils::is_visible(vgl_box_3d<double> const& bbox,
                            vpgl_camera_double_sptr const& camera,
                            unsigned int img_ni, unsigned int img_nj,
                            bool do_front_test)
{
  if (camera->type_name().compare("vpgl_perspective_camera")==0) {
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

//                                 MaxPosition
//
//                       7-----------6
//                      /           /|
//                     /           / |
//                    4-----------5  |
//                    |           |  |
//                    |           |  |
//                    |    3      |  2
//     Y              |           | /
//     |   Z          |           |/
//     |  /           0-----------1
//     | /         MinPosition
//     O-----X

vcl_vector<vgl_point_3d<double> >
boxm_utils::corners_of_box_3d(vgl_box_3d<double> box)
{
  vcl_vector<vgl_point_3d<double> > corners;

  corners.push_back(box.min_point());
  corners.push_back(vgl_point_3d<double> (box.min_x()+box.width(), box.min_y(), box.min_z()));
  corners.push_back(vgl_point_3d<double> (box.min_x()+box.width(), box.min_y()+box.height(), box.min_z()));
  corners.push_back(vgl_point_3d<double> (box.min_x(), box.min_y()+box.height(), box.min_z()));
  corners.push_back(vgl_point_3d<double> (box.min_x(), box.min_y(), box.max_z()));
  corners.push_back(vgl_point_3d<double> (box.min_x()+box.width(), box.min_y(), box.max_z()));
  corners.push_back(box.max_point());
  corners.push_back(vgl_point_3d<double> (box.min_x(), box.min_y()+box.height(), box.max_z()));
  return corners;
}

bool boxm_utils::is_face_visible(vcl_vector<vgl_point_3d<double> > corners,
                                 vpgl_camera_double_sptr const& camera)
{
  double u,v;
  vgl_box_2d<double> face;
  vcl_vector<vsol_point_2d_sptr> vs;

  assert(corners.size() >= 3);

  for (unsigned i=0; i<corners.size(); i++) {
    camera->project(corners[i].x(), corners[i].y(), corners[i].z(), u, v);
    vs.push_back(new vsol_point_2d(u,v));
  }


  vgl_vector_2d<double> v0 = vs[1]->get_p() - vs[0]->get_p();
  vgl_vector_2d<double> v1 = vs[2]->get_p() - vs[1]->get_p();
  double normal = cross_product<double>(v0,v1);
  if (normal < 0)
    return true;
  return false;
}

boct_face_idx
boxm_utils::visible_faces(vgl_box_3d<double> &bbox, vpgl_camera_double_sptr camera)
{
  boct_face_idx faces = boct_cell_face::NONE;
  if (camera->type_name().compare("vpgl_perspective_camera")==0) {
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
  // for other cameras, use projection and normals
  else {
    // fix the face normals so that the vertices are the counter clokwise order
    vcl_vector<vgl_point_3d<double> > corners=corners_of_box_3d(bbox);

    // face bottom [1,0,3,2]
    vcl_vector<vgl_point_3d<double> > face_corners;
    face_corners.push_back(corners[1]);
    face_corners.push_back(corners[0]);
    face_corners.push_back(corners[3]);
    face_corners.push_back(corners[2]);
    if (is_face_visible(face_corners, camera)) {
      faces |= boct_cell_face::Z_LOW;
#if DEBUG
      vcl_cout << "Z_LOW " ;
#endif
    }

    // face top [4,5,6,7]
    face_corners.clear();
    face_corners.push_back(corners[4]);
    face_corners.push_back(corners[5]);
    face_corners.push_back(corners[6]);
    face_corners.push_back(corners[7]);
    if (is_face_visible(face_corners, camera)) {
      faces |= boct_cell_face::Z_HIGH;
#if DEBUG
      vcl_cout << "Z_HIGH " ;
#endif
    }
    face_corners.clear();
    face_corners.push_back(corners[1]);
    face_corners.push_back(corners[2]);
    face_corners.push_back(corners[6]);
    face_corners.push_back(corners[5]);
    if (is_face_visible(face_corners, camera)) {
      faces |= boct_cell_face::X_HIGH;
#if DEBUG
      vcl_cout << "X_HIGH " ;
#endif
    }

    face_corners.clear();
    face_corners.push_back(corners[7]);
    face_corners.push_back(corners[3]);
    face_corners.push_back(corners[0]);
    face_corners.push_back(corners[4]);
    if (is_face_visible(face_corners, camera)) {
      faces |= boct_cell_face::X_LOW;
#if DEBUG
      vcl_cout << "X_LOW " ;
#endif
    }

    face_corners.clear();
    face_corners.push_back(corners[2]);
    face_corners.push_back(corners[3]);
    face_corners.push_back(corners[7]);
    face_corners.push_back(corners[6]);
    if (is_face_visible(face_corners, camera)) {
      faces |= boct_cell_face::Y_HIGH;
#if DEBUG
      vcl_cout << "Y_HIGH " ;
#endif
    }
    face_corners.clear();

    face_corners.clear();
    face_corners.push_back(corners[0]);
    face_corners.push_back(corners[1]);
    face_corners.push_back(corners[5]);
    face_corners.push_back(corners[4]);
    if (is_face_visible(face_corners, camera)){
      faces |= boct_cell_face::Y_LOW;
#if DEBUG
      vcl_cout << "Y_LOW " ;
#endif
    }
  }
  vcl_cout << vcl_endl;
  return faces;
}
