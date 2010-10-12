// This is vpgl/icam/icam_minimizer.h
#ifndef icam_minimizer_h_
#define icam_minimizer_h_
//:
// \file
// \brief Find a camera using the depth image of known camera and minimizing squared intensity differences 
// \author J.L. Mundy
// \date Sept. 6, 2010
//
// \verbatim
//  Modifications
//   None
// \endverbatim
// The camera is determined by minimizing the
// sum of squared differences with respect to the image of a known camera.
// The known camera is assumed to have the form K[I|0], i.e., the world 
// coordinate system is the same as the camera frame. A depth map, Z(u,v),
// is given for the known camera. The rotation and translation parameters for
// the unknown (to) camera with respect to the known (from) camera are
// adjusted so as to minimize the least squared difference in intensity between
// the from_image and to_image.
// 
#include <vcl_string.h>
#include <vnl/vnl_vector.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pyramid_image_view.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vbl/vbl_array_3d.h>
#include <icam/icam_depth_transform.h>
#include <icam/icam_depth_trans_pyramid.h>
#include <icam/icam_cost_func.h>
//: Minimize registration error at mulitiple scales 
class icam_minimizer 
{
 public:
  //: Constructor with all the parameters
  icam_minimizer( const vil_image_view<float>& source_img,
                  const vil_image_view<float>& dest_img,
                  const icam_depth_transform& dt,
                  unsigned min_level_size = 16,
                  unsigned box_reduction_k = 2,
                  double local_min_thresh = 0.05,
                  vcl_string const& base_path = "");
  //: Constructor, when source image is not known yet
  icam_minimizer(const vil_image_view<float>& dest_img,
                 const icam_depth_transform& dt,
                 unsigned min_level_size = 16,
                 unsigned box_reduction_k = 2,
                  double local_min_thresh = 0.05,
                  vcl_string const& base_path = "");
  //: number of pyramid levels
  unsigned n_levels() const {return dt_pyramid_.n_levels();}

  //: in the cases where source image is not know at construction, the image is set later
  void set_source_img(const vil_image_view<float>& source_img);


  //: Run the minimization starting at input values
  void minimize(vgl_rotation_3d<double>& rot,
                vgl_vector_3d<double>& trans);

  //: Run the minimization, adjusting focal length, starting at input values
  void minimize(vgl_rotation_3d<double>& rot,
                vgl_vector_3d<double>& trans,
                double to_fl);

  double end_error(){return end_error_;}


  //: current parameter values
  double to_fl(){return dt_pyramid_.to_fl();}
  vgl_rotation_3d<double> rotation(){return dt_pyramid_.rotation();}
  vgl_vector_3d<double> translation(){return dt_pyramid_.translation();}

  //: exhaustive search for rotation, given the camera translation
  bool exhaustive_rotation_search(vgl_vector_3d<double> const& trans,
                                  unsigned level,
                                  double min_allowed_overlap,
                                  vgl_rotation_3d<double>& min_rot,
                                  double& min_cost,
                                  double& min_overlap_fraction);

  bool initialized_rot_search(vgl_vector_3d<double> const& trans,
                              vgl_rotation_3d<double>& initial_rot,
                              unsigned initial_level,
                              unsigned search_level,
                              double min_allowed_overlap,
                              vgl_rotation_3d<double>& min_rot,
                              double& min_cost,
                              double& min_overlap_fraction);

  bool refine_minimum(int mx, int my, int mz,
                      unsigned level,
                      double min_allowed_overlap,
                      vgl_vector_3d<double>& min_trans,
                      double& min_cost);
             

 bool exhaustive_camera_search(vgl_box_3d<double> const& trans_box,
                               vgl_vector_3d<double> const& trans_steps,
                               unsigned level,
                               double min_allowed_overlap,
                               vgl_vector_3d<double>& min_trans,
                               vgl_rotation_3d<double>& min_rot,
                               double& min_cost,
                               double& min_overlap_fraction
                               );

bool  pyramid_camera_search(vgl_vector_3d<double> const&
                            start_trans,
                            vgl_rotation_3d<double> const& 
                            start_rotation,
                            vgl_vector_3d<double> const&
                            start_step_delta,
                            unsigned start_level,
                            unsigned final_level,
                            double min_allowed_overlap,
                            bool refine,
                            vgl_vector_3d<double>& min_trans,
                            vgl_rotation_3d<double>& min_rot,
                            double& min_cost,
                            double& min_overlap_fraction);

//: the full search function
 bool  camera_search( vgl_box_3d<double> const& trans_box,
                      vgl_vector_3d<double> const& trans_steps,
                      unsigned final_level,
                      double min_allowed_overlap,
                      bool refine,
                      vgl_vector_3d<double>& min_trans,
                      vgl_rotation_3d<double>& min_rot,
                      double& min_error,
                      double& min_overlap);
                                
  //: find the smallest cost in the camera box search that is also a 
  //  local minimum.  Return false if no local minima occur
  bool smallest_local_minimum(double nbhd_cost_threshold,
                              double& min_cost,
                              vgl_vector_3d<double>& min_trans,
                              vgl_rotation_3d<double>& min_rot,
                              int& ix_min, int& iy_min,
                              int& iz_min
                              );

  // ===============debug functions ===========
  //: the average intensity difference for a given rotation,
  //  translation and level
  double error(vgl_rotation_3d<double>& rot,
               vgl_vector_3d<double>& trans, unsigned level);

  //: the average intensity difference for a given rotation,
  //  translation and level over a set of parameter values
  vcl_vector<double> error(vgl_rotation_3d<double>& rot,
                           vgl_vector_3d<double>& trans, unsigned level,
                           unsigned param_index, double pmin,
                           double pmax, double pinc);

  //:source images mapped to destination camera
  // for a set of parameter values
  vcl_vector<vil_image_view<float> > views(vgl_rotation_3d<double>& rot,
                                           vgl_vector_3d<double>& trans, 
                                           unsigned level,
                                           unsigned param_index, double pmin,
                                           double pmax, double pinc);

  //:source image mapped to destination camera
  vil_image_view<float> view(vgl_rotation_3d<double>& rot,
                             vgl_vector_3d<double>& trans, 
                             unsigned level);
  //:mask for source image mapped to destination camera
  vil_image_view<float> mask(vgl_rotation_3d<double>& rot,
                             vgl_vector_3d<double>& trans, 
                             unsigned level);
  //: destination camera at level
  vpgl_perspective_camera<double> dest_cam(unsigned level);

  //:destination image at level
  vil_image_view<float> dest(unsigned level){return dest_pyramid_(level);}

  //: the cost function for a given level
  icam_cost_func cost_fn(unsigned level);

  //: display box search as a set of vrml spheres
  bool box_search_vrml(vcl_string const& vrml_file,
                       vgl_vector_3d<double> const& trans =
                       vgl_vector_3d<double>()
                       );

 protected:
  void set_origin_step_delta(vgl_box_3d<double> const&_trans_box,
                             vgl_vector_3d<double> const& trans_steps);
  // |<--- k --->|              
  // [   |   |   | * |   |   |   ]
  // |dx | 
  // |< --initial step delta --->|
  void reduce_search_box(vgl_vector_3d<double> const& center_trans,
                         vgl_vector_3d<double> const& initial_step_delta);
  unsigned box_reduction_k_;//defined above
  bool cam_search_valid_;
  double local_min_thresh_;
  vgl_vector_3d<double> box_origin_;
  vgl_vector_3d<double> step_delta_;
  vbl_array_3d<double> box_scores_;
  vbl_array_3d<vgl_rotation_3d<double> > box_rotations_;
  vil_pyramid_image_view<float> source_pyramid_;
  vil_pyramid_image_view<float> dest_pyramid_;
  icam_depth_trans_pyramid dt_pyramid_;
  unsigned min_level_size_;
  double end_error_; 
  vcl_string base_path_;
};

#endif // icam_minimizer_h_

