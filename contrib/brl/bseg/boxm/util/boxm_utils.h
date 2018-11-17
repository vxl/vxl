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

#include "boxm_quad_scan_iterator.h"
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <boct/boct_tree_cell.h>
#include <boxm/boxm_apm_traits.h>
#include <vil/vil_image_view.h>

class safe_inverse_functor
{
 public:
  safe_inverse_functor(float tol = 0.0f) : tol_(tol) {}

  float operator()(float x) const { return (x > tol_)? 1.0f/x : 0.0f; }

  float tol_;
};

class boxm_utils
{
 public:

  //: returns true if a given box is visible from the camera and the image plane
  static bool is_visible(vgl_box_3d<double> const& bbox,
                         vpgl_camera_double_sptr const& camera,
                         unsigned int img_ni, unsigned int img_nj,
                         bool do_front_test = true);

  // returns the 8 corner points of a box
  static std::vector<vgl_point_3d<double> > corners_of_box_3d(vgl_box_3d<double> const& box);

  //: returns the indices of the faces visible from the camera
  static bool is_face_visible(std::vector<vgl_point_3d<double> > &face,
                              vpgl_camera_double_sptr const& camera);

  //:
  static bool is_face_visible(const double * xverts, const double *yerts,
                              unsigned id1,unsigned id2,unsigned id3,unsigned id4);

  //: returns the visible faces of a box given a camera.
  // It puts a bit 1 for each face visible based on the boct_cell_face values.
  static boct_face_idx visible_faces(vgl_box_3d<double> const& bbox,
                                     const vpgl_camera_double_sptr& camera,
                                     double *xverts, double *yerts);
  static boct_face_idx visible_faces_cell(vgl_box_3d<double> const& bbox,
                                          const vpgl_camera_double_sptr& camera,
                                          double *xverts, double *yverts);

  static boct_face_idx visible_faces(vgl_box_3d<double> const& bbox, const vpgl_camera_double_sptr& camera);

  static void faces_of_box_3d(vgl_box_3d<double> const& bbox,
                              std::map<boct_face_idx, std::vector<vgl_point_3d<double> > > & faces);

  static void project_corners(std::vector<vgl_point_3d<double> > const& corners,const vpgl_camera_double_sptr& camera,
                              double * xverts, double *yerts);
  static void project_point3d(vgl_point_3d<double> const& point,
                              const vpgl_camera_double_sptr& camera,
                              double & xvert, double &yert, double & vertdist);

  static void project_corners(std::vector<vgl_point_3d<double> > const& corners,
                              const vpgl_camera_double_sptr& camera,
                              double* xverts, double* yverts,
                              float* vertdist);
  static void project_cube(vgl_box_3d<double> &bbox,
                           const vpgl_camera_double_sptr& camera,
                           std::map<boct_face_idx, std::vector< vgl_point_3d<double> > > & faces,
                           boct_face_idx & vis_face_ids);

  static std::vector<vgl_point_2d<double> >  project_face(std::vector<vgl_point_3d<double> > &face,
                                                         vpgl_camera_double_sptr const& camera);

  static void quad_interpolate(boxm_quad_scan_iterator &poly_it,
                               double* xvals, double* yvals, double* vals,
                               vil_image_view<float> &img,unsigned img_plane_num=0,
                               unsigned int v0=0, unsigned int v1=1,
                               unsigned int v2=2,unsigned int v3=3);

  static void quad_fill(boxm_quad_scan_iterator &poly_it,
                        vil_image_view<float> &img, float val,
                        unsigned img_plane_num=0);
  static void quad_fill(boxm_quad_scan_iterator &poly_it,
                        vil_image_view<float> &img,
                        vil_image_view<float> &weights_img,
                        float val,
                        unsigned img_plane_num=0);

  static bool project_cube_xyz(std::map<boct_face_idx,std::vector< vgl_point_3d<double> > > & faces,
                               boct_face_idx & vis_face_ids,
                               vil_image_view<float> &front_xyz,
                               vil_image_view<float> &back_xyz,
                               const vpgl_camera_double_sptr& camera);

 static bool project_cube_xyz(std::vector< vgl_point_3d<double> >  & corners,
                              boct_face_idx & vis_face_ids,
                              vil_image_view<float> &front_xyz,
                              vil_image_view<float> &back_xyz,
                              double *xverts,double * yerts);
 //: only for perspective camera
 static bool project_cube_xyz(std::vector< vgl_point_3d<double> > & corners,
                              boct_face_idx & vis_face_ids,
                              vil_image_view<float> &front_xyz,
                              vil_image_view<float> &back_xyz,
                              double *xverts,double * yverts,float * vertdist);

  static bool project_cube_fill_val( std::map<boct_face_idx,std::vector< vgl_point_3d<double> > > & faces,
                                     boct_face_idx & vis_face_ids,
                                     vil_image_view<float> &fill_img,
                                     float val, const vpgl_camera_double_sptr& cam);

  static bool project_cube_fill_val(boct_face_idx & vis_face_ids,
                                    vil_image_view<float> &fill_img,
                                    float val, double *xverts,double * yverts);
  static bool project_cube_fill_val_aa(boct_face_idx & vis_face_ids,
                                       vil_image_view<float> &fill_img,
                                       vil_image_view<float> &weights_img,
                                       float val, double *xverts,double * yverts);

  static bool cube_uniform_mean(boct_face_idx & vis_face_ids,
                                vil_image_view<float> &img,
                                float & val, double *xverts,double * yverts);
  static bool cube_weighted_mean(boct_face_idx & vis_face_ids,
                                 vil_image_view<float> &img,
                                 vil_image_view<float> &weight_img,
                                 float & val, double *xverts,double * yverts);

  static bool cube_sum(boct_face_idx & vis_face_ids,
                       vil_image_view<float> &img,
                       float & val, double *xverts,double * yverts);

  static void quad_mean(boxm_quad_scan_iterator &poly_it,
                        vil_image_view<float> &img, float &val,  float & count);
  static void quad_sum(boxm_quad_scan_iterator &poly_it,
                       vil_image_view<float> &img, float &val);
  static void quad_weighted_mean(boxm_quad_scan_iterator &poly_it,
                                 vil_image_view<float> &img,
                                 vil_image_view<float> &weighted_img,
                                 float &numerator,  float & denominator);
  static bool cube_entry_point(vgl_box_3d<double> box,vgl_point_3d<double> pt,
                               vgl_vector_3d<double> dir,
                               vgl_point_3d<double> & entry_point,
                               double & lambda, boct_face_idx & face_id);
  static bool cube_exit_point(vgl_box_3d<double> box,vgl_point_3d<double> pt,
                              vgl_vector_3d<double> dir, vgl_point_3d<double> & exit_point,
                              double & lambda,boct_face_idx & face_id);

  static double max_point_to_box_dist(vgl_box_3d<double> box,vgl_point_3d<double> pt);

  static boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype obtain_mog_grey_unit_mode();

  static boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype obtain_mog_grey_zero_mode();

  static boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype obtain_mog_grey_single_mode(float  mean);
};

#endif
