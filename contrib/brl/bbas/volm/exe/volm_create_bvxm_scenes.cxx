//:
// \file
// \brief  executable to take a polygonal ROI and cover it with bvxm scenes with different lvcs's
//
// \author Ozge C. Ozcanli
// \date August 22, 2013
// \verbatim
//  Modifications
//   Yi Dong         Feb-2014   modify it to use geo coordinated geo_index leaf size to define the scene size
// \endverbatim
//
#include <iostream>
#include <volm/volm_tile.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <volm/volm_osm_objects.h>
#include <volm/volm_geo_index2.h>
#include <volm/volm_io.h>
#include <volm/volm_io_tools.h>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>
#include <vcl_where_root_dir.h>
#include <vpgl/vpgl_lvcs.h>
#include <bkml/bkml_parser.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <bvxm/bvxm_world_params.h>
#include <bkml/bkml_write.h>
#include <vpgl/vpgl_utm.h>


int main(int argc, char** argv)
{
  // input
  vul_arg<std::string> in_folder("-in", "input folder to read DEM files as .tif", "");
  vul_arg<std::string> in_poly("-poly", "region polygon as kml, the scenes that cover this polygon will be created", "");
  vul_arg<std::string> out_folder("-out", "folder to write xml files","");
  vul_arg<std::string> lvcs_root("-lvcs", "folder to write lvcs files","");
  vul_arg<std::string> world_root("-world_dir", "the world folder where bvxm vox binary will be stored","");
  vul_arg<float> voxel_size("-vox", "size of voxel in meters", 1.0f);
  vul_arg<float> world_size_input("-size", "the size of the world in meters", 500.0f);
  vul_arg<float> height("-height", "the amount to be added on top of the terrain height to create the scene in meters", 0.0f);

  vul_arg_parse(argc, argv);

  // check input
  if (in_folder().compare("") == 0 || in_poly().compare("") == 0 || out_folder().compare("") == 0 || world_root().compare("") == 0) {
    std::cerr << " ERROR: input is missing!\n";
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  std::string lvcs_folder;
  if (lvcs_root() == "")
    lvcs_folder = out_folder();
  else
    lvcs_folder = lvcs_root();

  vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly());
  std::cout << "outer poly  has: " << poly[0].size() << std::endl;

  // find the bbox of ROI from its polygon
  vgl_box_2d<double> bbox_rect;
  for (auto i : poly[0]) {
    bbox_rect.add(i);
  }
  double square_size = (bbox_rect.width() >= bbox_rect.height()) ? bbox_rect.width() : bbox_rect.height();
  vgl_box_2d<double> bbox(bbox_rect.min_point(), square_size, square_size, vgl_box_2d<double>::min_pos);

  // truncate the world size from voxel size
  double world_size = (unsigned)std::ceil(world_size_input()/voxel_size())*(double)voxel_size();

  // from defined world size, calculate the min_size of the geoindex
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
  std::cout << " pre-defined world_size is " << world_size_input() << " and voxel size is " << voxel_size() << " actual world size is " << world_size << std::endl;
  std::cout << " bounding box for input polygon: " << bbox_rect << " expending to square: " << bbox << std::endl;
  std::cout << " ll: " << ll << " ---> " << " scale_x = " << scale_ll_x << " scale_y = " << scale_ll_y << std::endl;
  std::cout << " ur: " << ur << " ---> " << " scale_x = " << scale_ur_x << " scale_y = " << scale_ur_y << std::endl;
  std::set<double> scale_set;
  scale_set.insert(scale_ur_x);  scale_set.insert(scale_ur_y);  scale_set.insert(scale_ll_x);  scale_set.insert(scale_ll_y);
  double min_size = *scale_set.begin();
  std::cout << " given maximum allowed world size " << world_size_input() << " the min_size for geo index is: " << min_size << std::endl;

  // create a geo index and use the leaves as scenes, use template param as volm_loc_hyp_sptr but it won't actually be used
  volm_geo_index2_node_sptr root = volm_geo_index2::construct_tree<volm_loc_hyp_sptr>(bbox, min_size, poly);
  std::string txt_filename = out_folder() + "/geo_index.txt";
  volm_geo_index2::write(root, txt_filename, min_size);
  std::string kml_filename = out_folder() + "/scene_geo_index.kml";
  unsigned tree_depth = volm_geo_index2::depth(root);
  //volm_geo_index2::write_to_kml(root, tree_depth, kml_filename);
  std::vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root, leaves);
  std::cout << "the geo-index quad tree for scene has " << leaves.size() << " leaves and depth is " << tree_depth << std::endl;

  // write to kml files with leaf ids (which will be used as scene ids)
  std::ofstream ofs(kml_filename.c_str());
  bkml_write::open_document(ofs);
  for (unsigned i = 0; i < leaves.size(); i++) {
    std::stringstream explanation; explanation << "scene_" << i;
    volm_geo_index2::write_to_kml_node(ofs, leaves[i], 0, 0, explanation.str());
  }
  bkml_write::close_document(ofs);

  // load DEM images
  std::vector<volm_img_info> infos;
  volm_io_tools::load_aster_dem_imgs(in_folder(), infos);
  std::cout << " loaded " << infos.size() << " DEM tiles!\n";

  // create scenes for each leaf, note the scene size is different
  double largest_dif = 0;
  for (unsigned i = 0; i < leaves.size(); i++) {
    std::stringstream name;  name << out_folder() << "scene_" << i;
    vgl_point_2d<double> lower_left = leaves[i]->extent_.min_point();
    vgl_point_2d<double> upper_right = leaves[i]->extent_.max_point();
    // find the maximum elevation difference
    double min = 10000.0, max = -10000.0;
    if (!volm_io_tools::find_min_max_height(lower_left, upper_right, infos, min, max)) {
      std::cerr << " problems in the leaf: " << lower_left << " " << upper_right << " - cannot find height!\n";
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    double dif = max-min;
    if (dif > largest_dif) largest_dif = dif;
    //construct lvcs
    vpgl_lvcs_sptr lvcs = new vpgl_lvcs(lower_left.y(), lower_left.x(), min, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
    std::stringstream name_lvcs;  name_lvcs << lvcs_folder << "/scene_" << i;
    std::string lvcs_name = name_lvcs.str() + ".lvcs";
    std::ofstream ofs(lvcs_name.c_str());
    if (!ofs) {
      std::cerr << "Cannot open file: " << lvcs_name << "!\n";
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    lvcs->write(ofs);
    ofs.close();

    // create scene based on leaf size in lat/lon
    vgl_point_3d<float> corner(0.0f, 0.0f, 0.0f);
    double lx, ly, lz;
    lvcs->global_to_local(upper_right.x(), upper_right.y(), height()+max, vpgl_lvcs::wgs84, lx, ly, lz);
    auto dim_x = (unsigned)std::ceil(lx/voxel_size());
    auto dim_y = (unsigned)std::ceil(ly/voxel_size());
    auto dim_z = (unsigned)std::ceil(height()+dif);
    vgl_vector_3d<unsigned> num_voxels(dim_x, dim_y, dim_z);
    bvxm_world_params params;
    std::stringstream world_dir;
    world_dir << world_root() << "/scene_" << i;
    if (!vul_file::is_directory(world_dir.str()))
      vul_file::make_directory(world_dir.str());
    params.set_params(world_dir.str(), corner, num_voxels, voxel_size(), lvcs);
    std::string xml_name = name.str() + ".xml";
    params.write_xml(xml_name, lvcs_name);
  }

  std::cout << "largest height difference in the whole ROI is: " << largest_dif << std::endl;


#if 0


  std::vector<volm_img_info> infos;
  volm_io_tools::load_aster_dem_imgs(in_folder(), infos);
  std::cout << " loaded " << infos.size() << " DEM tiles!\n";

  double largest_dif = 0;
  for (unsigned i = 0; i < leaves.size(); i++) {
    std::stringstream name; name << out_folder() << "scene_" << i ;
    //std::cout << name.str() << std::endl;
    //vul_file::make_directory(dir);
    vgl_point_2d<double> lower_left = leaves[i]->extent_.min_point();
    vgl_point_2d<double> upper_right = leaves[i]->extent_.max_point();

    double min = 10000.0, max = -10000.0;
    if (!volm_io_tools::find_min_max_height(lower_left, upper_right, infos, min, max)) {
      std::cerr << " problems in the leaf: " << lower_left << " " << upper_right << " - cannot find height!\n";
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    double dif = max-min;
    if (dif > largest_dif) largest_dif = dif;
    //std::cout << "min: " << min << " " << max << "\n";

    //construct lvcs
    vpgl_lvcs_sptr lvcs = new vpgl_lvcs(lower_left.y(), lower_left.x(), min, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
    std::string lvcs_name = name.str() + ".lvcs";
    std::ofstream ofs(lvcs_name.c_str());
    if (!ofs) {
      std::cerr << "Cannot open file: " << lvcs_name << "!\n";
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    lvcs->write(ofs);
    ofs.close();

    vgl_point_3d<float> corner(0,0,0);
    unsigned dim_xy = (unsigned)std::ceil(world_size()/voxel_size());
    unsigned dim_z = (unsigned)std::ceil(dif+height());
    vgl_vector_3d<unsigned int> num_voxels(dim_xy, dim_xy, dim_z);
    bvxm_world_params params;
    //params.set_params(name.str(), corner, num_voxels, voxel_size(), lvcs);
    //params.set_params(out_folder().substr(0, out_folder().size()-1), corner, num_voxels, voxel_size(), lvcs);  // for now set model dir as out_folder
    // set bvxm_scene world parameters
    std::stringstream world_dir;
    world_dir << world_root() << "/scene_" << i;
    if (!vul_file::is_directory(world_dir.str())) {
      vul_file::make_directory(world_dir.str());
    }
    params.set_params(world_dir.str(), corner, num_voxels, voxel_size(), lvcs);   // the world dir is now different from the out_folder where scene.xml and scene.lvcs stores
    std::string xml_name = name.str() + ".xml";
    params.write_xml(xml_name, lvcs_name);
  }

  std::cout << "largest height difference in the whole ROI is: " << largest_dif << '\n';

#endif
  return volm_io::SUCCESS;
}
