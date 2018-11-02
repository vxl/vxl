// This is brl/bbas/bpgl/icam/icam_minimizer.h
#ifndef icam_minimizer_h_
#define icam_minimizer_h_
//:
// \file
// \brief Find a camera using the depth image of known camera and minimizing squared intensity differences or mutual information
//
// The camera is determined by minimizing the sum of squared differences or
// mutual information between a mapped source image and a destination image.
// The destination camera is assumed to have the form K[I|0], i.e., the world
// coordinate system is the same as the camera frame. A depth map, Z(u,v),
// is given for the known camera. The rotation and translation parameters for
// the unknown source camera with respect to the destination identity camera
// are adjusted so as to minimize the least squared difference in intensity
// between the mapped source image and the destination image. The mapping is
// computed using the depth map as described in icam_depth_transform.h
//
// \author J.L. Mundy
// \date Sept. 6, 2010
//
// \verbatim
//  Modifications
//   None
// \endverbatim
//
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_math.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pyramid_image_view.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vbl/vbl_array_3d.h>
#include <vsph/vsph_camera_bounds.h>
#include <icam/icam_depth_transform.h>
#include <icam/icam_depth_trans_pyramid.h>
#include <icam/icam_cost_func.h>
#include <icam/icam_minimizer_params.h>

//: Minimize registration error at multiple scales
class icam_minimizer
{
 public:
  //: Constructor with all the parameters
  icam_minimizer( const vil_image_view<float>& source_img,
                  const vil_image_view<float>& dest_img,
                  const icam_depth_transform& dt,
                  icam_minimizer_params const& params,
                  bool verbose=false);

  //: Constructor, when source image is not known yet
  icam_minimizer(const vil_image_view<float>& dest_img,
                 const icam_depth_transform& dt,
                 icam_minimizer_params const& params,
                 bool verbose=false);

  virtual ~icam_minimizer() = default;

  //: number of pyramid levels
  unsigned n_levels() const {return dt_pyramid_.n_levels();}

  //: in the cases where source image is not known at construction, the image is set later
  void set_source_img(const vil_image_view<float>& source_img);

  bool verbose() {return verbose_;}

  //: print parameters
  void print_params();

  //: principal ray iterator for exhaustive search, scans over a set of principal rays lying inside a cone computed for an image at the specified level
  principal_ray_scan pray_scan(unsigned level, unsigned& n_pts);

  //: polar angle increment for exhaustive search, the polar range is typically -pi <= a <= pi
  double polar_inc(unsigned level, unsigned& nsteps,
                   double polar_range = vnl_math::pi);

  //: principal ray iterator for initialized search
  principal_ray_scan initialized_pray_scan(unsigned initial_level,
                                           unsigned search_level,
                                           unsigned& n_pts);

  //: polar angle increment for initialized search
  void initialized_polar_inc(unsigned initial_level,
                             unsigned search_level,
                             unsigned& nsteps,
                             double& polar_range,
                             double& polar_inc);

  //: Run the minimization starting at input values
  void minimize(vgl_rotation_3d<double>& rot,
                vgl_vector_3d<double>& trans);

  //: Run the minimization, adjusting focal length, starting at input values
  void minimize(vgl_rotation_3d<double>& rot,
                vgl_vector_3d<double>& trans,
                double to_fl);

  //: Run powell minimization on rotation only using entropy diff for a level
  void minimize_rot(vgl_rotation_3d<double>& rot,
                    vgl_vector_3d<double> const& trans,
                    unsigned level,
                    double min_allowed_overlap = 0.01);

  double end_error() { return end_error_; }

  //: current parameter values
  double to_fl() {return dt_pyramid_.to_fl();}
  vgl_rotation_3d<double> rotation() {return dt_pyramid_.rotation();}
  vgl_vector_3d<double> translation() {return dt_pyramid_.translation();}

  //: a search over rotations at a given level of the pyramid
 bool rot_search(vgl_vector_3d<double> const& trans,
                 vgl_rotation_3d<double>& initial_rot,
                 unsigned n_axis_steps,
                 double axis_cone_half_angle,
                 unsigned n_polar_steps,
                 double polar_range,
                 unsigned level,
                 double min_allowed_overlap,
                 vgl_rotation_3d<double>& min_rot,
                 double& min_cost,
                 double& min_overlap_fraction);
  //: exhaustive search for rotation, given the camera translation
  // This virtual method is implemented in both C++ and in OpenCL
  // setup and finish are particular to OpenCL to signal setup and
  // finish of GPU context and buffers
  virtual bool exhaustive_rotation_search(vgl_vector_3d<double> const& trans,
                                          unsigned level,
                                          double min_allowed_overlap,
                                          vgl_rotation_3d<double>& min_rot,
                                          double& min_cost,
                                          double& min_overlap_fraction,
                                          bool setup,
                                          bool finish);

  //: search for rotation about a given initial value, given the camera translation
  // This virtual method is implemented in both C++ and in OpenCL
  // setup and finish are particular to OpenCL to signal setup and
  // finish of GPU context and buffers
 virtual bool initialized_rot_search(vgl_vector_3d<double> const& trans,
                                     vgl_rotation_3d<double>& initial_rot,
                                     unsigned initial_level,
                                     unsigned search_level,
                                     double min_allowed_overlap,
                                     vgl_rotation_3d<double>& min_rot,
                                     double& min_cost,
                                     double& min_overlap_fraction,
                                     bool setup,
                                     bool finish);

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
                                double& min_overlap_fraction);

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
  bool camera_search( vgl_box_3d<double> const& trans_box,
                      vgl_vector_3d<double> const& trans_steps,
                      unsigned final_level,
                      double min_allowed_overlap,
                      bool refine,
                      vgl_vector_3d<double>& min_trans,
                      vgl_rotation_3d<double>& min_rot,
                      double& min_error,
                      double& min_overlap);

  //: find the smallest cost in the camera box search that is also a local minimum.
  //  Return false if no local minima occur
  bool smallest_local_minimum(double nbhd_cost_threshold,
                              double& min_cost,
                              vgl_vector_3d<double>& min_trans,
                              vgl_rotation_3d<double>& min_rot,
                              int& ix_min, int& iy_min,
                              int& iz_min);

  // ===============debug functions ===========

  //: the average intensity difference for a given rotation, translation and level
  double error(vgl_rotation_3d<double>& rot,
               vgl_vector_3d<double>& trans, unsigned level);

  //: the average intensity difference for a given rotation, translation and level over a set of parameter values
  std::vector<double> error(vgl_rotation_3d<double>& rot,
                           vgl_vector_3d<double>& trans, unsigned level,
                           unsigned param_index, double pmin,
                           double pmax, double pinc);

  //: source images mapped to destination camera for a set of parameter values
  std::vector<vil_image_view<float> > views(vgl_rotation_3d<double>& rot,
                                           vgl_vector_3d<double>& trans,
                                           unsigned level,
                                           unsigned param_index, double pmin,
                                           double pmax, double pinc);

  //:source image mapped to destination by specified source cam
  vil_image_view<float> view(vgl_rotation_3d<double>& rot,
                             vgl_vector_3d<double>& trans,
                             unsigned level);
  //:mask for source image mapped to destination by specified source cam
  vil_image_view<float> mask(vgl_rotation_3d<double>& rot,
                             vgl_vector_3d<double>& trans,
                             unsigned level);

  //: source camera relative to destination coordinate system at level
  vpgl_perspective_camera<double> source_cam(unsigned level);

  //: source image at level
  vil_image_view<float> source(unsigned level) {return source_pyramid_(level);}

  //: destination image at level
  vil_image_view<float> dest(unsigned level) {return dest_pyramid_(level);}

  //: depth image at level
  vil_image_view<double> depth(unsigned level) {return dt_pyramid_.depth(level);}

  //: inverse depth image at level
  vil_image_view<double> inv_depth(unsigned level);

  //: the cost function for a given level
  icam_cost_func cost_fn(unsigned level);

  //: the depth transform for a given level
  icam_depth_transform depth_trans(unsigned level) {return dt_pyramid_.depth_trans(level);}

  //: display box search as a set of vrml spheres
  bool box_search_vrml(std::string const& vrml_file,
                       vgl_vector_3d<double> const& trans =
                       vgl_vector_3d<double>());
  vnl_matrix_fixed<double, 3, 3> to_calibration_matrix(unsigned level);
  vnl_matrix_fixed<double, 3, 3> from_calibration_matrix_inv(unsigned level);

  void set_actual_translation(vgl_vector_3d<double> const& trans) {actual_trans_ = trans;}

  void set_actual_rotation(vgl_rotation_3d<double> const& rot) {actual_rot_ = rot;}

  void print_axis_search_info(unsigned level, vgl_rotation_3d<double> const& actual,
                              vgl_rotation_3d<double> const& init = vgl_rotation_3d<double>(),
                              bool top_level = false);

  void print_polar_search_info(unsigned level, vgl_rotation_3d<double> const& actual,
                               vgl_rotation_3d<double> const& init = vgl_rotation_3d<double>(),
                               bool top_level = false);
 protected:
  void set_origin_step_delta(vgl_box_3d<double> const& trans_box,
                             vgl_vector_3d<double> const& trans_steps);
  // |<--- k --->|
  // [   |   |   | * |   |   |   ]
  // |dx |
  // |< --initial step delta --->|
  void reduce_search_box(vgl_vector_3d<double> const& center_trans,
                         vgl_vector_3d<double> const& initial_step_delta);

  icam_minimizer_params params_;
  bool cam_search_valid_;
  vgl_vector_3d<double> box_origin_;
  vgl_vector_3d<double> step_delta_;
  vbl_array_3d<double> box_scores_;
  vbl_array_3d<vgl_rotation_3d<double> > box_rotations_;
  vil_pyramid_image_view<float> source_pyramid_;
  vil_pyramid_image_view<float> dest_pyramid_;
  icam_depth_trans_pyramid dt_pyramid_;
  double end_error_;
  std::string base_path_;
  bool verbose_;
  vgl_vector_3d<double> actual_trans_;
  vgl_rotation_3d<double> actual_rot_;
};

#endif // icam_minimizer_h_
