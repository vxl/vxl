// This is brl/bseg/bvxm/algo/pro/processes/bvxm_create_scene_xml_process.cxx
#include "bvxm_create_scene_xml_process.h"
#include <iostream>
#include <string>
//:
// \file
#include <bkml/bkml_parser.h>
#include <bvxm/algo/bvxm_create_scene_xml.h>
#include <bvxm/bvxm_world_params.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_vector_3d.h>
#include <vpgl/vpgl_lvcs.h>
#include <vul/vul_file.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif



// set up input types
bool bvxm_create_scene_xml_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_create_scene_xml_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0]  = "vcl_string";      // scene xml output file
  input_types_[1]  = "vcl_string";      // scene world directory
  input_types_[2]  = "float";           // scene corner x coordinate
  input_types_[3]  = "float";           // scene corner y coordinate
  input_types_[4]  = "float";           // scene corner z coordinate
  input_types_[5]  = "unsigned";        // scene dimension in x direction
  input_types_[6]  = "unsigned";        // scene dimension in y direction
  input_types_[7]  = "unsigned";        // scene dimension in z direction
  input_types_[8]  = "float";           // scene voxel size
  input_types_[9]  = "vpgl_lvcs_sptr";  // scene lvcs
  input_types_[10]  = "vcl_string";     // scene lvcs file path
  input_types_[11] = "float";           // scene minimum occupancy probability
  input_types_[12] = "float";           // scene maximum occupancy probability
  input_types_[13] = "unsigned";        // scene maximum scale
  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

// execution
bool bvxm_create_scene_xml_process(bprb_func_process& pro)
{
  using namespace bvxm_create_scene_xml_process_globals;
  // sanity check
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong inputs!!\n";
    return false;
  }

  // get input
  unsigned in_i = 0;
  std::string  scene_xml = pro.get_input<std::string>(in_i++);
  std::string  world_dir = pro.get_input<std::string>(in_i++);
  auto       corner_x  = pro.get_input<float>(in_i++);
  auto       corner_y  = pro.get_input<float>(in_i++);
  auto       corner_z  = pro.get_input<float>(in_i++);
  auto       dim_x  = pro.get_input<unsigned>(in_i++);
  auto       dim_y  = pro.get_input<unsigned>(in_i++);
  auto       dim_z  = pro.get_input<unsigned>(in_i++);
  auto     voxel_size  = pro.get_input<float>(in_i++);
  vpgl_lvcs_sptr  lvcs_sptr = pro.get_input<vpgl_lvcs_sptr>(in_i++);
  std::string lvcs_file  = pro.get_input<std::string>(in_i++);
  auto   min_ocp_prob  = pro.get_input<float>(in_i++);
  auto   max_ocp_prob  = pro.get_input<float>(in_i++);
  auto   max_scale  = pro.get_input<unsigned>(in_i++);

  if (!lvcs_sptr) {
    std::cerr << pro.name() << ": input scene lvcs is empty!!\n";
    return false;
  }
  if (!vul_file::exists(lvcs_file)) {
    std::cerr << pro.name() << ": can not find lvcs file - " << lvcs_file << "!!\n";
    return false;
  }

  // Call wrapped function
  bvxm_create_scene_xml(scene_xml, world_dir,
                        corner_x, corner_y, corner_z,
                        dim_x, dim_y, dim_z,
                        voxel_size,
                        *lvcs_sptr, lvcs_file,
                        min_ocp_prob, max_ocp_prob, max_scale);

  return true;
}


// process to generate scenes that are arranged by a quad-tree structure to cover a large scale region
// Note that the scene may have a land mask ratio to quantify the scene urban coverage, if land masks are available.  (-1.0 is unknown)
bool bvxm_create_scene_xml_large_scale_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_create_scene_xml_large_scale_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";    // region where scenes will cover, defined in kml file
  input_types_[1] = "vcl_string";    // folder where scene xml files will reside
  input_types_[2] = "vcl_string";    // folder where scene world directories will reside
  input_types_[3] = "vcl_string";    // height map folder to retrieve elevation value
  input_types_[4] = "vcl_string";    // land mask folder
  input_types_[5] = "float";         // scene size
  input_types_[6] = "float";         // scene voxel size
  input_types_[7] = "float";         // the amount to be added on top of the terrain height (large enough to cover highest building)
  input_types_[8] = "float";         // the amount to be subtracted on bottom of the terrain height (to overcome the height map inaccuracy)
  input_types_[9] = "float";         // outline extension in meter
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "unsigned";     // number of scenes created

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvxm_create_scene_xml_large_scale_process(bprb_func_process& pro)
{
  using namespace bvxm_create_scene_xml_large_scale_process_globals;
  // sanity check
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong inputs!\n";
    return false;
  }
  // get inputs
  unsigned in_i = 0;
  std::string roi_kml     = pro.get_input<std::string>(in_i++);
  std::string scene_root  = pro.get_input<std::string>(in_i++);
  std::string world_dir   = pro.get_input<std::string>(in_i++);
  std::string dem_folder  = pro.get_input<std::string>(in_i++);
  std::string land_folder = pro.get_input<std::string>(in_i++);
  auto world_size_in = pro.get_input<float>(in_i++);
  auto voxel_size    = pro.get_input<float>(in_i++);
  auto height        = pro.get_input<float>(in_i++);
  auto height_sub    = pro.get_input<float>(in_i++);
  auto en_in         = pro.get_input<float>(in_i++);

  double extension = static_cast<double>(en_in);

  // find the bounding box from the given KML region
  vgl_polygon<double> poly = bkml_parser::parse_polygon(roi_kml);
  if (poly[0].size() == 0) {
    std::cerr << pro.name() << ": can not get region from input kml: " << roi_kml << "!\n";
    return false;
  }

  vgl_box_2d<double> bbox_rect;
  for (auto i : poly[0])
    bbox_rect.add(i);

  // Call wrapped function
  unsigned int leaves_size = bvxm_create_scene_xml_large_scale(bbox_rect,
                                                               scene_root,
                                                               world_dir,
                                                               dem_folder,
                                                               world_size_in,
                                                               voxel_size,
                                                               height,
                                                               height_sub,
                                                               extension,
                                                               land_folder);

  // generate output
  pro.set_output_val<unsigned>(0, leaves_size);
  return true;
}
