//This is brl/bbas/volm/volm_spherical_shell_container.h
#ifndef volm_spherical_shell_container_h_
#define volm_spherical_shell_container_h_
//:
// \file
// \brief  A class to represent a container of points uniformly distributed on a spherical surface with pre-defined radius, each of which represents 
//         on the ray from sphere center (0,0,0) to the surface point.  
//         The uniform distribution is accomplished by triangle division on octahedron to certain density
//         The points are stores both as cartesian coordinates and spherical coordinates
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
#include <vnl/vnl_math.h>
#include <vsph/vsph_spherical_coord_sptr.h>
#include <vsph/vsph_spherical_coord.h>
#include <vsph/vsph_sph_point_3d.h>

class volm_spherical_shell_container : public vbl_ref_count
{
 public:
  //: Constructor 
  volm_spherical_shell_container() {}
  volm_spherical_shell_container(double radius, double cap_angle, double point_angle);

  //: accessor
  double cap_angle() { return cap_angle_; }
  double radius() { return radius_; }
  double point_angle() { return point_angle_; }
  vgl_point_3d<double> cent() { return coord_sys_->origin(); }
  vcl_vector<vgl_point_3d<double> >& cart_points() { return cart_points_; }
  vcl_vector<vsph_sph_point_3d>& sph_points() { return sph_points_;}
  
  //: Methods
  vcl_size_t get_container_size() { return cart_points_.size(); }

protected:
  double radius_;
  double point_angle_;
  double cap_angle_;
  vsph_spherical_coord_sptr coord_sys_;
  vcl_vector<vgl_point_3d<double> > cart_points_;
  vcl_vector<vsph_sph_point_3d> sph_points_;
  void add_uniform_views();
  bool min_angle(vcl_vector<vgl_point_3d<double> > list, double point_angle);
  bool find_closest(vgl_point_3d<double> p, double& dist);
};

#endif  // volm_spherical_shell_container_h_