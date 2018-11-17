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
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_spherical_region.h>

#include <vsph/vsph_unit_sphere_sptr.h>
#include <vsph/vsph_segment_sphere.h>

class volm_spherical_region_index
{
 public:
  volm_spherical_region_index(std::map<std::string,std::string> & index_file_paths,
                              const std::string& usph_file_path);
  volm_spherical_region_index(std::map<std::string,std::vector<unsigned char> > & index_file_paths,
                              vsph_unit_sphere_sptr & usph);
  volm_spherical_region_index(float * boxes,int num_depth_regions, int num_orientation_regions, int num_nlcd_regions, int sky_regions);
  volm_spherical_regions_layer index_regions();
  void print(std::ostream& os) ;

  void write_binary(std::ofstream & oconfig,std::ofstream & odata);
  ~volm_spherical_region_index();
  vsph_segment_sphere * seg() { return seg_; }
 private:
  void construct_spherical_regions();
  void load_unitsphere(const std::string& usph_file_path);
  volm_spherical_regions_layer  sph_regions_;
  float check_phi_bounds(float  phi);
  std::vector<double> data_;
  vsph_unit_sphere_sptr usph_;
  vsph_segment_sphere * seg_;
};

#endif // volm_spherical_region_index_h_
