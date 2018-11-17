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
// February 2, 2013
// Replaced the internals of this class by the more basic vsph_unit_sphere
// Older constructors are maintained for compatibility. For example,
// radius_ is no longer needed since its value is always one. Note also that
// cap_angle is no longer used since min,max theta accomplish the same
// purpose.
// \endverbatim
//

#include <vector>
#include <iostream>
#include <cstddef>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <vsph/vsph_spherical_coord_sptr.h>
#include <vsph/vsph_spherical_coord.h>
#include <vsph/vsph_sph_point_3d.h>
#include <vsph/vsph_unit_sphere.h>
#include <vil/vil_image_view.h>

class volm_spherical_shell_container : public vbl_ref_count
{
 public:
  //: Default constructor
  volm_spherical_shell_container() = default;
  //: Legacy Constructor
  volm_spherical_shell_container(double radius, float cap_angle, float point_angle, float top_angle, float bottom_angle);
  //: Minimal constructor (to internally construct vsph_unit_sphere)
  volm_spherical_shell_container(double point_angle, double min_theta,
                                 double max_theta);
  //: Construct using an existing unit sphere smart ptr
 volm_spherical_shell_container(vsph_unit_sphere_sptr usph_ptr) :
  usph_(usph_ptr){}

  // === accessors ===

  double cap_angle() const { return 180.0; }
  double radius() const { return 1.0; }
  double point_angle() const { return usph_->point_angle(); }
  double top_angle() const { return usph_->min_theta(); }
  double bottom_angle() const {return 180.0 - usph_->max_theta(); }
  vgl_point_3d<double> cent() const { return {0.0, 0.0, 0.0}; }
  std::vector<vgl_point_3d<double> > cart_points() const;

  std::vector<vsph_sph_point_3d> sph_points() const;

  vsph_unit_sphere_sptr unit_sphere() const {return usph_;}

  std::size_t get_container_size() const { return usph_->size(); }

  void draw_template(const std::string& vrml_file_name);
  //: draw each disk with a color with respect to the values, the size and order of the values should be the size and order of the cart_points
  void draw_template(const std::string& vrml_file_name, std::vector<unsigned char>& values, unsigned char special);

  //: generate panaroma image
  void panaroma_img(vil_image_view<vil_rgb<vxl_byte> >& img, std::vector<unsigned char>& values);
  void panaroma_img_class_labels(vil_image_view<vil_rgb<vxl_byte> >& img, std::vector<unsigned char>& values);
  void panaroma_img_orientations(vil_image_view<vil_rgb<vxl_byte> >& img, std::vector<unsigned char>& values);
  void panaroma_images_from_combined(vil_image_view<vil_rgb<vxl_byte> >& img_orientation, vil_image_view<vil_rgb<vxl_byte> >& img, std::vector<unsigned char>& values);

  // ===========  binary I/O ================

  //: version
  unsigned version() const {return 1;}

  //: binary IO write
  void b_write(vsl_b_ostream& os);

  //: binary IO read
  void b_read(vsl_b_istream& is);

  bool operator== (const volm_spherical_shell_container &other) const;

 protected:
  vsph_unit_sphere_sptr usph_;
};

#endif  // volm_spherical_shell_container_h_
