// This is brl/bseg/bvxm/pro/processes/bvxm_create_scene_xml_process.cxx
#include <iostream>
#include <string>
#include "bvxm_create_scene_xml_process.h"
//:
// \file
#include <bvxm/bvxm_world_params.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>

#include <bvgl/algo/bvgl_2d_geo_index.h>
#include <bvgl/algo/bvgl_2d_geo_index_sptr.h>
#include <bkml/bkml_parser.h>
#include <bkml/bkml_write.h>
#include <vgl/vgl_intersection.h>
#include <vil/vil_image_view.h>
#include <volm/volm_io_tools.h>

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
  vpgl_lvcs_sptr  lvcs  = pro.get_input<vpgl_lvcs_sptr>(in_i++);
  std::string lvcs_file  = pro.get_input<std::string>(in_i++);
  auto   min_ocp_prob  = pro.get_input<float>(in_i++);
  auto   max_ocp_prob  = pro.get_input<float>(in_i++);
  auto   max_scale  = pro.get_input<unsigned>(in_i++);

  if (!lvcs) {
    std::cerr << pro.name() << ": input scene lvcs is empty!!\n";
    return false;
  }
  if (!vul_file::exists(lvcs_file)) {
    std::cerr << pro.name() << ": can not find lvcs file - " << lvcs_file << "!!\n";
    return false;
  }

  // create scene parameter
  vgl_point_3d<float> corner(corner_x, corner_y, corner_z);
  vgl_vector_3d<unsigned> num_voxels(dim_x, dim_y, dim_z);

  bvxm_world_params params;
  params.set_params(world_dir, corner, num_voxels, voxel_size, lvcs, min_ocp_prob, max_ocp_prob, max_scale);
  params.write_xml(scene_xml, lvcs_file);

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

  // find the bounding box from the given region
  vgl_polygon<double> poly = bkml_parser::parse_polygon(roi_kml);
  if (poly[0].size() == 0) {
    std::cerr << pro.name() << ": can not get region from input kml: " << roi_kml << "!\n";
    return false;
  }
  vgl_box_2d<double> bbox_rect;
  for (auto i : poly[0])
    bbox_rect.add(i);
  double square_size = (bbox_rect.width() >= bbox_rect.height()) ? bbox_rect.width() : bbox_rect.height();
  vgl_box_2d<double> bbox(bbox_rect.min_point(), square_size, square_size, vgl_box_2d<double>::min_pos);

  // truncate the world size from given voxel size
  double world_size = (unsigned)std::ceil(world_size_in / voxel_size)*(double)voxel_size;
  // from truncated world size, calculate the min_size of the geoindex
  vgl_point_2d<double> ll(bbox_rect.min_x(), bbox_rect.min_y());
  vgl_point_2d<double> ur(bbox_rect.max_x(), bbox_rect.max_y());
  vpgl_lvcs_sptr lvcs_ll = new vpgl_lvcs(ll.y(), ll.x(), 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vpgl_lvcs_sptr lvcs_ur = new vpgl_lvcs(ur.y(), ur.x(), 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  double scale_ll_x, scale_ll_y, gz;
  lvcs_ll->local_to_global(world_size, world_size, 0.0, vpgl_lvcs::wgs84, scale_ll_x, scale_ll_y, gz);
  scale_ll_x -= ll.x();  scale_ll_y -= ll.y();
  double scale_ur_x, scale_ur_y;
  lvcs_ur->local_to_global(world_size, world_size, 0.0, vpgl_lvcs::wgs84, scale_ur_x, scale_ur_y, gz);
  scale_ur_x -= ur.x();
  scale_ur_y -= ur.y();

  std::set<double> scale_set;
  scale_set.insert(scale_ur_x);  scale_set.insert(scale_ur_y);  scale_set.insert(scale_ll_x);  scale_set.insert(scale_ll_y);
  double min_size = *scale_set.begin();


  // create 2-d quad-tree
  // each leaf refers to a bvxm scene and scene content is unban land ratio of the scene, if land mask is available, otherwise -1.0
  bvgl_2d_geo_index_node_sptr root = bvgl_2d_geo_index::construct_tree<float>(bbox, min_size, poly);
  std::string txt_filename = scene_root + "/geo_index.txt";
  bvgl_2d_geo_index::write(root, txt_filename, min_size);
  unsigned tree_depth = bvgl_2d_geo_index::depth(root);
  std::vector<bvgl_2d_geo_index_node_sptr> leaves;
  bvgl_2d_geo_index::get_leaves(root, leaves);
  std::cout << "the geo-index has " << leaves.size() << " leaves and depth is " << tree_depth << std::endl;


  // initialize the urban land ratio
  for (auto & leave : leaves) {
    auto* leaf_ptr = dynamic_cast<bvgl_2d_geo_index_node<float>*>(leave.ptr());
    leaf_ptr->contents_ = -1.0f;
  }

  // load height images
  std::vector<volm_img_info> dem_infos;
  volm_io_tools::load_aster_dem_imgs(dem_folder, dem_infos);
  if (dem_infos.empty()) {
    std::cerr << pro.name() << ": can not load any height map from: " << dem_folder << "!\n";
    return false;
  }

  // load land masks
  std::vector<volm_img_info> land_infos;
  volm_io_tools::load_geocover_imgs(land_folder, land_infos);
  bool land_available = false;
  if (!land_infos.empty()) {
    land_available = true;
  }

  // create scene for each leaf, note that the scene size is different (in meters)
  double height_dif_max = 0.0;
  std::string lvcs_folder = scene_root + "/lvcs";
  if (!(vul_file::exists(lvcs_folder) && vul_file::is_directory(lvcs_folder)))
    if (!vul_file::make_directory(lvcs_folder)) {
      std::cerr << pro.name() << ": creating lvcs folder " << lvcs_folder << " failed!\n";
      return false;
    }
  std::cout << "pre-defined world size: " << world_size_in << ", voxel size: " << voxel_size << ", truncated world size: " << world_size << std::endl;
  std::cout << "bounding box for input region: " << bbox_rect << " expending to square: " << bbox << std::endl;
  std::cout << "lower  left: " << ll << " --> scale: " << scale_ll_x << ", " << scale_ll_y << std::endl;
  std::cout << "upper right: " << ur << " --> scale: " << scale_ur_x << ", " << scale_ur_y << std::endl;
  std::cout << "scene size: " << min_size << std::flush << std::endl;
  std::cout << "loaded " << dem_infos.size() << " DEM tiles!\n";
  std::cout << "loaded " << land_infos.size() << " Land mask!\n";
  std::cout << "the geo-index has " << leaves.size() << " leaves and depth is " << tree_depth << std::endl;
  std::cout << "Start to generate " << leaves.size() << " scenes: ";
  for (unsigned i = 0; i < leaves.size(); i++)
  {
    if (i % 100 == 0)
      std::cout << i << '.' << std::flush;

    vgl_point_2d<double> lower_left  = leaves[i]->extent_.min_point();
    vgl_point_2d<double> upper_right = leaves[i]->extent_.max_point();
    // find the evaluation information
    double min = 10000.0, max = -10000.0;
    if (!volm_io_tools::find_min_max_height(lower_left, upper_right, dem_infos, min, max)) {
      std::cerr << pro.name() << ": can not find height info for leave " << i << ", lower left: " << lower_left << ", upper right: " << upper_right << "!\n";
      return false;
    }
    double h_diff = max-min;
    if (h_diff > height_dif_max)
      height_dif_max = h_diff;
    // create scene lvcs
    vpgl_lvcs_sptr lvcs = new vpgl_lvcs(lower_left.y(), lower_left.x(), min-height_sub, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
    // create scene based on leaf size in lat/lon
    vgl_point_3d<float> corner(0.0f, 0.0f, 0.0f);
    double lx, ly, lz;
    lvcs->global_to_local(upper_right.x(), upper_right.y(), max+height, vpgl_lvcs::wgs84, lx, ly, lz);
    auto dim_x = (unsigned)std::ceil(lx / voxel_size);
    auto dim_y = (unsigned)std::ceil(ly / voxel_size);
    auto dim_z = (unsigned)std::ceil(lz / voxel_size);
    vgl_vector_3d<unsigned> num_voxels(dim_x, dim_y, dim_z);
    std::stringstream scene_world;
    scene_world << world_dir << "/scene_" << i;
    if (!(vul_file::exists(scene_world.str()) && vul_file::is_directory(scene_world.str())))
      if (!vul_file::make_directory(scene_world.str())) {
        std::cerr << pro.name() << ": creating scene world folder " << scene_world.str() << " failed!\n";
        return false;
      }
    bvxm_world_params params;
    params.set_params(scene_world.str(), corner, num_voxels, voxel_size, lvcs);

    // write out
    std::stringstream scene_file;  scene_file << scene_root  << "/scene_" << i << ".xml";
    std::stringstream lvcs_file;   lvcs_file  << lvcs_folder << "/scene_" << i << ".lvcs";
    std::ofstream ofs(lvcs_file.str().c_str());
    if (!ofs) {
      std::cerr << pro.name() << ": can not open file: " << lvcs_file.str() << "!\n";
      return false;
    }
    lvcs->write(ofs);
    ofs.close();
    params.write_xml(scene_file.str(), lvcs_file.str());

    // calculate urban ratio if available
    if (land_available) {
      unsigned land_id;
      for (unsigned l_idx = 0; l_idx < land_infos.size(); l_idx++) {
        if (!vgl_intersection(land_infos[l_idx].bbox, leaves[i]->extent_).is_empty()) {
          land_id = l_idx;
          break;
        }
      }
      volm_img_info land_cover = land_infos[land_id];
      auto* land_img = dynamic_cast<vil_image_view<vxl_byte>*>(land_cover.img_r.ptr());

      unsigned ni = dim_x;
      unsigned nj = dim_y;
      unsigned urban_pixels = 0;
      for (unsigned ii = 0; ii < ni; ii++) {
        for (unsigned jj = 0; jj < nj; jj++) {
          double lon, lat, gz;
          auto local_x = (float)(ii+0.5f);
          auto local_y = (float)(nj-jj+0.5);
          lvcs->local_to_global(local_x, local_y, 0, vpgl_lvcs::wgs84, lon, lat, gz);
          double u, v;
          land_cover.cam->global_to_img(lon, lat, gz, u, v);
          auto uu = (unsigned)std::floor(u+0.5);
          auto vv = (unsigned)std::floor(v+0.5);
          if (uu > 0 && vv > 0 && uu < land_cover.ni && vv < land_cover.nj) {
            if ((*land_img)(uu,vv) == volm_osm_category_io::GEO_URBAN) {
              urban_pixels++;
            }
          }
        }
      }

      double urban_ratio = (double)urban_pixels / (double)(ni*nj);
      auto* leaf_ptr = dynamic_cast<bvgl_2d_geo_index_node<float>*>(leaves[i].ptr());
      leaf_ptr->contents_ = urban_ratio;
    }
  } // end of the leaf loop

  // save the urban ratio in a txt file
  std::string urban_txt = scene_root + "/scene_urban_ratio.txt";
  std::ofstream ofs_urban(urban_txt.c_str());
  ofs_urban << "scene_id   urban_ratio\n";
  for (unsigned i = 0; i < leaves.size(); i++) {
    auto* leaf_ptr = dynamic_cast<bvgl_2d_geo_index_node<float>*>(leaves[i].ptr());
    ofs_urban << i << ' ' << leaf_ptr->contents_ << '\n';
  }
  ofs_urban.close();

  // write the kml for visualization
  std::string kml_file = scene_root + "/scene_geo_index.kml";
  std::ofstream ofs(kml_file.c_str());
  bkml_write::open_document(ofs);
  for (unsigned i = 0; i < leaves.size(); i++) {
    auto* leaf_ptr = dynamic_cast<bvgl_2d_geo_index_node<float>*>(leaves[i].ptr());
    std::stringstream name;  name << "scene_" << i;
    std::stringstream description; description << "scene_" << i << "_urban_" << leaf_ptr->contents_;
    bvgl_2d_geo_index::write_to_kml_node(ofs, leaves[i], 0, 0, description.str(), name.str());
  }
  bkml_write::close_document(ofs);
  ofs.close();

  std::cout << "\nDONE!!! largest height difference in the entire region is: " << height_dif_max << std::endl;

  // generate output
  pro.set_output_val<unsigned>(0, leaves.size());
  return true;
}
