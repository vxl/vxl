// This is core/vpgl/algo/vpgl_rational_adjust_onept.h
#ifndef vpgl_rational_adjust_onept_h_
#define vpgl_rational_adjust_onept_h_
//:
// \file
// \brief Adjust image offsets to register a set of rational cameras
// \author J. L. Mundy
// \date July 29, 2007
//
// \verbatim
//   Modifications
//    Yi Dong  Jun-2015  added new function to optimize the 3-d point intersection search by a user-defined initial
//    guess
// \endverbatim

#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <vnl/vnl_least_squares_function.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

//:
// The image offsets of rational cameras typically must be adjusted to
// compensate for errors in geographic alignment. This algorithm finds
// a set of minimum translations that registers the input set of images.
// After registration, the images have geographically corresponding rational
// cameras. That is, a visible 3-d point will project into its corresponding
// image location in all the images.

class vpgl_z_search_lsqr : public vnl_least_squares_function
{
public:
  //: Constructor
  vpgl_z_search_lsqr(std::vector<vpgl_rational_camera<double>> cams,
                     std::vector<float> cam_weights,
                     std::vector<vgl_point_2d<double>> image_pts,
                     const vgl_point_3d<double> & initial_pt,
                     const double & relative_diameter = 1.0);
  //: Destructor
  ~vpgl_z_search_lsqr() override = default;

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  fx has been sized appropriately before the call.
  void
  f(const vnl_vector<double> & elevation, vnl_vector<double> & projection_error) override;
  double
  xm() const
  {
    return xm_;
  }
  double
  ym() const
  {
    return ym_;
  }

protected:
  vpgl_z_search_lsqr(); // not valid
  vgl_point_3d<double> initial_pt_;
  std::vector<vpgl_rational_camera<double>> cameras_; // cameras
  std::vector<float> cam_weights_;
  std::vector<vgl_point_2d<double>> image_pts_; // image points
  double xm_, ym_;
  double relative_diameter_;
};


class vpgl_rational_adjust_onept
{
public:
  ~vpgl_rational_adjust_onept() = default;

  static bool
  find_intersection_point(const std::vector<vpgl_rational_camera<double>> & cams,
                          const std::vector<float> & cam_weights,
                          const std::vector<vgl_point_2d<double>> & corrs,
                          vgl_point_3d<double> & p_3d);

  static bool
  find_intersection_point(const std::vector<vpgl_rational_camera<double>> & cams,
                          const std::vector<float> & cam_weights,
                          const std::vector<vgl_point_2d<double>> & corrs,
                          const vgl_point_3d<double> & initial_pt,
                          const double & zmin,
                          const double & zmax,
                          vgl_point_3d<double> & p_3d,
                          const double & relative_diameter = 1.0);

  static bool
  refine_intersection_pt(const std::vector<vpgl_rational_camera<double>> & cams,
                         const std::vector<float> & cam_weights,
                         const std::vector<vgl_point_2d<double>> & image_pts,
                         const vgl_point_3d<double> & initial_pt,
                         vgl_point_3d<double> & final_pt,
                         const double & relative_diameter = 1.0);

  static bool
  adjust(const std::vector<vpgl_rational_camera<double>> & cams,
         const std::vector<vgl_point_2d<double>> & corrs,
         std::vector<vgl_vector_2d<double>> & cam_translations,
         vgl_point_3d<double> & intersection);

  static bool
  adjust(const std::vector<vpgl_rational_camera<double>> & cams,
         const std::vector<vgl_point_2d<double>> & corrs,
         const vgl_point_3d<double> & initial_pt,
         const double & zmin,
         const double & zmax,
         std::vector<vgl_vector_2d<double>> & cam_translations,
         vgl_point_3d<double> & intersection,
         const double & relative_diameter = 1.0);

  // pass a weight for each camera, the weights should add up to 1.0
  static bool
  adjust_with_weights(const std::vector<vpgl_rational_camera<double>> & cams,
                      std::vector<float> weights,
                      const std::vector<vgl_point_2d<double>> & corrs,
                      std::vector<vgl_vector_2d<double>> & cam_translations,
                      vgl_point_3d<double> & intersection);

  static bool
  adjust_with_weights(const std::vector<vpgl_rational_camera<double>> & cams,
                      const std::vector<float> & weights,
                      const std::vector<vgl_point_2d<double>> & corrs,
                      const vgl_point_3d<double> & initial_pt,
                      const double & zmin,
                      const double & zmax,
                      std::vector<vgl_vector_2d<double>> & cam_translations,
                      vgl_point_3d<double> & intersection,
                      const double & relative_diameter = 1.0);

protected:
  vpgl_rational_adjust_onept();
};


#endif // vpgl_rational_adjust_onept_h_
