#ifndef boxm_utils_h_
#define boxm_utils_h_
//:
// \file
// \brief  The utility methods for the boxm project
//
// \author Gamze Tunali
// \date Apr 07, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_camera.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include <boct/boct_tree_cell.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vil/vil_image_view.h>

class boxm_utils
{
 public:

  //: returns true if a given box is visible from the camera and the image plane
  static bool is_visible(vgl_box_3d<double> const& bbox,
                         vpgl_camera_double_sptr const& camera,
                         unsigned int img_ni, unsigned int img_nj,
                         bool do_front_test = true);

  // returns the 8 corner points of a box
  static vcl_vector<vgl_point_3d<double> > corners_of_box_3d(vgl_box_3d<double> const& box);

  //: returns the indices of the faces visible from the camera
  static bool is_face_visible(vcl_vector<vgl_point_3d<double> > &face,
                              vpgl_camera_double_sptr const& camera);

  //: 
  static bool is_face_visible(double * xverts, double *yerts,
                              unsigned id1,unsigned id2,unsigned id3,unsigned id4);

  //: returns the visible faces of a box given a camera.
  // It puts a bit 1 for each face visible based on the boct_cell_face values.
  static boct_face_idx visible_faces(vgl_box_3d<double> const& bbox,
                                     vpgl_camera_double_sptr camera,
						                         double *xverts, double *yerts);

  static boct_face_idx visible_faces(vgl_box_3d<double> const& bbox, vpgl_camera_double_sptr camera);

  static void faces_of_box_3d(vgl_box_3d<double> const& bbox,
                              vcl_map<boct_face_idx, vcl_vector<vgl_point_3d<double> > > & faces);

  static void project_corners(vcl_vector<vgl_point_3d<double> > & corners,vpgl_camera_double_sptr camera,
							  double * xverts, double *yerts);

  static void project_cube(vgl_box_3d<double> &bbox,
                           vpgl_camera_double_sptr camera,
                           vcl_map<boct_face_idx, vcl_vector< vgl_point_3d<double> > > & faces,
                           boct_face_idx & vis_face_ids);

  static vcl_vector<vgl_point_2d<double> >  project_face(vcl_vector<vgl_point_3d<double> > &face,
                                                         vpgl_camera_double_sptr const& camera);

  static void quad_interpolate(vgl_polygon_scan_iterator<double> &poly_it,
                               double* xvals, double* yvals, double* vals,
                               vil_image_view<float> &img,unsigned img_plane_num=0,
                               unsigned int v0=0, unsigned int v1=1,
                               unsigned int v2=2,unsigned int v3=3);

  static void quad_fill(vgl_polygon_scan_iterator<double> &poly_it,
                        vil_image_view<float> &img, float val,
                        unsigned img_plane_num=0);

  static bool project_cube_xyz(vcl_map<boct_face_idx,vcl_vector< vgl_point_3d<double> > > & faces,
                               boct_face_idx & vis_face_ids,
                               vil_image_view<float> &front_xyz,
                               vil_image_view<float> &back_xyz,
                               vpgl_camera_double_sptr camera);

 static bool project_cube_xyz(vcl_vector< vgl_point_3d<double> >  & corners,
								  boct_face_idx & vis_face_ids,
								  vil_image_view<float> &front_xyz,
								  vil_image_view<float> &back_xyz,
								  double *xverts,double * yerts);

  static bool project_cube_fill_val( vcl_map<boct_face_idx,vcl_vector< vgl_point_3d<double> > > & faces,
                                     boct_face_idx & vis_face_ids,
                                     vil_image_view<float> &fill_img,
                                     float val, vpgl_camera_double_sptr cam);

  static bool project_cube_fill_val(boct_face_idx & vis_face_ids,
									   vil_image_view<float> &fill_img,
									   float val, double *xverts,double * yverts);

  static bool cube_uniform_mean(boct_face_idx & vis_face_ids,
								vil_image_view<float> &img,
								float & val, double *xverts,double * yverts);

  static void quad_mean(vgl_polygon_scan_iterator<double> &poly_it,
                        vil_image_view<float> &img, float &val,  int & count);


};

#endif
