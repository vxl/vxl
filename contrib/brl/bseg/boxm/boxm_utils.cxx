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
#include <vcl_cmath.h>
#include <vcl_algorithm.h>

#include <vnl/algo/vnl_determinant.h>
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
//: corners of the input face
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
void boxm_utils::quad_interpolate(vgl_polygon_scan_iterator<double> &poly_it, 
							 double* xvals, double* yvals, double* vals, 
							 vil_image_view<float> &min_img, vil_image_view<float> &max_img,
							 unsigned int v0, unsigned int v1, 
							 unsigned int v2,unsigned int v3)
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
	while(poly_it.next()) {
		int y = poly_it.scany();
		if (y < 0){
			// not inside of image bounds yet. go to next scanline.
			continue;
		}
		unsigned int yu = (unsigned int)y;
		if (yu >= max_img.nj() ) {
			// we have left the image bounds. no need to continue.
			break;
		}
		if ( (poly_it.startx() >= (int)max_img.ni()) || (poly_it.endx() <= 0) ) {
			// no part of this scanline is within the image bounds. go to next scanline.
			continue;
		}
		unsigned int startx = (unsigned int)vcl_max((int)0,poly_it.startx());
		unsigned int endx = (unsigned int)vcl_min((int)max_img.ni(),poly_it.endx());

		for (unsigned int x = startx; x < endx; ++x) {
			float interp_val = (float)(s0*x*y + s1*x + s2*y+s3);
			if ( (min_img(x,yu) == 0) || (min_img(x,yu) > interp_val) ){
				min_img(x,yu) = interp_val;
			}
			if (max_img(x,yu) < interp_val) {
				max_img(x,yu) = interp_val;
			}
		}
	}
	return;
}

//void project_cube_vertices(vbl_box_3d<double> const &cube, vpgl_perspective_camera<double> const& cam, double* xverts_2d, double* yverts_2d, float* vert_distances, psm_cube_face_list &visible_faces)
//{
//  // 3d coordinates of cube
//  double xverts_3d[] = {cube.xmin(), cube.xmax()};
//  double yverts_3d[] = {cube.ymin(), cube.ymax()};
//  double zverts_3d[] = {cube.zmin(), cube.zmax()};
//
//  vgl_point_3d<double> cam_center = vgl_point_3d<double>(cam.camera_center());
//
//  unsigned int n=0;
//  for (unsigned int k=0; k<2; ++k) {
//    for (unsigned int j=0; j<2; ++j) {
//      for (unsigned int i=0; i<2; ++i) {
//        cam.project(xverts_3d[i],yverts_3d[j],zverts_3d[k],xverts_2d[n],yverts_2d[n]);
//        vert_distances[n] =  (float)(cam_center - vgl_point_3d<double>(xverts_3d[i],yverts_3d[j],zverts_3d[k])).length();
//        ++n;
//      }
//    }
//  }
//
//  visible_faces = psm_cube_face::NONE;
//  if (visible(xverts_2d,yverts_2d, 2,0,6)) {
//    visible_faces |= psm_cube_face::X_LOW;
//  }
//  if (visible(xverts_2d,yverts_2d, 1,3,7)) {
//    visible_faces |= psm_cube_face::X_HIGH;
//  }
//  if (visible(xverts_2d,yverts_2d, 0,1,5)) {
//    visible_faces |= psm_cube_face::Y_LOW;
//  }
//  if (visible(xverts_2d,yverts_2d, 7,3,2)) {
//    visible_faces |= psm_cube_face::Y_HIGH;
//  }
//  if (visible(xverts_2d,yverts_2d, 0,2,1)) {
//    visible_faces |= psm_cube_face::Z_LOW;
//  }
//  if (visible(xverts_2d,yverts_2d, 4,5,6)) {
//    visible_faces |= psm_cube_face::Z_HIGH;
//  }
//}
//
//
//bool cube_camera_distance(vbl_box_3d<double> const& cube,
//						  vpgl_perspective_camera<double> const& cam, 
//						  vil_image_view<float> &front_dist, 
//						  vil_image_view<float> &back_dist,
//						  psm_cube_face_list visible_faces)
//{
//  // 3d coordinates of cube
//  double xverts_3d[] = {cube.xmin(), cube.xmax()};
//  double yverts_3d[] = {cube.ymin(), cube.ymax()};
//  double zverts_3d[] = {cube.zmin(), cube.zmax()};
//  // 2d projected coordinates of cube
//  double xverts_2d[8], yverts_2d[8];
//  // distances from camera center of 3d vertices
//  float vert_distances[8];
//
//  vgl_point_3d<double> cam_center = vgl_point_3d<double>(cam.camera_center());
//
//  unsigned int n=0;
//  for (unsigned int k=0; k<2; ++k) {
//    for (unsigned int j=0; j<2; ++j) {
//      for (unsigned int i=0; i<2; ++i) {
//        cam.project(xverts_3d[i],yverts_3d[j],zverts_3d[k],xverts_2d[n],yverts_2d[n]);
//        vert_distances[n] =  (float)(cam_center - vgl_point_3d<double>(xverts_3d[i],yverts_3d[j],zverts_3d[k])).length();
//        ++n;
//      }
//    }
//  }
//  // for each face, create two triangle iterators and fill in pixel data
//  vgl_triangle_scan_iterator<double> tri_it;
//  vgl_point_2d<double> tri_verts[3];
//
//  // X_LOW
//  // tri 0
//  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 2, 0, 6);
//  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 2, 0, 6);
//
//  // tri 1
//  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 0, 4, 6);
//  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 0, 4, 6);
//
//  // X_HIGH
//  // tri 0
//  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 1, 3, 7);
//  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 1, 3, 7);
//  // tri 1
//  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 7, 5, 1);
//  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 7, 5, 1);
//
//  // Y_LOW
//  // tri 0
//  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 0, 1, 5);
//  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 0, 1, 5);
//  // tri 1
//  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 4, 0, 5);
//  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 4, 0, 5);
//
//  // Y_HIGH
//  // tri 0
//  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 7, 3, 2);
//  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 7, 3, 2);
//  // tri 1
//  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 2, 6, 7);
//  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 2, 6, 7);
//
//  // Z_LOW
//  // tri 0
//  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 0, 1, 2);
//  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 0, 2, 1);
//  // tri 1
//  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 1, 3, 2);
//  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 1, 2, 3);
//
//  // Z_HIGH
//  // tri 0
//  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 4, 5, 6);
//  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 4, 5, 6);
//  // tri 1
//  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 5, 7, 6);
//  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 5, 7, 6);
//
//  return true;
//
//}
//
//
//
