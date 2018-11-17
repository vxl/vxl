//:
// \file
// \brief  executable to sort the bvxm scene regions based on their overlap with geo_cover urban region
//
// \author Yi Dong
// \date Nov 13, 2013
// \verbatim
//  Modification
//   <None yet>
// \endverbatim

#include <bkml/bkml_parser.h>
#include <ios>
#include <iostream>
#include <utility>
#include <vgl/vgl_intersection.h>
#include <vil/vil_save.h>
#include <volm/volm_geo_index2.h>
#include <volm/volm_geo_index2_sptr.h>
#include <volm/volm_io.h>
#include <volm/volm_io_tools.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


void error(std::string log_file, const std::string& msg)
{
  std::cerr << msg;  volm_io::write_post_processing_log(std::move(log_file), msg);
}

int main(int argc, char** argv)
{
  // input
  vul_arg<std::string> geo_folder("-geo", "folder where geo_cover tif images stores", "");
  vul_arg<std::string> scene_root("-scene-root", "root directory where all scene xml and lvcs files are stored", "");
  vul_arg<std::string> lvcs_root("-lvcs-root", "root directory where all scene lvcs files are stored","");
  vul_arg<std::string> out_filename("-out-file", "name of the txt file which the sorted results will be written", "./urban_sort_scene.txt");
  vul_arg<float> voxel_size("-vox", "size of voxel in meters", 1.0f);
  vul_arg<float> world_size_input("-size", "the size of the world in meters", 500.0f);
  vul_arg<std::string> in_poly("-poly", "region polygon as kml, the scenes that cover this polygon will be created", "");
  vul_arg_parse(argc, argv);

  // check input
  if (geo_folder().compare("") == 0 || in_poly().compare("") == 0 || scene_root().compare("") == 0) {
    std::cerr << " ERROR: input is missing!\n";
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::stringstream log_file;
  std::stringstream log;

  log_file << scene_root() << "/log_sort_scene_urban.xml";

  std::string lvcs_dir;
  if (lvcs_root().compare("") == 0)
    lvcs_dir = scene_root();
  else
    lvcs_dir = lvcs_root();
  std::cout << " lvcs directory is: " << lvcs_dir << std::endl;
  // load the geo_index from the folder, if tree structure not exist, create tree from given parameter
  std::string tree_txt = scene_root() + "/geo_index.txt";
  volm_geo_index2_node_sptr root;
  if (vul_file::exists(tree_txt)) {
    double min_size;
    root = volm_geo_index2::read_and_construct<volm_loc_hyp_sptr>(tree_txt, min_size);
  }
  else {
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
    scale_ur_x -= ur.x();  scale_ur_y -= ur.y();
    std::set<double> scale_set;
    scale_set.insert(scale_ur_x);  scale_set.insert(scale_ur_y);  scale_set.insert(scale_ll_x);  scale_set.insert(scale_ll_y);
    double min_size = *scale_set.begin();
    // create a geo index and use the leaves as scenes, use template param as volm_loc_hyp_sptr but it won't actually be used
    root = volm_geo_index2::construct_tree<volm_loc_hyp_sptr>(bbox, min_size, poly);
  }

  std::vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root, leaves);
  std::cout << "the scene has " << leaves.size() << " leaves and depth is " << volm_geo_index2::depth(root) << std::endl;

  // load geo_cover image
  std::vector<volm_img_info> geo_info;
  volm_io_tools::load_geocover_imgs(geo_folder(), geo_info);
  std::cout << geo_info.size() << " geo cover images are loaded" << std::endl;

  // loop over all scene lvcs file
  std::multimap<double, std::string> scene_order;  // sorted list of scene xmls

  std::string file_glob = lvcs_dir + "/*.lvcs";
  unsigned cnt=0;
  for (vul_file_iterator fn = file_glob.c_str(); fn; ++fn) {
    std::cout << "working on lvcs: " << fn() << std::endl;
    std::string lvcs_file = fn();
    //std::string scene_file = vul_file::strip_extension(lvcs_file) + ".xml";
    //std::string scene_name = vul_file::strip_directory(scene_file);
    std::string scene_name = vul_file::strip_extension(vul_file::strip_directory(lvcs_file));
    std::string name = scene_name.substr(scene_name.find_first_of('_')+1, scene_name.size());
    std::stringstream str(name);
    unsigned scene_id;
    str >> scene_id;
    std::string scene_file = scene_root() + "/" + scene_name + ".xml";

    if (!vul_file::exists(lvcs_file) || !vul_file::exists(scene_file)) {
      log << "ERROR: can not find scene " << lvcs_file << '\n';  error(log_file.str(), log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }

    // load the lvcs
    vpgl_lvcs_sptr lvcs = new vpgl_lvcs;
    std::ifstream ifs(lvcs_file.c_str());
    if (!ifs.good()) {
      log << "ERROR: load lvcs: " << lvcs_file << " failed\n";  error(log_file.str(), log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    lvcs->read(ifs);
    ifs.close();

    // obtain the bounding box from the leaf
    vgl_box_2d<double> bbox_geo = leaves[scene_id]->extent_;
    double lon_min, lat_min, lon_max, lat_max;
    lon_min = bbox_geo.min_x();  lat_min = bbox_geo.min_y();
    lon_max = bbox_geo.max_x();  lat_max = bbox_geo.max_y();

    double lvcs_ori_lon, lvcs_ori_lat, lvcs_ori_elev;
    lvcs->get_origin(lvcs_ori_lat, lvcs_ori_lon, lvcs_ori_elev);

    if ( (lvcs_ori_lat-lat_min)*(lvcs_ori_lat-lat_min) > 1e-5 || (lvcs_ori_lon-lon_min)*(lvcs_ori_lon-lon_min) > 1e-5) {
      log << " ERROR: mismatch for leaf extent " << bbox_geo << " with lvcs origin (" << lvcs_ori_lon << ", " << lvcs_ori_lat << ") for scene " << scene_id << '\n';
      error(log_file.str(), log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
    }

    // locate the geo_cover image which intersects with current leaf
    unsigned geo_cover_id;
    for (unsigned g_idx = 0; g_idx < geo_info.size(); g_idx++) {
      if (!vgl_intersection(geo_info[g_idx].bbox, bbox_geo).is_empty()) {
        geo_cover_id = g_idx;
        break;
      }
    }
    volm_img_info geo_cover = geo_info[geo_cover_id];
    auto* geo_img = dynamic_cast<vil_image_view<vxl_byte> * >(geo_cover.img_r.ptr());

    // create the image associated with current scene
    double box_lx, box_ly, box_lz;
    lvcs->global_to_local(lon_max, lat_max, 0, vpgl_lvcs::wgs84, box_lx, box_ly, box_lz);

    auto ni = (unsigned)std::ceil(box_lx);
    auto nj = (unsigned)std::ceil(box_ly);
#if 0
    vil_image_view<vxl_byte> out_img(ni, nj, 1);
    out_img.fill(0);
#endif

    // count the urban pixels
    unsigned urban_pixels = 0;
    for (unsigned i = 0; i < ni; i++) {
      for (unsigned j = 0; j < nj; j++) {
        // transfer coords to get geo cover pixel
        double lon, lat, gz;
        auto local_x = (float)(i+0+0.5);
        auto local_y = (float)(box_ly-j+0.5);
        lvcs->local_to_global(local_x, local_y, 0, vpgl_lvcs::wgs84, lon, lat, gz);
        double u, v;
        geo_cover.cam->global_to_img(lon, lat, gz, u, v);
        auto uu = (unsigned)std::floor(u+0.5);
        auto vv = (unsigned)std::floor(v+0.5);
        if (uu > 0 && vv > 0 && uu < geo_cover.ni && vv < geo_cover.nj)
          if ((*geo_img)(uu,vv) == volm_osm_category_io::GEO_URBAN) {
            //out_img(i,j) = 255;
            urban_pixels++;
          }
      }
    }

    // calculate ration of the urban region
    double urban_ratio = (double)urban_pixels/(double)(ni*nj);
    scene_order.insert(std::pair<double, std::string>(urban_ratio, scene_name));
    std::cout << " for scene " << scene_id << " img size is " << ni << " x " << nj
             << ", urban pixels are " << urban_pixels << " and the urban ration is: " << urban_ratio << std::endl;
#if 0
    std::cout << " for scene " << scene_id << " img size is " << ni << " x " << nj
             << ", urban pixels are " << urban_pixels << " and the urban ration is: " << urban_ratio << std::endl;
    // output an image for debug purpose
    std::stringstream out_img_name;
    out_img_name << scene_root() << "/urban_img_" << scene_id << ".tif";
    vil_save(out_img, out_img_name.str().c_str());
#endif
    cnt++;
    if (cnt % 1000 == 0) {
      std::cout << cnt << '.';
    }
  } // end of loop over all lvcs files

  // output to a text file
  std::string out_txt = out_filename();

  std::ofstream ofs(out_txt.c_str());
  ofs << "urban_ratio \t\t scene_xml\n";

  auto mit = scene_order.end();
  --mit;
  for (; mit != scene_order.begin(); --mit) {\
    ofs.precision(7);  ofs.width(14);
    ofs << std::setiosflags(std::ios::left) << mit->first << ' ' << mit->second << '\n';
  }
  ofs.close();

  return volm_io::SUCCESS;
}
