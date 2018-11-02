#ifndef icam_cylinder_map_h_
#define icam_cylinder_map_h_
//:
// \file
// \brief A mapping from images to a cylindrical surface
// \author J.L. Mundy
// \date Oct 26, 2013
//
// \verbatim
//  Modifications
//   None
// \endverbatim
//
// Requires a set of images and cameras. The portion of images with camera rays
// most normal to the cylinder are inserted in the map. The cylinder is
// parameterized by aziumuth (theta) and z. The z range is specified as:
//         -lower_height_ <=  z  <= upper_height_
// The map is composed of  nz_ x n_theta_ discrete locations.
// In the current design the cylinder origin is variable
// but the cylinder axis is always the z axis in the world reference frame.
// For complete generality there should be a rotation transform associated
// with the cylinder, but not implemented.
//
#include <iostream>
#include <vector>
#include <vil/vil_image_view.h>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_ray_3d.h>
#include <vpgl/vpgl_camera.h>
class icam_cylinder_map : public vbl_ref_count
{
 public:
  //: Constructors
  icam_cylinder_map(): n_theta_(0), nz_(0),origin_(vgl_point_3d<double>()),
                       radius_(1.0), upper_height_(1.0), lower_height_(0.0){}
  icam_cylinder_map(unsigned n_theta, unsigned nz,
                    vgl_point_3d<double> const& pt, double radius,
                    double h_upper, double h_lower = 0.0):
  n_theta_(n_theta), nz_(nz), origin_(pt), radius_(radius),
    upper_height_(h_upper),lower_height_(h_lower){}
  //: Destructor
  ~icam_cylinder_map() override= default;

  //: images must have RGB byte pixel type
  void set_data(std::vector<vil_image_view<vxl_byte> > const& images,
                std::vector<vpgl_camera_double_sptr > const& cams){
    images_ = images; cams_ = cams;}
    bool closest_camera(vgl_ray_3d<double> const& cyl_ray,
                          vgl_point_3d<double> const& p,
                          unsigned& cam_index,
                          double& u, double& v) const;

  bool map_cylinder();
  vil_image_view<vxl_byte> cyl_map() const{
    return cyl_map_;}
  //assumes a color byte image
  bool render_map(vil_image_view<vxl_byte>const& backgnd,
                  vpgl_camera_double_sptr const& cam,
                  double theta, vil_image_view<vxl_byte>& img,
                  double scale = 1.0,
                  float back_r = 0.0f, float back_g = 0.0f, float back_b = 255.0f);
 protected:
  // number of samples in the map
  unsigned n_theta_;
  unsigned nz_;
  // cylinder geometry
  vgl_point_3d<double> origin_;
  double radius_;
  double upper_height_;
  double lower_height_;
  vil_image_view<vxl_byte> cyl_map_;
  // input data (RGB byte images)
  std::vector<vil_image_view<vxl_byte> > images_;
  // currently supports only projective or perspective camera type
  std::vector<vpgl_camera_double_sptr > cams_;
};



#endif
