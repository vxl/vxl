//This is brl/bbas/volm/volm_spherical_region_index.h
#ifndef volm_spherical_region_index_h_
#define volm_spherical_region_index_h_
//:
// \file
// \brief A class to construct an segment an index based on axis-aligned boxes on the sphere
// Units are in meters
//
// \author Vishal Jain
// \date feb 22, 2013
// \verbatim
//  Modifications
// None
// \endverbatim

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_map.h>
#include <vcl_string.h>
#include <volm/volm_camera_space.h>
#include <volm/volm_camera_space_sptr.h>
#include <depth_map/depth_map_scene_sptr.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_spherical_region.h>

#include <vsph/vsph_unit_sphere_sptr.h>
#include <vsph/vsph_segment_sphere.h>
class volm_spherical_region_index
{
 public:
  volm_spherical_region_index(vcl_string index_file_path,
                              vcl_string usph_file_path);
  volm_spherical_regions_layer index_regions();
  void print(vcl_ostream& os) ;

 private:
  void construct_spherical_regions();
  void load_unitsphere(vcl_string usph_file_path);
  volm_spherical_regions_layer  sph_regions_;

  vcl_vector<double> data_;
  vsph_unit_sphere_sptr usph_;
  vsph_segment_sphere * seg;
};

#endif // volm_spherical_region_index_h_
