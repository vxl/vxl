// This is vpgl/algo/vpgl_bundle_adjust.h
#ifndef vpgl_bundle_adjust_h_
#define vpgl_bundle_adjust_h_
//:
// \file
// \brief Bundle adjustment sparse least squares functions
// \author Matt Leotta
// \date April 18, 2005
// \verbatim
//  Modifications
//   Mar 23, 2010  MJL - Separate file for least square function class
// \endverbatim


#include <vector>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_bundle_adjust_lsqr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Static functions for bundle adjustment
class vpgl_bundle_adjust
{
 public:
  //: Constructor
  vpgl_bundle_adjust();
  //: Destructor
  ~vpgl_bundle_adjust();

  void set_use_m_estimator(bool use_m) { use_m_estimator_ = use_m; }
  void set_m_estimator_scale(double scale) { m_estimator_scale_ = scale; }
  void set_use_gradient(bool use_gradient) { use_gradient_ = use_gradient; }
  void set_self_calibrate(bool self_calibrate) { self_calibrate_ = self_calibrate; }
  void set_normalize_data(bool normalize) { normalize_data_ = normalize; }
  void set_verbose(bool verbose) { verbose_ = verbose; }
  void set_max_iterations(unsigned maxitr) { max_iterations_ = maxitr; }
  void set_x_tolerence(double xtol) { x_tol_ = xtol; }
  void set_g_tolerence(double gtol) { g_tol_ = gtol; }
  //: step size for finite differencing operations
  void set_epsilon(double eps) { epsilon_ = eps; }

  //: Return the ending error
  double end_error() const { return end_error_; }
  //: Return the starting error
  double start_error() const { return start_error_; }
  //: Return the number of iterations
  int num_iterations() const { return num_iterations_; }
  //: Return the weights a the end of the optimization
  const std::vector<double>& final_weights() const { return weights_; }

  //: Return the raw camera parameters
  const vnl_vector<double>& cam_params() const { return a_; }
  //: Return the raw world point parameters
  const vnl_vector<double>& point_params() const { return b_; }

  //: Approximately depth invert the scene.
  //  Apply this and re-optimize to get out of a common local minimum.
  //  Find the mean axis between cameras and points, mirror the points about
  //  a plane perpendicular to this axis, and rotate the cameras 180 degrees
  //  around this axis
  void depth_reverse(std::vector<vpgl_perspective_camera<double> >& cameras,
                     std::vector<vgl_point_3d<double> >& world_points);

  //: Bundle Adjust
  //
  // \param mask should have the same number of entries as \param cameras,
  // and each entry of \param mask should be the same size as
  // \param world_points. mask[i][j] is true if point j is visible from
  // camera i
  //
  // \param image_points and is a linear list of the 2D locations of the
  // 3D points as seen by the cameras. There is one image point for every
  // true in \param mask. The following piece of code shows the structure
  // of \param image_points
  //
  // for( int c = 0; c < num_cameras; c++ ){
  //   for( int dp = 0; dp < num_world_points; dp++ ){
  //     if( mask[c][dp] )
  //       img_points.push_back( all_image_points[c*num_world_points+dp] );
  //   }
  // }
  bool optimize(std::vector<vpgl_perspective_camera<double> >& cameras,
                std::vector<vgl_point_3d<double> >& world_points,
                const std::vector<vgl_point_2d<double> >& image_points,
                const std::vector<std::vector<bool> >& mask);

  //: Write cameras and points to a file in VRML 2.0 for debugging
  static void write_vrml(const std::string& filename,
                         const std::vector<vpgl_perspective_camera<double> >& cameras,
                         const std::vector<vgl_point_3d<double> >& world_points);

 private:
  //: normalize image points to be mean centered with scale sqrt(2)
  // \return parameters such that original point are recovered as (ns*x+nx, ns*y+ny)
  void normalize_points(std::vector<vgl_point_2d<double> >& image_points,
                        double& nx, double& ny, double& ns);

  // reflect the points about a plane
  void reflect_points(const vgl_plane_3d<double>& plane,
                      std::vector<vgl_point_3d<double> >& points);

  // rotation the cameras 180 degrees around an axis
  void rotate_cameras(const vgl_vector_3d<double>& axis,
                      std::vector<vpgl_perspective_camera<double> >& cameras);

  //: The bundle adjustment error function
  vpgl_bundle_adjust_lsqr* ba_func_;
  //: The last camera parameters
  vnl_vector<double> a_;
  //: The last point parameters
  vnl_vector<double> b_;
  //: The last global parameters
  vnl_vector<double> c_;
  //: The last estimated weights
  std::vector<double> weights_;

  bool use_m_estimator_;
  double m_estimator_scale_;
  bool use_gradient_;
  bool self_calibrate_;
  bool normalize_data_;
  bool verbose_;
  unsigned int max_iterations_;
  double x_tol_;
  double g_tol_;
  double epsilon_;

  double start_error_;
  double end_error_;
  int num_iterations_;
};


#endif // vpgl_bundle_adjust_h_
