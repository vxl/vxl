#include <cmath>
#include <iostream>
#include <algorithm>
#include "boxm_utils.h"
//:
// \file
#include "boxm_quad_scan_iterator.h"
#include <boct/boct_tree_cell.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_intersection.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vnl/algo/vnl_determinant.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool boxm_utils::is_visible(vgl_box_3d<double> const& bbox,
                            vpgl_camera_double_sptr const& camera,
                            unsigned int img_ni, unsigned int img_nj,
                            bool do_front_test)
{
  if (camera->type_name().compare("vpgl_perspective_camera")==0) {
    // make a test for vertices for behind-front case
    auto* cam = static_cast<vpgl_perspective_camera<double>*>(camera.ptr());
    if (do_front_test) {
      if (cam->is_behind_camera(vgl_homg_point_3d<double>(bbox.min_x(),bbox.min_y(),bbox.min_z()))&&
          cam->is_behind_camera(vgl_homg_point_3d<double>(bbox.min_x(),bbox.min_y(),bbox.max_z()))&&
          cam->is_behind_camera(vgl_homg_point_3d<double>(bbox.min_x(),bbox.max_y(),bbox.min_z()))&&
          cam->is_behind_camera(vgl_homg_point_3d<double>(bbox.min_x(),bbox.max_y(),bbox.max_z()))&&
          cam->is_behind_camera(vgl_homg_point_3d<double>(bbox.max_x(),bbox.min_y(),bbox.min_z()))&&
          cam->is_behind_camera(vgl_homg_point_3d<double>(bbox.max_x(),bbox.min_y(),bbox.max_z()))&&
          cam->is_behind_camera(vgl_homg_point_3d<double>(bbox.max_x(),bbox.max_y(),bbox.min_z()))&&
          cam->is_behind_camera(vgl_homg_point_3d<double>(bbox.max_x(),bbox.max_y(),bbox.max_z())))
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
  return ! intersection.is_empty();
}

//:
// \verbatim
//                                 MaxPosition
//
//                       7-----------6
//                      /           /|
//                     /           / |
//                    4-----------5  |
//                    |           |  |
//                    |           |  |
//                    |    3      |  2
//     Z              |           | /
//     |   Y          |           |/
//     |  /           0-----------1
//     | /         MinPosition
//     O-----X
//
// \endverbatim
std::vector<vgl_point_3d<double> >
boxm_utils::corners_of_box_3d(vgl_box_3d<double> const& box)
{
  std::vector<vgl_point_3d<double> > corners;

  corners.push_back(box.min_point());
  corners.emplace_back(box.min_x()+box.width(), box.min_y(), box.min_z());
  corners.emplace_back(box.min_x()+box.width(), box.min_y()+box.height(), box.min_z());
  corners.emplace_back(box.min_x(), box.min_y()+box.height(), box.min_z());
  corners.emplace_back(box.min_x(), box.min_y(), box.max_z());
  corners.emplace_back(box.min_x()+box.width(), box.min_y(), box.max_z());
  corners.push_back(box.max_point());
  corners.emplace_back(box.min_x(), box.min_y()+box.height(), box.max_z());
  return corners;
}

void
boxm_utils::project_corners(std::vector<vgl_point_3d<double> > const& corners,
                            const vpgl_camera_double_sptr& camera,
                            double* xverts, double* yverts)
{
  for (unsigned i=0; i<corners.size(); ++i)
    camera->project(corners[i].x(), corners[i].y(), corners[i].z(), xverts[i], yverts[i]);
}

void
boxm_utils::project_corners(std::vector<vgl_point_3d<double> > const& corners,
                            const vpgl_camera_double_sptr& camera,
                            double* xverts, double* yverts,
                            float* vertdist)
{
    if (camera->type_name().compare("vpgl_perspective_camera")==0) {
        auto* cam = static_cast<vpgl_perspective_camera<double>*>(camera.ptr());
        vgl_point_3d<double> cam_center = vgl_point_3d<double>(cam->camera_center());
        for (unsigned i=0; i<corners.size(); ++i)
        {
            cam->project(corners[i].x(), corners[i].y(), corners[i].z(), xverts[i], yverts[i]);
            vertdist[i]=(float)(cam_center-corners[i]).length();
        }
    }
}

void
boxm_utils::project_point3d(vgl_point_3d<double> const& point,
                            const vpgl_camera_double_sptr& camera,
                            double & xvert, double &yvert,
                            double & vertdist)
{
    if (camera->type_name().compare("vpgl_perspective_camera")==0) {
        auto* cam = static_cast<vpgl_perspective_camera<double>*>(camera.ptr());
        vgl_point_3d<double> cam_center = vgl_point_3d<double>(cam->camera_center());
        cam->project(point.x(), point.y(), point.z(), xvert,yvert);
        vertdist=(float)(cam_center-point).length();
    }
}

//: corners of the input face: visible?
bool boxm_utils::is_face_visible(const double * xverts, const double *yverts,
                                 unsigned id1,unsigned id2,unsigned id3,unsigned  /*id4*/) // id4 unused?!? - FIXME
{
  double normal=(xverts[id2]-xverts[id1])*(yverts[id3]-yverts[id2])-(yverts[id2]-yverts[id1])*(xverts[id3]-xverts[id2]);
  return normal < 0;
}

//: corners of the input face: visible?
bool boxm_utils::is_face_visible(std::vector<vgl_point_3d<double> > &face,
                                 vpgl_camera_double_sptr const& camera)
{
  assert(face.size() >= 3);
  std::vector<vgl_point_2d<double> > vs;

  for (auto & i : face) {
    double u,v;
    camera->project(i.x(), i.y(), i.z(), u, v);
    vs.emplace_back(u,v);
  }

  vgl_vector_2d<double> v0 = vs[1] - vs[0];
  vgl_vector_2d<double> v1 = vs[2] - vs[1];
  auto normal = cross_product<double>(v0,v1);
  //if (normal>-1e-4 && normal<0.0)
  //    std::cout<<"+";
  return normal < -1e-5;
}

std::vector<vgl_point_2d<double> >
boxm_utils::project_face(std::vector<vgl_point_3d<double> > &face,
                         vpgl_camera_double_sptr const& camera)
{
  std::vector<vgl_point_2d<double> > vs;

  for (auto & i : face) {
    double u,v;
    camera->project(i.x(), i.y(), i.z(), u, v);
    vs.emplace_back(u,v);
  }

  return vs;
}

boct_face_idx
boxm_utils::visible_faces(vgl_box_3d<double> const& bbox, const vpgl_camera_double_sptr& camera)
{
  boct_face_idx face_idx = NONE;
  //if (camera->type_name().compare("vpgl_perspective_camera")==0) {
  //  vpgl_perspective_camera<double>* cam = static_cast<vpgl_perspective_camera<double>*>(camera.ptr());
  //  vgl_point_3d<double> const& cam_center = cam->camera_center();

  //  if (cam_center.x() > bbox.max_x()) {
  //    face_idx |= X_HIGH;
  //  }
  //  else if (cam_center.x() < bbox.min_x()) {
  //    face_idx |= X_LOW;
  //  }
  //  if (cam_center.y() > bbox.max_y()) {
  //    face_idx |= Y_HIGH;
  //  }
  //  else if (cam_center.y() < bbox.min_y()) {
  //    face_idx |= Y_LOW;
  //  }
  //  if (cam_center.z() > bbox.max_x()) {
  //    face_idx |= Z_HIGH;
  //  }
  //  else if (cam_center.z() < bbox.min_z()) {
  //    face_idx |= Z_LOW;
  //  }
  //}
  //// for other cameras, use projection and normals
  //else
  {
    // fix the face normals so that the vertices are in counter-clockwise order
    std::map<boct_face_idx, std::vector<vgl_point_3d<double> > > faces;
    faces_of_box_3d(bbox, faces);

    if (is_face_visible(faces.find(Z_LOW)->second, camera)) {
      face_idx |= Z_LOW;
#ifdef DEBUG
      std::cout << "Z_LOW " ;
#endif
    }
    if (is_face_visible(faces.find(Z_HIGH)->second, camera)) {
      face_idx |= Z_HIGH;
#ifdef DEBUG
      std::cout << "Z_HIGH " ;
#endif
    }
    if (is_face_visible(faces.find(X_LOW)->second, camera)) {
      face_idx |= X_LOW;
#ifdef DEBUG
      std::cout << "X_LOW " ;
#endif
    }

    if (is_face_visible(faces.find(X_HIGH)->second, camera)) {
      face_idx |= X_HIGH;
#ifdef DEBUG
      std::cout << "X_HIGH " ;
#endif
    }

    if (is_face_visible(faces.find(Y_LOW)->second, camera)) {
      face_idx |= Y_LOW;
#ifdef DEBUG
      std::cout << "Y_LOW " ;
#endif
    }

    if (is_face_visible(faces.find(Y_HIGH)->second, camera)) {
      face_idx |= Y_HIGH;
#ifdef DEBUG
      std::cout << "Y_HIGH " ;
#endif
    }
  }
#ifdef DEBUG
  std::cout << std::endl;
#endif

  return face_idx;
}

boct_face_idx
boxm_utils::visible_faces(vgl_box_3d<double> const& /*bbox*/, // FIXME: currently unused -- see commented out code
                          const vpgl_camera_double_sptr& /*camera*/, // idem
                          double *xverts, double *yverts)
{
  boct_face_idx face_idx = NONE;
#if 0
  if (camera->type_name().compare("vpgl_perspective_camera")==0) {
    vpgl_perspective_camera<double>* cam = static_cast<vpgl_perspective_camera<double>*>(camera.ptr());
    vgl_point_3d<double> const& cam_center = cam->camera_center();

    if (cam_center.x() > bbox.max_x()) {
      face_idx |= X_HIGH;
    }
    else if (cam_center.x() < bbox.min_x()) {
      face_idx |= X_LOW;
    }
    if (cam_center.y() > bbox.max_y()) {
      face_idx |= Y_HIGH;
    }
    else if (cam_center.y() < bbox.min_y()) {
      face_idx |= Y_LOW;
    }
    if (cam_center.z() > bbox.max_x()) {
      face_idx |= Z_HIGH;
    }
    else if (cam_center.z() < bbox.min_z()) {
      face_idx |= Z_LOW;
    }
  }
  // for other cameras, use projection and normals
  else
#endif // 0
  {
    // fix the face normals so that the vertices are the counter clockwise order
#if 0
    std::map<boct_face_idx, std::vector<vgl_point_3d<double> > > faces;
    faces_of_box_3d(bbox, faces);
    project_corners(bbox,camera,xverts,yverts);
#endif // 0
    if (is_face_visible(xverts,yverts,1,0,3,2)) {
      face_idx |= Z_LOW;
#ifdef DEBUG
      std::cout << "Z_LOW " ;
#endif
    }
    if (is_face_visible(xverts,yverts,4,5,6,7)) {
      face_idx |= Z_HIGH;
#ifdef DEBUG
      std::cout << "Z_HIGH " ;
#endif
    }
    if (is_face_visible(xverts,yverts,7,3,0,4)) {
      face_idx |= X_LOW;
#ifdef DEBUG
      std::cout << "X_LOW " ;
#endif
    }

    if (is_face_visible(xverts,yverts,1,2,6,5)) {
      face_idx |= X_HIGH;
#ifdef DEBUG
      std::cout << "X_HIGH " ;
#endif
    }

    if (is_face_visible(xverts,yverts,0,1,5,4)) {
      face_idx |= Y_LOW;
#ifdef DEBUG
      std::cout << "Y_LOW " ;
#endif
    }

    if (is_face_visible(xverts,yverts,2,3,7,6)) {
      face_idx |= Y_HIGH;
#ifdef DEBUG
      std::cout << "Y_HIGH " ;
#endif
    }
  }
#ifdef DEBUG
  std::cout << std::endl;
#endif

  return face_idx;
}

boct_face_idx
boxm_utils::visible_faces_cell(vgl_box_3d<double> const& bbox, const vpgl_camera_double_sptr& camera,
                               double *xverts, double *yverts)
{
  boct_face_idx face_idx = NONE;
  if (camera->type_name().compare("vpgl_perspective_camera")==0) {
    auto* cam = static_cast<vpgl_perspective_camera<double>*>(camera.ptr());
    vgl_point_3d<double> const& cam_center = cam->camera_center();

    if (cam_center.x() > bbox.max_x()) {
      face_idx |= X_HIGH;
    }
    else if (cam_center.x() < bbox.min_x()) {
      face_idx |= X_LOW;
    }
    if (cam_center.y() > bbox.max_y()) {
      face_idx |= Y_HIGH;
    }
    else if (cam_center.y() < bbox.min_y()) {
      face_idx |= Y_LOW;
    }
    if (cam_center.z() > bbox.max_x()) {
      face_idx |= Z_HIGH;
    }
    else if (cam_center.z() < bbox.min_z()) {
      face_idx |= Z_LOW;
    }
  }
  // for other cameras, use projection and normals
  else
  {
    // fix the face normals so that the vertices are the counter clockwise order
#if 0
    std::map<boct_face_idx, std::vector<vgl_point_3d<double> > > faces;
    faces_of_box_3d(bbox, faces);
    double * xverts; double *yverts;
    project_corners(bbox,camera,xverts,yverts);
#endif // 0
    if (is_face_visible(xverts,yverts,1,0,3,2)) {
      face_idx |= Z_LOW;
#ifdef DEBUG
      std::cout << "Z_LOW " ;
#endif
    }
    if (is_face_visible(xverts,yverts,4,5,6,7)) {
      face_idx |= Z_HIGH;
#ifdef DEBUG
      std::cout << "Z_HIGH " ;
#endif
    }
    if (is_face_visible(xverts,yverts,7,3,0,4)) {
      face_idx |= X_LOW;
#ifdef DEBUG
      std::cout << "X_LOW " ;
#endif
    }

    if (is_face_visible(xverts,yverts,1,2,6,5)) {
      face_idx |= X_HIGH;
#ifdef DEBUG
      std::cout << "X_HIGH " ;
#endif
    }

    if (is_face_visible(xverts,yverts,0,1,5,4)) {
      face_idx |= Y_LOW;
#ifdef DEBUG
      std::cout << "Y_LOW " ;
#endif
    }

    if (is_face_visible(xverts,yverts,2,3,7,6)) {
      face_idx |= Y_HIGH;
#ifdef DEBUG
      std::cout << "Y_HIGH " ;
#endif
    }
  }
#ifdef DEBUG
  std::cout << std::endl;
#endif

  return face_idx;
}

//: returns the faces of a box, the vertices are ordered in the normal direction
void boxm_utils::faces_of_box_3d(vgl_box_3d<double> const& bbox,
                                 std::map<boct_face_idx, std::vector<vgl_point_3d<double> > >& faces)
{
  std::vector<vgl_point_3d<double> > corners=corners_of_box_3d(bbox);

  // face bottom [1,0,3,2]
  std::vector<vgl_point_3d<double> > face_corners;
  face_corners.push_back(corners[1]);
  face_corners.push_back(corners[0]);
  face_corners.push_back(corners[3]);
  face_corners.push_back(corners[2]);
  faces[Z_LOW] = face_corners;

  face_corners.resize(0);
  face_corners.push_back(corners[4]);
  face_corners.push_back(corners[5]);
  face_corners.push_back(corners[6]);
  face_corners.push_back(corners[7]);
  faces[Z_HIGH] = face_corners;

  face_corners.resize(0);
  face_corners.push_back(corners[7]);
  face_corners.push_back(corners[3]);
  face_corners.push_back(corners[0]);
  face_corners.push_back(corners[4]);
  faces[X_LOW] = face_corners;

  face_corners.resize(0);
  face_corners.push_back(corners[1]);
  face_corners.push_back(corners[2]);
  face_corners.push_back(corners[6]);
  face_corners.push_back(corners[5]);
  faces[X_HIGH] = face_corners;

  face_corners.resize(0);
  face_corners.push_back(corners[0]);
  face_corners.push_back(corners[1]);
  face_corners.push_back(corners[5]);
  face_corners.push_back(corners[4]);
  faces[Y_LOW] = face_corners;

  face_corners.resize(0);
  face_corners.push_back(corners[2]);
  face_corners.push_back(corners[3]);
  face_corners.push_back(corners[7]);
  face_corners.push_back(corners[6]);
  faces[Y_HIGH] = face_corners;
}

//: returns the union of the projected faces of a polygon
void boxm_utils::project_cube(vgl_box_3d<double> &bbox,
                              const vpgl_camera_double_sptr& camera,
                              std::map<boct_face_idx, std::vector< vgl_point_3d<double> > > & faces,
                              boct_face_idx & vis_face_ids)
{
  faces_of_box_3d(bbox, faces);
  vis_face_ids=boxm_utils::visible_faces(bbox,camera);
}


void boxm_utils::quad_interpolate(boxm_quad_scan_iterator &poly_it,
                                  double* xvals, double* yvals, double* vals,
                                  vil_image_view<float> &img,unsigned img_plane_num,
                                  unsigned int v0, unsigned int v1,
                                  unsigned int v2,unsigned int v3 )
{
  // first compute s0, s1, s2, s3 such that  val = s0*x + s1*y + s2 for any point within the triangle
  // (no need to compute barycentric coordinates of a quadrilateral at each step)
  // subtract 0.5 from xvals and yvals, so that interpolated value at pixel x,y evaluates to coordinates x+0.5, y+0.5 (center of pixel)

  double Acol0[] = {(xvals[v0]-0.5)*(yvals[v0]-0.5),
            (xvals[v1]-0.5)*(yvals[v1]-0.5),
            (xvals[v2]-0.5)*(yvals[v2]-0.5),
            (xvals[v3]-0.5)*(yvals[v3]-0.5)};
  double Acol1[] = {xvals[v0]-0.5, xvals[v1]-0.5, xvals[v2]-0.5,xvals[v3]-0.5};
  double Acol2[] = {yvals[v0]-0.5, yvals[v1]-0.5, yvals[v2]-0.5,yvals[v3]-0.5};
  double Acol3[] = {1.0, 1.0, 1.0,1.0};

  double Z[] = {vals[v0], vals[v1], vals[v2],vals[v3]};

  double detA = vnl_determinant(Acol0, Acol1, Acol2,Acol3);
  double s0 = vnl_determinant(Z, Acol1, Acol2,Acol3) / detA;
  double s1 = vnl_determinant(Acol0, Z, Acol2,Acol3) / detA;
  double s2 = vnl_determinant(Acol0, Acol1, Z,Acol3) / detA;
  double s3 = vnl_determinant(Acol0, Acol1, Acol2,Z) / detA;

  poly_it.reset();
  while (poly_it.next()) {
    int y = poly_it.scany();
    if (y < 0){
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    auto yu = (unsigned int)y;
    if (yu >= img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (poly_it.startx() >= (int)img.ni()) || (poly_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)std::max((int)0,poly_it.startx());
    unsigned int endx = (unsigned int)std::min((int)img.ni(),poly_it.endx());

    for (unsigned int x = startx; x < endx; ++x) {
      auto interp_val = (float)(s0*x*y + s1*x + s2*y+s3);
      img(x,yu,img_plane_num) += (poly_it.pix_coverage(x)*interp_val);
    }
  }
  return;
#if 0 // commented out
  poly_it.reset();
  while (poly_it.next()) {
    int y = poly_it.scany();
    if (y < 0){
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    unsigned int yu = (unsigned int)y;
    if (yu >= img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (poly_it.startx() >= (int)img.ni()) || (poly_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)std::max((int)0,poly_it.startx());
    unsigned int endx = (unsigned int)std::min((int)img.ni(),poly_it.endx());

    double start_val=0;
    double end_val=0;

    if (poly_it.x_start_end_val(vals,start_val,end_val))
    {
      float rx=endx-startx;
      for (unsigned int x = startx; x < endx; ++x) {
        float interp_val = start_val+(x-startx)/rx*(end_val-start_val);
        img(x,yu,img_plane_num) += (poly_it.pix_coverage(x)*interp_val);
        //img(x,yu,img_plane_num) =interp_val;
      }
    }
  }
  return;
#endif // 0
}

void boxm_utils::quad_fill(boxm_quad_scan_iterator &poly_it,
                           vil_image_view<float> &img, float val,
                           unsigned img_plane_num)
{
  poly_it.reset();
  while (poly_it.next()) {
    int y = poly_it.scany();
    if (y < 0){
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    auto yu = (unsigned int)y;
    if (yu >= img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (poly_it.startx() >= (int)img.ni()) || (poly_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)std::max((int)0,poly_it.startx());
    unsigned int endx = (unsigned int)std::min((int)img.ni(),poly_it.endx());

    for (unsigned int x = startx; x < endx; ++x) {
      img(x,yu,img_plane_num) = val;
#if 0 // was:
      img(x,yu,img_plane_num) += (poly_it.pix_coverage(x)*val);
      img(x,yu,img_plane_num) += (poly_it.pix_coverage(x)*val);
      if (poly_it.pix_coverage(x)>1)
        std::cout<<"ERROR ALERT "<<poly_it.pix_coverage(x)<<std::endl;
      assert(poly_it.pix_coverage(x)>1);
#endif // 0
    }
  }
  return;
}


void boxm_utils::quad_fill(boxm_quad_scan_iterator &poly_it,
                           vil_image_view<float> &img,
                           vil_image_view<float> &weights_img,
                           float val,
                           unsigned img_plane_num)
{
  poly_it.reset();
  while (poly_it.next()) {
    int y = poly_it.scany();
    if (y < 0){
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    auto yu = (unsigned int)y;
    if (yu >= img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (poly_it.startx() >= (int)img.ni()) || (poly_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)std::max((int)0,poly_it.startx());
    unsigned int endx = (unsigned int)std::min((int)img.ni(),poly_it.endx());

    for (unsigned int x = startx; x < endx; ++x) {
      //img(x,yu,img_plane_num) += (poly_it.pix_coverage(x)*val);
      img(x,yu,img_plane_num) += (poly_it.pix_coverage(x)*val);
      weights_img(x,yu)+=poly_it.pix_coverage(x);
    }
  }
  return;
}


void boxm_utils::quad_mean(boxm_quad_scan_iterator &poly_it,
                           vil_image_view<float> &img, float &val,  float & count)
{
  poly_it.reset();
  while (poly_it.next()) {
    int y = poly_it.scany();
    if (y < 0){
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    auto yu = (unsigned int)y;
    if (yu >= img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (poly_it.startx() >= (int)img.ni()) || (poly_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)std::max((int)0,poly_it.startx());
    unsigned int endx = (unsigned int)std::min((int)img.ni(),poly_it.endx());

    for (unsigned int x = startx; x < endx; ++x) {
      val+=poly_it.pix_coverage(x)*img(x,yu) ;
      count+=poly_it.pix_coverage(x);
    }
  }
  return;
}

void boxm_utils::quad_weighted_mean(boxm_quad_scan_iterator &poly_it,
                                    vil_image_view<float> &img,
                                    vil_image_view<float> &weighted_img,
                                    float &numerator,  float & denominator)
{
  poly_it.reset();
  while (poly_it.next()) {
    int y = poly_it.scany();
    if (y < 0){
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    auto yu = (unsigned int)y;
    if (yu >= img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (poly_it.startx() >= (int)img.ni()) || (poly_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)std::max((int)0,poly_it.startx());
    unsigned int endx = (unsigned int)std::min((int)img.ni(),poly_it.endx());

    for (unsigned int x = startx; x < endx; ++x) {
      numerator+=poly_it.pix_coverage(x)*img(x,yu)*weighted_img(x,yu);
      denominator+=poly_it.pix_coverage(x)*weighted_img(x,yu);
      //numerator+=img(x,yu)*weighted_img(x,yu);
      //denominator+=weighted_img(x,yu);
    }
  }
  return;
}

void boxm_utils::quad_sum(boxm_quad_scan_iterator &poly_it,
                          vil_image_view<float> &img, float &val)
{
  poly_it.reset();
  while (poly_it.next()) {
    int y = poly_it.scany();
    if (y < 0){
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    auto yu = (unsigned int)y;
    if (yu >= img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (poly_it.startx() >= (int)img.ni()) || (poly_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)std::max((int)0,poly_it.startx());
    unsigned int endx = (unsigned int)std::min((int)img.ni(),poly_it.endx());

    for (unsigned int x = startx; x < endx; ++x) {
      val+=(poly_it.pix_coverage(x)*img(x,yu)) ;
    }
  }
  return;
}

bool boxm_utils::project_cube_xyz( std::map<boct_face_idx,std::vector< vgl_point_3d<double> > > & faces,
                                   boct_face_idx & vis_face_ids,
                                   vil_image_view<float> &front_xyz,
                                   vil_image_view<float> &back_xyz,
                                   const vpgl_camera_double_sptr& cam)
{
  auto face_it=faces.begin();
  for (; face_it!=faces.end(); ++face_it)
  {
    std::vector<vgl_point_3d<double> > face_corners=face_it->second;
    std::vector<vgl_point_2d<double> > face_projected=project_face(face_corners,cam);

    double xs[]={face_projected[0].x(),face_projected[1].x(),face_projected[2].x(),face_projected[3].x()};
    double ys[]={face_projected[0].y(),face_projected[1].y(),face_projected[2].y(),face_projected[3].y()};
    boxm_quad_scan_iterator poly_it(xs,ys);
    double Xs[]={face_corners[0].x(),face_corners[1].x(),face_corners[2].x(),face_corners[3].x()};
    double Ys[]={face_corners[0].y(),face_corners[1].y(),face_corners[2].y(),face_corners[3].y()};
    double Zs[]={face_corners[0].z(),face_corners[1].z(),face_corners[2].z(),face_corners[3].z()};

    if (vis_face_ids & face_it->first){
      quad_interpolate(poly_it,xs,ys,Xs,front_xyz,0);
      quad_interpolate(poly_it,xs,ys,Ys,front_xyz,1);
      quad_interpolate(poly_it,xs,ys,Zs,front_xyz,2);
    }
    else
    {
      quad_interpolate(poly_it,xs,ys,Xs,back_xyz,0);
      quad_interpolate(poly_it,xs,ys,Ys,back_xyz,1);
      quad_interpolate(poly_it,xs,ys,Zs,back_xyz,2);
    }
  }

  return true;
}


bool boxm_utils::project_cube_xyz(std::vector< vgl_point_3d<double> > & corners,
                                  boct_face_idx & vis_face_ids,
                                  vil_image_view<float> &front_xyz,
                                  vil_image_view<float> &back_xyz,
                                  double *xverts,double * yverts)
{
  // open extra scope, to allow redeclaration of xs[], ys[]
  {
    double xs[]={xverts[1],xverts[0],xverts[3],xverts[2]};
    double ys[]={yverts[1],yverts[0],yverts[3],yverts[2]};
#if 0
    std::vector<vgl_point_2d<double> > face_polygon(4);
    for (unsigned i=0;i<4;++i)
      face_polygon.push_back(vgl_point_2d<double>(xs[i],ys[i]));
#endif // 0

    boxm_quad_scan_iterator poly_it(xs,ys);
    vgl_polygon<double> p(xs,ys,4);

    double Xs[]={corners[1].x(),corners[0].x(),corners[3].x(),corners[2].x()};
    double Ys[]={corners[1].y(),corners[0].y(),corners[3].y(),corners[2].y()};
    double Zs[]={corners[1].z(),corners[0].z(),corners[3].z(),corners[2].z()};
    if (vis_face_ids & Z_LOW){
      quad_interpolate(poly_it,xs,ys,Xs,front_xyz,0);
      quad_interpolate(poly_it,xs,ys,Ys,front_xyz,1);
      quad_fill(poly_it,front_xyz,float(Zs[0]),2);
    }
    else
    {
      quad_interpolate(poly_it,xs,ys,Xs,back_xyz,0);
      quad_interpolate(poly_it,xs,ys,Ys,back_xyz,1);
      quad_fill(poly_it,back_xyz,float(Zs[0]),2);
    }
  }
  {
    double xs[]={xverts[4],xverts[5],xverts[6],xverts[7]};
    double ys[]={yverts[4],yverts[5],yverts[6],yverts[7]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    vgl_polygon<double> p(xs,ys,4);

    double Xs[]={corners[4].x(),corners[5].x(),corners[6].x(),corners[7].x()};
    double Ys[]={corners[4].y(),corners[5].y(),corners[6].y(),corners[7].y()};
    double Zs[]={corners[4].z(),corners[5].z(),corners[6].z(),corners[7].z()};
    if (vis_face_ids & Z_HIGH){
      quad_interpolate(poly_it,xs,ys,Xs,front_xyz,0);
      quad_interpolate(poly_it,xs,ys,Ys,front_xyz,1);
      quad_fill(poly_it,front_xyz,float(Zs[0]),2);
    }
    else
    {
      quad_interpolate(poly_it,xs,ys,Xs,back_xyz,0);
      quad_interpolate(poly_it,xs,ys,Ys,back_xyz,1);
      quad_fill(poly_it,back_xyz,float(Zs[0]),2);
    }
  }
  {
    double xs[]={xverts[7],xverts[3],xverts[0],xverts[4]};
    double ys[]={yverts[7],yverts[3],yverts[0],yverts[4]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    double Xs[]={corners[7].x(),corners[3].x(),corners[0].x(),corners[4].x()};
    double Ys[]={corners[7].y(),corners[3].y(),corners[0].y(),corners[4].y()};
    double Zs[]={corners[7].z(),corners[3].z(),corners[0].z(),corners[4].z()};
    if (vis_face_ids & X_LOW){
      quad_fill(poly_it,front_xyz,float(Xs[0]),0);
      quad_interpolate(poly_it,xs,ys,Ys,front_xyz,1);
      quad_interpolate(poly_it,xs,ys,Zs,front_xyz,2);
    }
    else
    {
      quad_fill(poly_it,back_xyz,float(Xs[0]),0);
      quad_interpolate(poly_it,xs,ys,Ys,back_xyz,1);
      quad_interpolate(poly_it,xs,ys,Zs,back_xyz,2);
    }
  }
  {
    double xs[]={xverts[1],xverts[2],xverts[6],xverts[5]};
    double ys[]={yverts[1],yverts[2],yverts[6],yverts[5]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    double Xs[]={corners[1].x(),corners[2].x(),corners[6].x(),corners[5].x()};
    double Ys[]={corners[1].y(),corners[2].y(),corners[6].y(),corners[5].y()};
    double Zs[]={corners[1].z(),corners[2].z(),corners[6].z(),corners[5].z()};
    if (vis_face_ids & X_HIGH){
      quad_fill(poly_it,front_xyz,float(Xs[0]),0);
      quad_interpolate(poly_it,xs,ys,Ys,front_xyz,1);
      quad_interpolate(poly_it,xs,ys,Zs,front_xyz,2);
    }
    else
    {
      quad_fill(poly_it,back_xyz,float(Xs[0]),0);
      quad_interpolate(poly_it,xs,ys,Ys,back_xyz,1);
      quad_interpolate(poly_it,xs,ys,Zs,back_xyz,2);
    }
  }
  {
    double xs[]={xverts[0],xverts[1],xverts[5],xverts[4]};
    double ys[]={yverts[0],yverts[1],yverts[5],yverts[4]};
    boxm_quad_scan_iterator poly_it(xs,ys);
        vgl_polygon<double> p(xs,ys,4);

    double Xs[]={corners[0].x(),corners[1].x(),corners[5].x(),corners[4].x()};
    double Ys[]={corners[0].y(),corners[1].y(),corners[5].y(),corners[4].y()};
    double Zs[]={corners[0].z(),corners[1].z(),corners[5].z(),corners[4].z()};
    if (vis_face_ids & Y_LOW){
      quad_interpolate(poly_it,xs,ys,Xs,front_xyz,0);

      quad_fill(poly_it,front_xyz,float(Ys[0]),1);
      quad_interpolate(poly_it,xs,ys,Zs,front_xyz,2);
    }
    else
    {
      quad_interpolate(poly_it,xs,ys,Xs,back_xyz,0);
      quad_fill(poly_it,back_xyz,float(Ys[0]),1);
      quad_interpolate(poly_it,xs,ys,Zs,back_xyz,2);
    }
  }
  {
    double xs[]={xverts[2],xverts[3],xverts[7],xverts[6]};
    double ys[]={yverts[2],yverts[3],yverts[7],yverts[6]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    vgl_polygon<double> p(xs,ys,4);

    double Xs[]={corners[2].x(),corners[3].x(),corners[7].x(),corners[6].x()};
    double Ys[]={corners[2].y(),corners[3].y(),corners[7].y(),corners[6].y()};
    double Zs[]={corners[2].z(),corners[3].z(),corners[7].z(),corners[6].z()};
    if (vis_face_ids & Y_HIGH){
      quad_interpolate(poly_it,xs,ys,Xs,front_xyz,0);
      quad_fill(poly_it,front_xyz,float(Ys[0]),1);
      quad_interpolate(poly_it,xs,ys,Zs,front_xyz,2);
    }
    else
    {
      quad_interpolate(poly_it,xs,ys,Xs,back_xyz,0);
      quad_fill(poly_it,back_xyz,float(Ys[0]),1);
      quad_interpolate(poly_it,xs,ys,Zs,back_xyz,2);
    }
  }

  return true;
}


bool boxm_utils::project_cube_xyz(std::vector< vgl_point_3d<double> > & /*corners*/,
                                  boct_face_idx & vis_face_ids,
                                  vil_image_view<float> &front_xyz,
                                  vil_image_view<float> &back_xyz,
                                  double *xverts,double * yverts,float * vertdist)
{
  // Use extra {} so that destructors will do their work
  {
    double xs[]={xverts[1],xverts[0],xverts[3],xverts[2]};
    double ys[]={yverts[1],yverts[0],yverts[3],yverts[2]};
    double ds[]={vertdist[1],vertdist[0],vertdist[3],vertdist[2]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    vgl_polygon<double> p(xs,ys,4);
    if (vis_face_ids & Z_LOW){ quad_interpolate(poly_it,xs,ys,ds,front_xyz,0); }
    else                     { quad_interpolate(poly_it,xs,ys,ds,back_xyz,0); }
  }
  {
    double xs[]={xverts[4],xverts[5],xverts[6],xverts[7]};
    double ys[]={yverts[4],yverts[5],yverts[6],yverts[7]};
    double ds[]={vertdist[4],vertdist[5],vertdist[6],vertdist[7]};

    boxm_quad_scan_iterator poly_it(xs,ys);
    vgl_polygon<double> p(xs,ys,4);
    if (vis_face_ids & Z_HIGH){ quad_interpolate(poly_it,xs,ys,ds,front_xyz,0); }
    else                      { quad_interpolate(poly_it,xs,ys,ds,back_xyz,0); }
  }
  {
    double xs[]={xverts[7],xverts[3],xverts[0],xverts[4]};
    double ys[]={yverts[7],yverts[3],yverts[0],yverts[4]};
    double ds[]={vertdist[7],vertdist[3],vertdist[0],vertdist[4]};

    boxm_quad_scan_iterator poly_it(xs,ys);
    if (vis_face_ids & X_LOW){ quad_interpolate(poly_it,xs,ys,ds,front_xyz,0); }
    else                     { quad_interpolate(poly_it,xs,ys,ds,back_xyz,0); }
  }
  {
    double xs[]={xverts[1],xverts[2],xverts[6],xverts[5]};
    double ys[]={yverts[1],yverts[2],yverts[6],yverts[5]};
    double ds[]={vertdist[1],vertdist[2],vertdist[6],vertdist[5]};

    boxm_quad_scan_iterator poly_it(xs,ys);
    if (vis_face_ids & X_HIGH){ quad_interpolate(poly_it,xs,ys,ds,front_xyz,0); }
    else                      { quad_interpolate(poly_it,xs,ys,ds,back_xyz,0); }
  }
  {
    double xs[]={xverts[0],xverts[1],xverts[5],xverts[4]};
    double ys[]={yverts[0],yverts[1],yverts[5],yverts[4]};
    double ds[]={vertdist[0],vertdist[1],vertdist[5],vertdist[4]};

    boxm_quad_scan_iterator poly_it(xs,ys);
    vgl_polygon<double> p(xs,ys,4);
    if (vis_face_ids & Y_LOW){ quad_interpolate(poly_it,xs,ys,ds,front_xyz,0); }
    else                     { quad_interpolate(poly_it,xs,ys,ds,back_xyz,0); }
  }
  {
    double xs[]={xverts[2],xverts[3],xverts[7],xverts[6]};
    double ys[]={yverts[2],yverts[3],yverts[7],yverts[6]};
    double ds[]={vertdist[2],vertdist[3],vertdist[7],vertdist[6]};

    boxm_quad_scan_iterator poly_it(xs,ys);
    vgl_polygon<double> p(xs,ys,4);
    if (vis_face_ids & Y_HIGH){ quad_interpolate(poly_it,xs,ys,ds,front_xyz,0); }
    else                      { quad_interpolate(poly_it,xs,ys,ds,back_xyz,0); }
  }

  return true;
}


#if 0
bool boxm_utils::project_cube_fill_val( std::map<boct_face_idx,std::vector< vgl_point_3d<double> > > & faces,
                                        boct_face_idx & vis_face_ids,
                                        vil_image_view<float> &fill_img,
                                        float val, vpgl_camera_double_sptr cam)
{
  for (std::map<boct_face_idx, std::vector<vgl_point_3d<double> > >::iterator face_it=faces.begin();
       face_it!=faces.end(); ++face_it)
  {
    std::vector<vgl_point_3d<double> > face_corners=face_it->second;
    std::vector<vgl_point_2d<double> > face_projected=project_face(face_corners,cam);
    vgl_polygon<double> face_polygon(face_projected);
    double xs[]={xverts[1],xverts[0],xverts[3],xverts[2]};
    double ys[]={yverts[1],yverts[0],yverts[3],yverts[2]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    if (vis_face_ids & face_it->first){
      quad_fill(poly_it,fill_img,val,0);
    }
  }
}
#else
bool boxm_utils::project_cube_fill_val( std::map<boct_face_idx,std::vector< vgl_point_3d<double> > > &,
                                        boct_face_idx &, vil_image_view<float> &, float, const vpgl_camera_double_sptr&)
{ return false; }
#endif // 0

bool boxm_utils::project_cube_fill_val(boct_face_idx & vis_face_ids,
                                       vil_image_view<float> &fill_img,
                                       float val, double *xverts,double * yverts)
{
  if (vis_face_ids & Z_LOW){
    double xs[]={xverts[1],xverts[0],xverts[3],xverts[2]};
    double ys[]={yverts[1],yverts[0],yverts[3],yverts[2]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_fill(poly_it,fill_img,val,0);
  }
  if (vis_face_ids & Z_HIGH){
    double xs[]={xverts[4],xverts[5],xverts[6],xverts[7]};
    double ys[]={yverts[4],yverts[5],yverts[6],yverts[7]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_fill(poly_it,fill_img,val,0);
  }
  if (vis_face_ids & X_LOW){
    double xs[]={xverts[7],xverts[3],xverts[0],xverts[4]};
    double ys[]={yverts[7],yverts[3],yverts[0],yverts[4]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_fill(poly_it,fill_img,val,0);
  }
  if (vis_face_ids & X_HIGH){
    double xs[]={xverts[1],xverts[2],xverts[6],xverts[5]};
    double ys[]={yverts[1],yverts[2],yverts[6],yverts[5]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_fill(poly_it,fill_img,val,0);
  }
  if (vis_face_ids & Y_LOW){
    double xs[]={xverts[0],xverts[1],xverts[5],xverts[4]};
    double ys[]={yverts[0],yverts[1],yverts[5],yverts[4]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_fill(poly_it,fill_img,val,0);
  }
  if (vis_face_ids & Y_HIGH){
    double xs[]={xverts[2],xverts[3],xverts[7],xverts[6]};
    double ys[]={yverts[2],yverts[3],yverts[7],yverts[6]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_fill(poly_it,fill_img,val,0);
  }
  return true;
}


bool boxm_utils::project_cube_fill_val_aa(boct_face_idx & vis_face_ids,
                                          vil_image_view<float> &fill_img,
                                          vil_image_view<float> &weight_img,
                                          float val, double *xverts,double * yverts)
{
  if (vis_face_ids & Z_LOW){
    double xs[]={xverts[1],xverts[0],xverts[3],xverts[2]};
    double ys[]={yverts[1],yverts[0],yverts[3],yverts[2]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_fill(poly_it,fill_img,weight_img,val,0);
  }
  if (vis_face_ids & Z_HIGH){
    double xs[]={xverts[4],xverts[5],xverts[6],xverts[7]};
    double ys[]={yverts[4],yverts[5],yverts[6],yverts[7]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_fill(poly_it,fill_img,weight_img,val,0);
  }
  if (vis_face_ids & X_LOW){
    double xs[]={xverts[7],xverts[3],xverts[0],xverts[4]};
    double ys[]={yverts[7],yverts[3],yverts[0],yverts[4]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_fill(poly_it,fill_img,weight_img,val,0);
  }
  if (vis_face_ids & X_HIGH){
    double xs[]={xverts[1],xverts[2],xverts[6],xverts[5]};
    double ys[]={yverts[1],yverts[2],yverts[6],yverts[5]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_fill(poly_it,fill_img,weight_img,val,0);
  }
  if (vis_face_ids & Y_LOW){
    double xs[]={xverts[0],xverts[1],xverts[5],xverts[4]};
    double ys[]={yverts[0],yverts[1],yverts[5],yverts[4]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_fill(poly_it,fill_img,weight_img,val,0);
  }
  if (vis_face_ids & Y_HIGH){
    double xs[]={xverts[2],xverts[3],xverts[7],xverts[6]};
    double ys[]={yverts[2],yverts[3],yverts[7],yverts[6]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_fill(poly_it,fill_img,weight_img,val,0);
  }
  return true;
}

bool boxm_utils::cube_sum(boct_face_idx & vis_face_ids,
                          vil_image_view<float> &img,
                          float & val, double *xverts,double * yverts)
{
  val=0;
  if (vis_face_ids & Z_LOW){
    double xs[]={xverts[1],xverts[0],xverts[3],xverts[2]};
    double ys[]={yverts[1],yverts[0],yverts[3],yverts[2]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_sum(poly_it,img,val);
  }
  if (vis_face_ids & Z_HIGH){
    double xs[]={xverts[4],xverts[5],xverts[6],xverts[7]};
    double ys[]={yverts[4],yverts[5],yverts[6],yverts[7]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_sum(poly_it,img,val);
  }
  if (vis_face_ids & X_LOW){
    double xs[]={xverts[7],xverts[3],xverts[0],xverts[4]};
    double ys[]={yverts[7],yverts[3],yverts[0],yverts[4]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_sum(poly_it,img,val);
  }
  if (vis_face_ids & X_HIGH){
    double xs[]={xverts[1],xverts[2],xverts[6],xverts[5]};
    double ys[]={yverts[1],yverts[2],yverts[6],yverts[5]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_sum(poly_it,img,val);
  }
  if (vis_face_ids & Y_LOW){
    double xs[]={xverts[0],xverts[1],xverts[5],xverts[4]};
    double ys[]={yverts[0],yverts[1],yverts[5],yverts[4]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_sum(poly_it,img,val);
  }
  if (vis_face_ids & Y_HIGH){
    double xs[]={xverts[2],xverts[3],xverts[7],xverts[6]};
    double ys[]={yverts[2],yverts[3],yverts[7],yverts[6]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_sum(poly_it,img,val);
  }

  return true;
}


bool boxm_utils::cube_uniform_mean(boct_face_idx & vis_face_ids,
                                   vil_image_view<float> &img,
                                   float & val, double *xverts,double * yverts)
{
  val=0;
  float count=0;
  if (vis_face_ids & Z_LOW){
    double xs[]={xverts[1],xverts[0],xverts[3],xverts[2]};
    double ys[]={yverts[1],yverts[0],yverts[3],yverts[2]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_mean(poly_it,img,val,count);
  }
  if (vis_face_ids & Z_HIGH){
    double xs[]={xverts[4],xverts[5],xverts[6],xverts[7]};
    double ys[]={yverts[4],yverts[5],yverts[6],yverts[7]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_mean(poly_it,img,val,count);
  }
  if (vis_face_ids & X_LOW){
    double xs[]={xverts[7],xverts[3],xverts[0],xverts[4]};
    double ys[]={yverts[7],yverts[3],yverts[0],yverts[4]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_mean(poly_it,img,val,count);
  }
  if (vis_face_ids & X_HIGH){
    double xs[]={xverts[1],xverts[2],xverts[6],xverts[5]};
    double ys[]={yverts[1],yverts[2],yverts[6],yverts[5]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_mean(poly_it,img,val,count);
  }
  if (vis_face_ids & Y_LOW){
    double xs[]={xverts[0],xverts[1],xverts[5],xverts[4]};
    double ys[]={yverts[0],yverts[1],yverts[5],yverts[4]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_mean(poly_it,img,val,count);
  }
  if (vis_face_ids & Y_HIGH){
    double xs[]={xverts[2],xverts[3],xverts[7],xverts[6]};
    double ys[]={yverts[2],yverts[3],yverts[7],yverts[6]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_mean(poly_it,img,val,count);
  }

  if (count > 0) {
    val/=(float)count;
    return true;
  }
  else {
    val = 0;
    return true;
  }
}


bool boxm_utils::cube_weighted_mean(boct_face_idx & vis_face_ids,
                                    vil_image_view<float> &img,
                                    vil_image_view<float> &weight_img,
                                    float & val, double *xverts,double * yverts)
{
  val=0;
  float count=0;
  if (vis_face_ids & Z_LOW){
    double xs[]={xverts[1],xverts[0],xverts[3],xverts[2]};
    double ys[]={yverts[1],yverts[0],yverts[3],yverts[2]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_weighted_mean(poly_it,img,weight_img,val,count);
  }
  if (vis_face_ids & Z_HIGH){
    double xs[]={xverts[4],xverts[5],xverts[6],xverts[7]};
    double ys[]={yverts[4],yverts[5],yverts[6],yverts[7]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_weighted_mean(poly_it,img,weight_img,val,count);
  }
  if (vis_face_ids & X_LOW){
    double xs[]={xverts[7],xverts[3],xverts[0],xverts[4]};
    double ys[]={yverts[7],yverts[3],yverts[0],yverts[4]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_weighted_mean(poly_it,img,weight_img,val,count);
  }
  if (vis_face_ids & X_HIGH){
    double xs[]={xverts[1],xverts[2],xverts[6],xverts[5]};
    double ys[]={yverts[1],yverts[2],yverts[6],yverts[5]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_weighted_mean(poly_it,img,weight_img,val,count);
  }
  if (vis_face_ids & Y_LOW){
    double xs[]={xverts[0],xverts[1],xverts[5],xverts[4]};
    double ys[]={yverts[0],yverts[1],yverts[5],yverts[4]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_weighted_mean(poly_it,img,weight_img,val,count);
  }
  if (vis_face_ids & Y_HIGH){
    double xs[]={xverts[2],xverts[3],xverts[7],xverts[6]};
    double ys[]={yverts[2],yverts[3],yverts[7],yverts[6]};
    boxm_quad_scan_iterator poly_it(xs,ys);
    quad_weighted_mean(poly_it,img,weight_img,val,count);
  }

  if (count > 0) {
    val/=(float)count;
    return true;
  }
  else {
    val = 0;
    return true;
  }
}


bool
boxm_utils::cube_entry_point(vgl_box_3d<double> cube,vgl_point_3d<double> pt,
                             vgl_vector_3d<double> direction,
                             vgl_point_3d<double> & entry_point,
                             double & lambda,boct_face_idx & face_id)
{
  // check each face
  if (direction.x() > 0 && pt.x()<=cube.min_x()) {
    // intersect with low x plane
    lambda = (cube.min_x() - pt.x()) / direction.x();
    entry_point = pt + direction * lambda;
    if ( (entry_point.y() >= cube.min_y()) && (entry_point.y() <= cube.max_y()) &&
      (entry_point.z() >= cube.min_z()) && (entry_point.z() <= cube.max_z()) ) {
        face_id=X_LOW;
        return true;
    }
  }
  else if (direction.x() < 0 && pt.x()>=cube.max_x()){
    // intersect with high x plane
    lambda = (cube.max_x() - pt.x()) / direction.x();
    entry_point = pt + direction * lambda;
    if ( (entry_point.y() >= cube.min_y()) && (entry_point.y() <= cube.max_y()) &&
      (entry_point.z() >= cube.min_z()) && (entry_point.z() <= cube.max_z()) ) {
        face_id=X_HIGH;
        return true;
    }
  }
  if (direction.y() > 0&& pt.y()<=cube.min_y()) {
    // intersect with low y plane
    lambda = (cube.min_y() - pt.y()) / direction.y();
    entry_point = pt + direction * lambda;
    if ( (entry_point.x() >= cube.min_x()) && (entry_point.x() <= cube.max_x()) &&
      (entry_point.z() >= cube.min_z()) && (entry_point.z() <= cube.max_z()) ) {
        face_id=Y_LOW;
        return true;
    }
  }
  else if (direction.y() < 0 && pt.y()>=cube.max_y()){
    // intersect with high y plane
    lambda = (cube.max_y() - pt.y()) / direction.y();
    entry_point = pt + direction * lambda;
    if ( (entry_point.x() >= cube.min_x()) && (entry_point.x() <= cube.max_x()) &&
      (entry_point.z() >= cube.min_z()) && (entry_point.z() <= cube.max_z()) ) {
        face_id=Y_HIGH;
        return true;
    }
  }
  if (direction.z() > 0&& pt.z()<=cube.min_z()) {
    // intersect with low z plane
    lambda = (cube.min_z() - pt.z()) / direction.z();
    entry_point = pt + direction * lambda;
    if ( (entry_point.x() >= cube.min_x()) && (entry_point.x() <= cube.max_x()) &&
      (entry_point.y() >= cube.min_y()) && (entry_point.y() <= cube.max_y()) ) {
        face_id=Z_LOW;
        return true;
    }
  }
  else if (direction.z() < 0 && pt.z()>=cube.max_z()){
    // intersect with high z plane
    lambda = (cube.max_z() - pt.z()) / direction.z();
    entry_point = pt + direction * lambda;
    if ( (entry_point.x() >= cube.min_x()) && (entry_point.x() <= cube.max_x()) &&
      (entry_point.y() >= cube.min_y()) && (entry_point.y() <= cube.max_y()) ) {
        face_id=Z_HIGH;
        return true;
    }
  }
  return false;
}


bool boxm_utils::cube_exit_point(vgl_box_3d<double> cube,vgl_point_3d<double> pt,
                                 vgl_vector_3d<double> direction,
                                 vgl_point_3d<double> & exit_pt,
                                 double & lambda,boct_face_idx & face_id)
{
  // check each face
  if (direction.x() < 0 ) {
    // intersect with low x plane
    lambda = (cube.min_x() - pt.x()) / direction.x();
    exit_pt = pt + direction * lambda;
    if ( (exit_pt.y() >= cube.min_y()) && (exit_pt.y() <= cube.max_y()) &&
         (exit_pt.z() >= cube.min_z()) && (exit_pt.z() <= cube.max_z()) ) {
      face_id=X_LOW;
      return true;
    }
  }
  else if (direction.x() > 0 ) {
    // intersect with high x plane
    lambda = (cube.max_x() - pt.x()) / direction.x();
    exit_pt = pt + direction * lambda;
    if ( (exit_pt.y() >= cube.min_y()) && (exit_pt.y() <= cube.max_y()) &&
         (exit_pt.z() >= cube.min_z()) && (exit_pt.z() <= cube.max_z()) ) {
      face_id=X_HIGH;
      return true;
    }
  }
  if (direction.y() < 0 ) {
    // intersect with low y plane
    lambda = (cube.min_y() - pt.y()) / direction.y();
    exit_pt = pt + direction * lambda;
    if ( (exit_pt.x() >= cube.min_x()) && (exit_pt.x() <= cube.max_x()) &&
         (exit_pt.z() >= cube.min_z()) && (exit_pt.z() <= cube.max_z()) ) {
      face_id=Y_LOW;
      return true;
    }
  }
  else if (direction.y() > 0 ) {
    // intersect with high y plane
    lambda = (cube.max_y() - pt.y()) / direction.y();
    exit_pt = pt + direction * lambda;
    if ( (exit_pt.x() >= cube.min_x()) && (exit_pt.x() <= cube.max_x()) &&
         (exit_pt.z() >= cube.min_z()) && (exit_pt.z() <= cube.max_z()) ) {
      face_id=Y_HIGH;
      return true;
    }
  }
  if (direction.z() < 0 ) {
    // intersect with low z plane
    lambda = (cube.min_z() - pt.z()) / direction.z();
    exit_pt = pt + direction * lambda;
    if ( (exit_pt.x() >= cube.min_x()) && (exit_pt.x() <= cube.max_x()) &&
         (exit_pt.y() >= cube.min_y()) && (exit_pt.y() <= cube.max_y()) ) {
      face_id=Z_LOW;
      return true;
    }
  }
  else if (direction.z() > 0 ) {
    // intersect with high z plane
    lambda = (cube.max_z() - pt.z()) / direction.z();
    exit_pt = pt + direction * lambda;
    if ( (exit_pt.x() >= cube.min_x()) && (exit_pt.x() <= cube.max_x()) &&
         (exit_pt.y() >= cube.min_y()) && (exit_pt.y() <= cube.max_y()) ) {
      face_id=Z_HIGH;
      return true;
    }
  }
  return false;
}


boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype
boxm_utils::obtain_mog_grey_unit_mode()
{
  bsta_gauss_sf1 simple_gauss_sf1(1.0f,0.1f);
  bsta_num_obs<bsta_gauss_sf1> simple_obs_gauss_val_sf1(simple_gauss_sf1,1);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>, 3>  simple_mix_gauss_val_sf1;

  simple_mix_gauss_val_sf1.insert(simple_obs_gauss_val_sf1,1.0f);

  typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>,3>  simple_bsta_mixture_fixed_sf1_3;
  bsta_num_obs<simple_bsta_mixture_fixed_sf1_3>  simple_obs_mix_gauss_val_sf1(simple_mix_gauss_val_sf1);

  return simple_obs_mix_gauss_val_sf1;
}

boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype
boxm_utils::obtain_mog_grey_zero_mode()
{
  bsta_gauss_sf1 simple_gauss_sf1(0.0f,0.1f);
  bsta_num_obs<bsta_gauss_sf1> simple_obs_gauss_val_sf1(simple_gauss_sf1,1);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>, 3>  simple_mix_gauss_val_sf1;

  simple_mix_gauss_val_sf1.insert(simple_obs_gauss_val_sf1,1.0f);

  typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>,3>  simple_bsta_mixture_fixed_sf1_3;
  bsta_num_obs<simple_bsta_mixture_fixed_sf1_3>  simple_obs_mix_gauss_val_sf1(simple_mix_gauss_val_sf1);

  return simple_obs_mix_gauss_val_sf1;
}

boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype
boxm_utils::obtain_mog_grey_single_mode(float  mean)
{
  bsta_gauss_sf1 simple_gauss_sf1(mean,0.1f);
  bsta_num_obs<bsta_gauss_sf1> simple_obs_gauss_val_sf1(simple_gauss_sf1,1);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>, 3>  simple_mix_gauss_val_sf1;

  simple_mix_gauss_val_sf1.insert(simple_obs_gauss_val_sf1,1.0f);

  typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>,3>  simple_bsta_mixture_fixed_sf1_3;
  bsta_num_obs<simple_bsta_mixture_fixed_sf1_3>  simple_obs_mix_gauss_val_sf1(simple_mix_gauss_val_sf1);

  return simple_obs_mix_gauss_val_sf1;
}

double boxm_utils::max_point_to_box_dist(vgl_box_3d<double> box,vgl_point_3d<double> pt)
{
  double dist_max=0;

  std::vector<vgl_point_3d<double> > corners=corners_of_box_3d(box);
  for (const auto & corner : corners)
  {
    if (dist_max<(corner-pt).length())
      dist_max=(corner-pt).length();
  }
  return dist_max;
}
