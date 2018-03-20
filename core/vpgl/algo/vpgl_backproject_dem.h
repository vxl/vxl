// This is core/vpgl/algo/vpgl_backproject_dem.h
#ifndef vpgl_backproject_dem_h_
#define vpgl_backproject_dem_h_
//:
// \file
// \brief Camera backproject functions involving geotiff DEMs
// \author J. L. Mundy
// \date Oct 22, 2016
//
// \verbatim
//   Modifications  NONE
// \endverbatim
// The camera is assumed to project lon, lat and elevation to u and v
// The units are degrees and meters with WGS84 datum
//
#include <vector>
#include <vgl/vgl_fwd.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vgl_point_3d.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
class vpgl_geo_camera; //forward declare for ptr
class vpgl_backproject_dem
{
 public:
  //: default constructor not allowed
  vpgl_backproject_dem(vil_image_resource_sptr const& dem, double zmin=0.0, double zmax=-1.0);
  ~vpgl_backproject_dem();

  //: turn on/off messages (default not verbose)
  void set_verbose(bool verbose){verbose_ = verbose;}

  //center of the DEM - useful for initializing non-linear search
  vgl_point_3d<double> geo_center() const {return geo_center_;}

  //: elevation bounds of the DEM (with tail removal)
  double zmax() const {return z_max_;}
  double zmin() const {return z_min_;}

  //: geographic corners of the DEM - useful for bounding valid image regions
  std::vector<vgl_point_3d<double> > dem_corners() const {return dem_corners_;}

  /// General camera - typically a RPC camera
  //:Backproject an image point onto a geotiff dem, start with initial_guess
  // use local ray with origin and dir from backprojection onto max_z and min_z planes
  bool bproj_dem(const vpgl_camera<double>* cam,
                 vnl_double_2 const& image_point,
                 double max_z, double min_z,
                 vnl_double_3 const& initial_guess,
                 vnl_double_3& world_point,
                 double error_tol = 1.0);

       // === vgl interface ===

  //:Backproject an image point onto a geotiff dem, start with initial_guess
  // use local ray with origin and dir from backprojection onto max_z and min_z planes
  bool bproj_dem(const vpgl_camera<double>* cam,
                 vgl_point_2d<double> const& image_point,
                 double max_z, double min_z,
                 vgl_point_3d<double> const& initial_guess,
                 vgl_point_3d<double>& world_point,
                 double error_tol = 1.0);


// +++ concrete rational camera interfaces +++

// === vnl interface ===

  //:Backproject an image point onto a plane, start with initial_guess
  // use local ray with origin and dir from backprojection onto rcam's z bounds
  bool bproj_dem(vpgl_rational_camera<double> const& rcam,
                 vnl_double_2 const& image_point,
                 double max_z, double min_z,
                 vnl_double_3 const& initial_guess,
                 vnl_double_3& world_point,
                 double error_tol = 0.05);

       // ==== vgl interface ===

  //:Backproject an image point onto a plane, start with initial_guess
  // use local ray with origin and dir from backprojection onto rcam's z bounds
  bool bproj_dem(vpgl_rational_camera<double> const& rcam,
                 vgl_point_2d<double> const& image_point,
                 double max_z, double min_z,
                 vgl_point_3d<double> const& initial_guess,
                 vgl_point_3d<double>& world_point,
                 double error_tol = 0.05);
 private:
  bool verbose_;
  double min_samples_;
  double tail_fract_;
  vil_image_resource_sptr dem_;
  vil_image_view<float> dem_view_;
  vpgl_geo_camera* geo_cam_;
  vgl_point_3d<double> geo_center_;
  std::vector<vgl_point_3d<double> > dem_corners_;
  //: bounds of the DEM
  double z_min_;
  double z_max_;
};

#endif // vpgl_backproject_dem_h_
