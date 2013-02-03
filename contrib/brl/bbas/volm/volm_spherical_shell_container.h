//This is brl/bbas/volm/volm_spherical_shell_container.h
#ifndef volm_spherical_shell_container_h_
#define volm_spherical_shell_container_h_
//:
// \file
// \brief  A class to represent a container of points uniformly distributed on a spherical surface.
// The radius of that sphere is pre-defined. Each point represents the ray from sphere center (0,0,0) to the surface point.
// The uniform distribution is accomplished by triangle division on octahedron to certain density.
// The points are stored both as cartesian coordinates and spherical coordinates.
// The spherical coordinate system is based on bbas/vsph:
//  elevation is zero at the North pole and 180 at the South pole
//  azimuth is zero pointing along X and positive rotating towards Y
//  azimuth is +- 180 pointing along -X
//
// \author Yi Dong
// \date October 24, 2012
// \verbatim
//  Modifications
//
// \endverbatim
//

#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vcl_cstddef.h> // for std::size_t
#include <vgl/vgl_point_3d.h>
#include <vsph/vsph_spherical_coord_sptr.h>
#include <vsph/vsph_spherical_coord.h>
#include <vsph/vsph_sph_point_3d.h>
#include <vil/vil_image_view.h>

class volm_spherical_shell_container : public vbl_ref_count
{
 public:
  //: Default constructor
  volm_spherical_shell_container() {}
  //: Constructor
  volm_spherical_shell_container(double radius, float cap_angle, float point_angle, float top_angle, float bottom_angle);

  // === accessors ===

  double cap_angle() const { return cap_angle_; }
  double radius() const { return radius_; }
  double point_angle() const { return point_angle_; }
  double top_angle() const { return top_angle_; }
  double bottom_angle() const { return bottom_angle_; }
  vgl_point_3d<double> cent() const { return coord_sys_->origin(); }
  vcl_vector<vgl_point_3d<double> >& cart_points() { return cart_points_; }
  vcl_vector<vsph_sph_point_3d>& sph_points() { return sph_points_;}

  vcl_size_t get_container_size() const { return cart_points_.size(); }

  void draw_template(vcl_string vrml_file_name);
  //: draw each disk with a color with respect to the values, the size and order of the values should be the size and order of the cart_points
  void draw_template(vcl_string vrml_file_name, vcl_vector<unsigned char>& values, unsigned char special);

  //: generate panaroma image
  void panaroma_img(vil_image_view<vil_rgb<vxl_byte> >& img, vcl_vector<unsigned char>& values);
  void panaroma_img_class_labels(vil_image_view<vil_rgb<vxl_byte> >& img, vcl_vector<unsigned char>& values);
  void panaroma_img_orientations(vil_image_view<vil_rgb<vxl_byte> >& img, vcl_vector<unsigned char>& values);

  // ===========  binary I/O ================

  //: version
  unsigned version() const {return 1;}

  //: binary IO write
  void b_write(vsl_b_ostream& os);

  //: binary IO read
  void b_read(vsl_b_istream& is);

  bool operator== (const volm_spherical_shell_container &other) const;

 protected:
  void add_uniform_views();
  bool min_angle(vcl_vector<vgl_point_3d<double> > list, double point_angle);
  bool find_closest(vgl_point_3d<double> p, double& dist);
  void remove_top_and_bottom();

  double radius_;
  double point_angle_;
  double cap_angle_;
  double top_angle_;
  double bottom_angle_;
  vsph_spherical_coord_sptr coord_sys_;
  vcl_vector<vgl_point_3d<double> > cart_points_;
  vcl_vector<vsph_sph_point_3d> sph_points_;
};

#endif  // volm_spherical_shell_container_h_
