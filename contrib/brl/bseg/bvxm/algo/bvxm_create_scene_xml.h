// This is brl/bseg/bvxm/algo/bvxm_create_scene_xml.h
#ifndef bvxm_create_scene_xml_h_
#define bvxm_create_scene_xml_h_

#include <string>

#include <vgl/vgl_box_2d.h>
#include <vpgl/vpgl_lvcs.h>

void bvxm_create_scene_xml(std::string const& scene_xml, std::string const& world_dir,
                           float corner_x, float corner_y, float corner_z,
                           unsigned int dim_x, unsigned int dim_y, unsigned int dim_z,
                           float voxel_size,
                           vpgl_lvcs const& lvcs, std::string const& lvcs_file,
                           float min_ocp_prob, float max_ocp_prob, unsigned int max_scale);

unsigned int bvxm_create_scene_xml_large_scale(vgl_box_2d<double> const& bbox_rect_input,
                                               std::string const& scene_root,
                                               std::string const& world_dir,
                                               std::string const& dem_folder,
                                               float world_size_in=500.0,
                                               float voxel_size=1.0,
                                               float height=120.0,
                                               float height_sub=25.0,
                                               double extension=500.0,
                                               std::string const& land_folder="");

#endif  // bvxm_create_scene_xml_h_
