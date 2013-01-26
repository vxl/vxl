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

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_string.h>
#include <volm/volm_camera_space_sptr.h>
#include <depth_map/depth_map_scene_sptr.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_spherical_query_region.h>
#include <volm/volm_spherical_layers.h>
class volm_spherical_region_query
{
 public:
  volm_spherical_region_query(depth_map_scene_sptr const& dm_scene,
			      volm_camera_space_sptr const& cam_space,
			      volm_spherical_container_sptr const& sph_vol);
  
  void construct_spherical_regions();

 private:
  depth_map_scene_sptr dm_scene_;
  volm_camera_space_sptr cam_space_;
  volm_spherical_container_sptr sph_vol_;
  //     roll index                 regions on sphere
  vcl_map< unsigned, vcl_vector<volm_spherical_query_region> > sph_regions_;
};

#endif // volm_spherical_region_query_h_
