// This is core/vpgl/algo/vpgl_rational_adjust_multipt.h
#ifndef vpgl_rational_adjust_multipt_h_
#define vpgl_rational_adjust_multipt_h_
//:
// \file
// \brief Adjust image offsets to register a set of rational cameras using multiple correspondence points
// \author Ozge C. Ozcanli
// \date Nov 17, 2011
//
// \verbatim
//   Modifications
//    Yi Dong  Jun-2015  added new function to optimize the Lev-Marq refinement with given initial 3-d point, height range and search diameter
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
// This algorithm finds a set of minimum translations that registers the input set of images using multiple correspondences
// e.g. multiple 3D points projecting to a set of 2D correspondences in each image
// After registration, the images have geographically corresponding rational
// cameras. That is, a visible 3-d point will project into its corresponding
// image location in all the images.

class vpgl_cam_trans_search_lsqr : public vnl_least_squares_function
{
 public:
  //: Constructor
  vpgl_cam_trans_search_lsqr(std::vector<vpgl_rational_camera<double> > const& cams,
                             std::vector<float>  cam_weights,
                             std::vector< std::vector<vgl_point_2d<double> > > const& image_pts,  // for each 3D corr, an array of 2D corrs for each camera
                             std::vector< vgl_point_3d<double> >  initial_pts);
  //: Destructor
  ~vpgl_cam_trans_search_lsqr() override = default;

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  fx has been sized appropriately before the call.
  void f(vnl_vector<double> const& translation,   // size is 2*cams.size()
                 vnl_vector<double>& projection_errors) override;  // size is cams.size()*image_pts.size() --> compute a residual for each 3D corr point in each image

  void get_finals(std::vector<vgl_point_3d<double> >& finals);

 protected:
  vpgl_cam_trans_search_lsqr();//not valid
  std::vector<vgl_point_3d<double> > initial_pts_;
  std::vector<vpgl_rational_camera<double> > cameras_; //cameras
  std::vector<float> cam_weights_; // should sum up to 1
  std::vector<std::vector<vgl_point_2d<double> > > corrs_;
  std::vector<vgl_point_3d<double> > finals_;
};

class vpgl_rational_adjust_multiple_pts
{
 public:
  ~vpgl_rational_adjust_multiple_pts() = default;

  //: exhaustively searches the parameter space to find the best parameter setting
  static bool adjust(std::vector<vpgl_rational_camera<double> > const& cams,
                     std::vector<float> const& cam_weights,
                     std::vector<std::vector< vgl_point_2d<double> > > const& corrs,  // a vector of correspondences for each cam
                     double radius, int n,       // divide radius into n intervals to generate camera translation space
                     std::vector<vgl_vector_2d<double> >& cam_translations,          // output translations for each cam
                     std::vector<vgl_point_3d<double> >& intersections);             // output 3d locations for each correspondence

  //: run Lev-Marq optimization to search the param space to find the best parameter setting
  static bool adjust_lev_marq(std::vector<vpgl_rational_camera<double> > const& cams,
                              std::vector<float> const& cam_weights,
                              std::vector<std::vector< vgl_point_2d<double> > > const& corrs, // a vector of correspondences for each cam
                              std::vector<vgl_vector_2d<double> >& cam_translations, // output translations for each cam
                              std::vector<vgl_point_3d<double> >& intersections);    // output 3d locations for each correspondence

  //: run Lev-Marq optimization to search the param space to find the best parameter setting, with a initial guess and relative diameter given for back-projection
  static bool adjust_lev_marq(std::vector<vpgl_rational_camera<double> > const& cams,          // cameras that will be corrected
                              std::vector<float> const& cam_weights,                           // camera weight parameters
                              std::vector<std::vector<vgl_point_2d<double> > > const& corrs,    // a vector of correspondences for each cam
                              vgl_point_3d<double> const& initial_pt,                         // initial 3-d point for back-projection
                              double const& zmin,                                             // minimum allowed height of the 3-d intersection point
                              double const& zmax,                                             // maximum allowed height of the 3-d intersection point
                              std::vector<vgl_vector_2d<double> >& cam_translations,           // output translations for each camera
                              std::vector<vgl_point_3d<double> >& intersections,               // output 3-d locations for each correspondence
                              double const relative_diameter = 1.0);                          // relative diameter used in back-projection

 protected:
  vpgl_rational_adjust_multiple_pts();
};


#endif // vpgl_rational_adjust_multipt_h_
