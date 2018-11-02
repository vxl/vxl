#ifndef icam_spherical_map_h_
#define icam_spherical_map_h_
//:
// \file
// \brief A mapping from images to a spherical surface
// \author J.L. Mundy
// \date Dec 6, 2013
//
// \verbatim
//  Modifications
//   None
// \endverbatim
//
// maps a set of images onto the surface of a sphere
// the elevation (theta) range is defined on the interval:
//   theta_cap_  <= theta <=  (pi-theta_cap_)
// the aziumuth range is:
//  0 <= phi <= two_pi
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
class icam_spherical_map : public vbl_ref_count
{
 public:
  //: Constructors
  icam_spherical_map(): n_theta_(0), n_phi_(0), t_theta_(0.0), t_phi_(0.0),
    origin_(vgl_point_3d<double>()), radius_(1.0), theta_cap_(0){}
  icam_spherical_map(unsigned n_theta, unsigned nphi,
                     vgl_point_3d<double> const& origin, double radius,
                     double theta_cap):
  n_theta_(n_theta), n_phi_(nphi), t_theta_(0.0), t_phi_(0.0),
    origin_(origin), radius_(radius), theta_cap_(theta_cap){}

  //: Destructor
  ~icam_spherical_map() override= default;
  //: images must be RGB byte. Cameras projective or perspective

  void set_data(std::vector<vil_image_view<vxl_byte> > const& images,
                std::vector<vpgl_camera_double_sptr > const& cams){
    images_ = images; cams_ = cams;}

  //: find the camera with ray most aligned with the sphere normal at p
    bool closest_camera(vgl_ray_3d<double> const& sph_ray,
                          vgl_point_3d<double> const& p,
                        unsigned& cam_index, double& dot_prod,
                        double& u, double& v) const;
    //: translate the map in elevation(theta) and azimuth (phi)
    void set_trans(double t_theta, double t_phi)
    {t_theta_ = t_theta; t_phi_=t_phi;}
    //: create a spherical map
    // If use_image is false then the map shows the degree of normality
    // for the available cameras. Bright foreground means that there is
    // a camera pixel with a ray aligned with the sphere normal at a
    // given point. Otherwise the intensity of the image is modulated
    // according to alignment with the sphere normal
    bool map_sphere(double angle_exponent = 0.0, bool use_image = true,
                    vxl_byte fg_r = 0, vxl_byte fg_g = 255, vxl_byte fg_b = 0,
                    vxl_byte bk_r = 20, vxl_byte bk_g = 20, vxl_byte bk_b = 20);

    vil_image_view<vxl_byte> sphere_map() const{
      return sph_map_;}
    //: render the sphere from a given theta and phi offset
    // the rendered sphere is inserted in the background image
    bool render_map(vil_image_view<vxl_byte>const& backgnd,
                    vpgl_camera_double_sptr const& cam,
                    double theta_off, double phi_off,
                    vil_image_view<vxl_byte>& img,
                    double scale = 1.0, float back_r = 0.0f,
                    float back_g = 0.0f, float back_b = 255.0f);
 protected:
  // number of samples in the map
  unsigned n_theta_;
  unsigned n_phi_;
  // transform the sphere origins
  double t_theta_;
  double t_phi_;
  // sphere geometry
  vgl_point_3d<double> origin_;
  double radius_;
  //theta angle from z axis, where sphere starts, e.g. pi/18
  double theta_cap_;
  vil_image_view<vxl_byte> sph_map_;
  // input data
  // must be RGB byte images
  std::vector<vil_image_view<vxl_byte> > images_;
  // currently projective/perspective cameras are supported
 std::vector<vpgl_camera_double_sptr > cams_;
};



#endif
