#include <iostream>
#include <vector>
#include "volm_spherical_query_region.h"
#include <vgl/vgl_polygon.h>
#include <bsol/bsol_algs.h>
#include <bpgl/bpgl_camera_utils.h>
#include <vsph/vsph_utils.h>
#include "volm_spherical_container.h"
#include "volm_char_codes.h"
#include <bpgl/depth_map/depth_map_region.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void volm_spherical_query_region::
set_from_depth_map_region(vpgl_perspective_camera<double> const& cam,
                          depth_map_region_sptr const& dm_region,
                          volm_spherical_container_sptr const& sph_vol)
{
  //extract object attributes
  depth_map_region::orientation otype = dm_region->orient_type();
  double min_depth = dm_region->min_depth();
  double max_depth = dm_region->max_depth();
  order_ =  static_cast<unsigned char>(dm_region->order());
  nlcd_id_ = static_cast<unsigned char>(dm_region->land_id());
  //nlcd_id_ = static_cast<unsigned char>(dm_region->nlcd_id());
  std::string nam = dm_region->name();
  if (otype == depth_map_region::GROUND_PLANE) {
    orientation_ = static_cast<unsigned char>(depth_map_region::HORIZONTAL);
    min_depth_ = static_cast<unsigned char>(sph_vol->min_voxel_res());
    max_depth_ =
      static_cast<unsigned char>(sph_vol->get_depth_interval(max_depth));
  }
  if (otype == depth_map_region::INFINT||nam=="sky") {
    min_depth_ = sky_depth;
    max_depth_ = sky_depth;
    order_ = sky_order;
    orientation_ = static_cast<unsigned char>(depth_map_region::INFINT);
  }
  else {
    orientation_ = static_cast<unsigned char>(otype);
    min_depth_ = sph_vol->get_depth_interval(min_depth);
    max_depth_ = sph_vol->get_depth_interval(max_depth);
  }
  // form the bounding box
  vgl_polygon<double> poly = bsol_algs::vgl_from_poly(dm_region->region_2d());
  vgl_polygon<double> sph_poly =
    vsph_utils::project_poly_onto_unit_sphere(cam, poly);
  unsigned n_sheets = sph_poly.num_sheets();
  for (unsigned sh_idx = 0; sh_idx<n_sheets; ++sh_idx) {
    unsigned n_verts = sph_poly[sh_idx].size();
    for (unsigned v_idx = 0; v_idx < n_verts; ++v_idx) {
      vgl_point_2d<double> pt = sph_poly[sh_idx][v_idx];
      vsph_sph_point_2d sph_pt(pt.x(), pt.y());
      box_.add(sph_pt);
    }
  }
}

void volm_spherical_query_region::print(std::ostream& os) const
{
  std::cout << depth_map_region::orient_string(orientation_) << "::";
  box_.print(os, false);
}
