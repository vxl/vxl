#include "boxm_utils.h"
#include <boct/boct_tree_cell.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_clip.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_polygon_3d.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vnl/algo/vnl_determinant.h>

#define DEBUG 0


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
bool boxm_utils::is_face_visible(vcl_vector<vgl_point_3d<double> > &face,
                                 vpgl_camera_double_sptr const& camera)
{
  double u,v;
  //vgl_box_2d<double> face;
  vcl_vector<vgl_point_2d<double> > vs;

  assert(face.size() >= 3);

  for (unsigned i=0; i<face.size(); i++) {
    camera->project(face[i].x(), face[i].y(), face[i].z(), u, v);
    vs.push_back(vgl_point_2d<double>(u,v));
  }


  vgl_vector_2d<double> v0 = vs[1] - vs[0];
  vgl_vector_2d<double> v1 = vs[2] - vs[1];
  double normal = cross_product<double>(v0,v1);
  if (normal < 0)
    return true;
  return false;
}
vcl_vector<vgl_point_2d<double> >  
boxm_utils::project_face(vcl_vector<vgl_point_3d<double> > &face,
                                 vpgl_camera_double_sptr const& camera)
{
  double u,v;
  vcl_vector<vgl_point_2d<double> > vs;


  for (unsigned i=0; i<face.size(); i++) {
    camera->project(face[i].x(), face[i].y(), face[i].z(), u, v);
    vs.push_back(vgl_point_2d<double>(u,v));
  }

  return vs;
}

boct_face_idx
boxm_utils::visible_faces(vgl_box_3d<double> &bbox, vpgl_camera_double_sptr camera)
{
  boct_face_idx face_idx = boct_cell_face::NONE;
  if (camera->type_name().compare("vpgl_perspective_camera")==0) {
    vpgl_perspective_camera<double>* cam = static_cast<vpgl_perspective_camera<double>*>(camera.ptr());
    vgl_point_3d<double> const& cam_center = cam->camera_center();

    if (cam_center.x() > bbox.max_x()) {
      face_idx |= boct_cell_face::X_HIGH;
    }
    else if (cam_center.x() < bbox.min_x()) {
      face_idx |= boct_cell_face::X_LOW;
    }
    if (cam_center.y() > bbox.max_y()) {
      face_idx |= boct_cell_face::Y_HIGH;
    }
    else if (cam_center.y() < bbox.min_y()) {
      face_idx |= boct_cell_face::Y_LOW;
    }
    if (cam_center.z() > bbox.max_x()) {
      face_idx |= boct_cell_face::Z_HIGH;
    }
    else if (cam_center.z() < bbox.min_z()) {
      face_idx |= boct_cell_face::Z_LOW;
    }
  }
  // for other cameras, use projection and normals
  else {
    // fix the face normals so that the vertices are the counter clokwise order
    vcl_map<boct_face_idx, vcl_vector<vgl_point_3d<double> > > faces;
    faces_of_box_3d(bbox, faces);

    if (is_face_visible(faces.find(boct_cell_face::Z_LOW)->second, camera)) {
      face_idx |= boct_cell_face::Z_LOW;
#if DEBUG
      vcl_cout << "Z_LOW " ;
#endif
    }
    if (is_face_visible(faces.find(boct_cell_face::Z_HIGH)->second, camera)) {
      face_idx |= boct_cell_face::Z_HIGH;
#if DEBUG
      vcl_cout << "Z_HIGH " ;
#endif
    }
    if (is_face_visible(faces.find(boct_cell_face::X_LOW)->second, camera)) {
      face_idx |= boct_cell_face::X_LOW;
#if DEBUG
      vcl_cout << "X_LOW " ;
#endif
    }

    if (is_face_visible(faces.find(boct_cell_face::X_HIGH)->second, camera)) {
      face_idx |= boct_cell_face::X_HIGH;
#if DEBUG
      vcl_cout << "X_HIGH " ;
#endif
    }

    if (is_face_visible(faces.find(boct_cell_face::Y_LOW)->second, camera)) {
      face_idx |= boct_cell_face::Y_LOW;
#if DEBUG
      vcl_cout << "Y_LOW " ;
#endif
    }

    if (is_face_visible(faces.find(boct_cell_face::Y_HIGH)->second, camera)) {
      face_idx |= boct_cell_face::Y_HIGH;
#if DEBUG
      vcl_cout << "Y_HIGH " ;
#endif
    }
    
  }
#if DEBUG
  vcl_cout << vcl_endl;
#endif

  return face_idx;
}

//: returns the faces of a box, the vertices are ordered in the normal direction
void boxm_utils::faces_of_box_3d(vgl_box_3d<double> &bbox, 
                            vcl_map<boct_face_idx, vcl_vector<vgl_point_3d<double> > >& faces)
{
  vcl_vector<vgl_point_3d<double> > corners=corners_of_box_3d(bbox);
   
  // face bottom [1,0,3,2]
  vcl_vector<vgl_point_3d<double> > face_corners;
  face_corners.push_back(corners[1]);
  face_corners.push_back(corners[0]);
  face_corners.push_back(corners[3]);
  face_corners.push_back(corners[2]);
  faces[boct_cell_face::Z_LOW] = face_corners;

  face_corners.resize(0);
  face_corners.push_back(corners[4]);
  face_corners.push_back(corners[5]);
  face_corners.push_back(corners[6]);
  face_corners.push_back(corners[7]);
  faces[boct_cell_face::Z_HIGH] = face_corners;

  face_corners.resize(0);
  face_corners.push_back(corners[7]);
  face_corners.push_back(corners[3]);
  face_corners.push_back(corners[0]);
  face_corners.push_back(corners[4]);
  faces[boct_cell_face::X_LOW] = face_corners;

  face_corners.resize(0);
  face_corners.push_back(corners[1]);
  face_corners.push_back(corners[2]);
  face_corners.push_back(corners[6]);
  face_corners.push_back(corners[5]);
  faces[boct_cell_face::X_HIGH] = face_corners;

  face_corners.resize(0);
  face_corners.push_back(corners[0]);
  face_corners.push_back(corners[1]);
  face_corners.push_back(corners[5]);
  face_corners.push_back(corners[4]);
  faces[boct_cell_face::Y_LOW] = face_corners;

  face_corners.resize(0);
  face_corners.push_back(corners[2]);
  face_corners.push_back(corners[3]);
  face_corners.push_back(corners[7]);
  face_corners.push_back(corners[6]);
  faces[boct_cell_face::Y_HIGH] = face_corners;
}

//: returns the union of the projected faces of a polygon
void boxm_utils::project_cube(vgl_box_3d<double> &bbox, 
                              vpgl_camera_double_sptr camera,
                              vcl_map<boct_face_idx, vcl_vector< vgl_point_3d<double> > > & faces,
							  boct_face_idx & vis_face_ids)

{
	faces_of_box_3d(bbox, faces);
	vis_face_ids=boxm_utils::visible_faces(bbox,camera); 

}


void boxm_utils::quad_interpolate(vgl_polygon_scan_iterator<double> &poly_it, 
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
	while(poly_it.next()) {
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
		unsigned int startx = (unsigned int)vcl_max((int)0,poly_it.startx());
		unsigned int endx = (unsigned int)vcl_min((int)img.ni(),poly_it.endx());

		for (unsigned int x = startx; x < endx; ++x) {
			float interp_val = (float)(s0*x*y + s1*x + s2*y+s3);
			img(x,yu,img_plane_num) = interp_val;
		}
	}
	return;
}

void boxm_utils::quad_fill(vgl_polygon_scan_iterator<double> &poly_it, 
							 vil_image_view<float> &img, float val, unsigned img_plane_num)
{

	poly_it.reset();
	while(poly_it.next()) {
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
		unsigned int startx = (unsigned int)vcl_max((int)0,poly_it.startx());
		unsigned int endx = (unsigned int)vcl_min((int)img.ni(),poly_it.endx());

		for (unsigned int x = startx; x < endx; ++x) {
			img(x,yu,img_plane_num) = val;
		}
	}
	return;
}



bool boxm_utils::project_cube_xyz( vcl_map<boct_face_idx,vcl_vector< vgl_point_3d<double> > > & faces,
								   boct_face_idx & vis_face_ids,
								   vil_image_view<float> &front_xyz,
								   vil_image_view<float> &back_xyz,
								   vpgl_camera_double_sptr cam)
{
  vcl_map<boct_face_idx, vcl_vector<vgl_point_3d<double> > >::iterator face_it=faces.begin();
  for(;face_it!=faces.end();face_it++)
  {
	  vcl_vector<vgl_point_3d<double> > face_corners=face_it->second;
	  vcl_vector<vgl_point_2d<double> > face_projected=project_face(face_corners,cam);
	  vgl_polygon<double> face_polygon(face_projected);
	  vgl_polygon_scan_iterator<double> poly_it(face_polygon);

	  double xs[]={face_projected[0].x(),face_projected[1].x(),face_projected[2].x(),face_projected[3].x()};
	  double ys[]={face_projected[0].y(),face_projected[1].y(),face_projected[2].y(),face_projected[3].y()};

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





bool boxm_utils::project_cube_fill_val( vcl_map<boct_face_idx,vcl_vector< vgl_point_3d<double> > > & faces,
								   boct_face_idx & vis_face_ids,
								   vil_image_view<float> &fill_img,
								   float val, vpgl_camera_double_sptr cam)
{
  vcl_map<boct_face_idx, vcl_vector<vgl_point_3d<double> > >::iterator face_it=faces.begin();
  for(;face_it!=faces.end();face_it++)
  {
	  vcl_vector<vgl_point_3d<double> > face_corners=face_it->second;
	  vcl_vector<vgl_point_2d<double> > face_projected=project_face(face_corners,cam);
	  vgl_polygon<double> face_polygon(face_projected);
	  vgl_polygon_scan_iterator<double> poly_it(face_polygon);

	  double xs[]={face_projected[0].x(),face_projected[1].x(),face_projected[2].x(),face_projected[3].x()};
	  double ys[]={face_projected[0].y(),face_projected[1].y(),face_projected[2].y(),face_projected[3].y()};


	  if (vis_face_ids & face_it->first){
		  quad_fill(poly_it,fill_img,val,0);
	  }
  }
          
 return true; 
}




