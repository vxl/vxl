//This is brl/bbas/volm/volm_spherical_region_query.h
#ifndef volm_spherical_region_query_h_
#define volm_spherical_region_query_h_
//:
// \file
// \brief A class to construct a query based on axis-aligned boxes on the sphere
// Units are in meters
//
// \author J.L. Mundy
// \date January 20, 2012
// \verbatim
//  Modifications
// None
// \endverbatim

#include <vector>
#include <iostream>
#include <map>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <volm/volm_camera_space.h>
#include <volm/volm_camera_space_sptr.h>
#include <bpgl/depth_map/depth_map_scene_sptr.h>
#include <bpgl/depth_map/depth_map_region_sptr.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_spherical_region.h>
#include <vsph/vsph_unit_sphere_sptr.h>
class volm_spherical_region_query
{
 public:
  volm_spherical_region_query(depth_map_scene_sptr const& dm_scene,
                              volm_camera_space_sptr const& cam_space,
                              volm_spherical_container_sptr const& sph_vol);

  volm_spherical_regions_layer query_regions(unsigned roll_indx);

  void print(std::ostream& os) const;

 private:
  void construct_spherical_regions();
  volm_spherical_region set_from_depth_map_region(vpgl_perspective_camera<double> const& cam,
                                                  depth_map_region_sptr const& dm_region);
  volm_spherical_region set_ground_from_depth_map_region(vpgl_perspective_camera<double> const& cam,
                                                       depth_map_region_sptr const& dm_region);
  volm_spherical_region set_sky_from_depth_map_region(vpgl_perspective_camera<double> const& cam,
                                                       depth_map_region_sptr const& dm_region);
  depth_map_scene_sptr dm_scene_;
  volm_camera_space_sptr cam_space_;
  volm_spherical_container_sptr sph_vol_;

  //     roll index  regions on sphere
  std::map< unsigned, volm_spherical_regions_layer > sph_regions_;

  double canonical_top_fov_ ;
  double canonical_head_    ;
  double canonical_tilt_    ;
};

#endif // volm_spherical_region_query_h_
