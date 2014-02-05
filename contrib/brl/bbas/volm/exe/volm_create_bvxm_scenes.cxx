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
#include <volm/volm_tile.h>
#include <vcl_iostream.h>
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
  vul_arg<vcl_string> in_folder("-in", "input folder to read DEM files as .tif", "");
  vul_arg<vcl_string> in_poly("-poly", "region polygon as kml, the scenes that cover this polygon will be created", "");
  vul_arg<vcl_string> out_folder("-out", "folder to write xml files","");   
  vul_arg<vcl_string> world_root("-world_dir", "the world folder where bvxm vox binary will be stored","");
  vul_arg<float> voxel_size("-vox", "size of voxel in meters", 1.0f);
  vul_arg<float> world_size_input("-size", "the size of the world in meters", 500.0f);
  vul_arg<float> height("-height", "the amount to be added on top of the terrain height to create the scene in meters", 0.0f);

  vul_arg_parse(argc, argv);

  // check input
  if (in_folder().compare("") == 0 || in_poly().compare("") == 0 || out_folder().compare("") == 0 || world_root().compare("") == 0) {
    vcl_cerr << " ERROR: input is missing!\n";
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  
  vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly());
  vcl_cout << "outer poly  has: " << poly[0].size() << vcl_endl;

  // find the bbox of ROI from its polygon
  vgl_box_2d<double> bbox_rect;
  for (unsigned i = 0; i < poly[0].size(); i++) {
    bbox_rect.add(poly[0][i]);
  }
  double square_size = (bbox_rect.width() >= bbox_rect.height()) ? bbox_rect.width() : bbox_rect.height();
  vgl_box_2d<double> bbox(bbox_rect.min_point(), square_size, square_size, vgl_box_2d<double>::min_pos);

  // truncate the world size from voxel size
  double world_size = (unsigned)vcl_ceil(world_size_input()/voxel_size())*(double)voxel_size();

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
  vcl_cout << " pre-defined world_size is " << world_size_input() << " and voxel size is " << voxel_size() << " actual world size is " << world_size << vcl_endl;
  vcl_cout << " bounding box for input polygon: " << bbox_rect << " expending to square: " << bbox << vcl_endl;
  vcl_cout << " ll: " << ll << " ---> " << " scale_x = " << scale_ll_x << " scale_y = " << scale_ll_y << vcl_endl;
  vcl_cout << " ur: " << ur << " ---> " << " scale_x = " << scale_ur_x << " scale_y = " << scale_ur_y << vcl_endl;
  vcl_set<double> scale_set;
  scale_set.insert(scale_ur_x);  scale_set.insert(scale_ur_y);  scale_set.insert(scale_ll_x);  scale_set.insert(scale_ll_y);
  double min_size = *scale_set.begin();
  vcl_cout << " given maximum allowd world size " << world_size_input() << " the min_size for geo index is: " << min_size << vcl_endl;

  // create a geo index and use the leaves as scenes, use template param as volm_loc_hyp_sptr but it won't actually be used
  volm_geo_index2_node_sptr root = volm_geo_index2::construct_tree<volm_loc_hyp_sptr>(bbox, min_size, poly);
  vcl_string txt_filename = out_folder() + "/geo_index.txt";
  volm_geo_index2::write(root, txt_filename, min_size);
  vcl_string kml_filename = out_folder() + "/scene_geo_index.kml";
  unsigned tree_depth = volm_geo_index2::depth(root);
  volm_geo_index2::write_to_kml(root, tree_depth, kml_filename);
  vcl_vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root, leaves);
  vcl_cout << "the geoindex quadtree for scene has " << leaves.size() << " leaves and depth is " << tree_depth << vcl_endl;

  // load DEM images
  vcl_vector<volm_img_info> infos;
  volm_io_tools::load_aster_dem_imgs(in_folder(), infos);
  vcl_cout << " loaded " << infos.size() << " DEM tiles!\n";

  // create scenes for each leaf, note the scene size is different
  double largest_dif = 0;
  for (unsigned i = 0; i < leaves.size(); i++) {
    vcl_stringstream name;  name << out_folder() << "scene_" << i;
    vgl_point_2d<double> lower_left = leaves[i]->extent_.min_point();
    vgl_point_2d<double> upper_right = leaves[i]->extent_.max_point();
    // find the maximum elevation difference
    double min = 10000.0, max = -10000.0;
    if (!volm_io_tools::find_min_max_height(lower_left, upper_right, infos, min, max)) {
      vcl_cerr << " problems in the leaf: " << lower_left << " " << upper_right << " - cannot find height!\n";
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    double dif = max-min;
    if (dif > largest_dif) largest_dif = dif;
    //construct lvcs
    vpgl_lvcs_sptr lvcs = new vpgl_lvcs(lower_left.y(), lower_left.x(), min, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
    vcl_string lvcs_name = name.str() + ".lvcs";
    vcl_ofstream ofs(lvcs_name.c_str());
    if (!ofs) {
      vcl_cerr << "Cannot open file: " << lvcs_name << "!\n";
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    lvcs->write(ofs);
    ofs.close();

    // create scene based on leaf size in lat/lon
    vgl_point_3d<float> corner(0.0f, 0.0f, 0.0f);
    double lx, ly, lz;
    lvcs->global_to_local(upper_right.x(), upper_right.y(), height()+max, vpgl_lvcs::wgs84, lx, ly, lz);
    unsigned dim_x = (unsigned)vcl_ceil(lx/voxel_size());
    unsigned dim_y = (unsigned)vcl_ceil(ly/voxel_size());
    unsigned dim_z = (unsigned)vcl_ceil(height()+dif);
    vgl_vector_3d<unsigned> num_voxels(dim_x, dim_y, dim_z);
    bvxm_world_params params;
    vcl_stringstream world_dir;
    world_dir << world_root() << "/scene_" << i;
    if (!vul_file::is_directory(world_dir.str()))
      vul_file::make_directory(world_dir.str());
    params.set_params(world_dir.str(), corner, num_voxels, voxel_size(), lvcs);
    vcl_string xml_name = name.str() + ".xml";
    params.write_xml(xml_name, lvcs_name);
  }

  vcl_cout << "largest height difference in the whole ROI is: " << largest_dif << vcl_endl;


#if 0
  
  
  vcl_vector<volm_img_info> infos;
  volm_io_tools::load_aster_dem_imgs(in_folder(), infos);
  vcl_cout << " loaded " << infos.size() << " DEM tiles!\n";

  double largest_dif = 0;
  for (unsigned i = 0; i < leaves.size(); i++) {
    vcl_stringstream name; name << out_folder() << "scene_" << i ;
    //vcl_cout << name.str() << vcl_endl;
    //vul_file::make_directory(dir);
    vgl_point_2d<double> lower_left = leaves[i]->extent_.min_point();
    vgl_point_2d<double> upper_right = leaves[i]->extent_.max_point();

    double min = 10000.0, max = -10000.0;
    if (!volm_io_tools::find_min_max_height(lower_left, upper_right, infos, min, max)) {
      vcl_cerr << " problems in the leaf: " << lower_left << " " << upper_right << " - cannot find height!\n";
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    double dif = max-min;
    if (dif > largest_dif) largest_dif = dif;
    //vcl_cout << "min: " << min << " " << max << "\n";

    //construct lvcs
    vpgl_lvcs_sptr lvcs = new vpgl_lvcs(lower_left.y(), lower_left.x(), min, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
    vcl_string lvcs_name = name.str() + ".lvcs";
    vcl_ofstream ofs(lvcs_name.c_str());
    if (!ofs) {
      vcl_cerr << "Cannot open file: " << lvcs_name << "!\n";
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    lvcs->write(ofs);
    ofs.close();

    vgl_point_3d<float> corner(0,0,0);
    unsigned dim_xy = (unsigned)vcl_ceil(world_size()/voxel_size());
    unsigned dim_z = (unsigned)vcl_ceil(dif+height());
    vgl_vector_3d<unsigned int> num_voxels(dim_xy, dim_xy, dim_z);
    bvxm_world_params params;
    //params.set_params(name.str(), corner, num_voxels, voxel_size(), lvcs); 
    //params.set_params(out_folder().substr(0, out_folder().size()-1), corner, num_voxels, voxel_size(), lvcs);  // for now set model dir as out_folder
    // set bvxm_scene world parameters
    vcl_stringstream world_dir;
    world_dir << world_root() << "/scene_" << i;
    if (!vul_file::is_directory(world_dir.str())) {
      vul_file::make_directory(world_dir.str());
    }
    params.set_params(world_dir.str(), corner, num_voxels, voxel_size(), lvcs);   // the world dir is now different from the out_folder where scene.xml and scene.lvcs stores
    vcl_string xml_name = name.str() + ".xml";
    params.write_xml(xml_name, lvcs_name);
  }

  vcl_cout << "largest height difference in the whole ROI is: " << largest_dif << '\n';

#endif
  return volm_io::SUCCESS;
}