//:
// \file
// \brief  executable to load open street map date to create associated volumetric osm data
//
// \author Yi Dong
// \date August 07, 2013
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
#include <iostream>
#include <utility>
#include <volm/volm_tile.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <volm/volm_osm_objects.h>
#include <volm/volm_geo_index2.h>
#include <volm/volm_geo_index2_sptr.h>
#include <volm/volm_io.h>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>
#include <vcl_where_root_dir.h>

static void error_report(std::string error_file, const std::string& error_msg)
{
  std::cerr << error_msg;
  volm_io::write_post_processing_log(std::move(error_file), error_msg);
}


int main(int argc, char** argv)
{
  // input
  vul_arg<std::string> osm_file("-osm", "open street map xml file","");       // open street map xml file
  vul_arg<std::string> osm_out("-out", "output folder", "");                  // output folder to store the create binary file
  vul_arg<bool> is_create_osm_kml("-k", "option to create kml file from parsed osm file", false);
  vul_arg_parse(argc, argv);

  // check input
  if (osm_file().compare("") == 0 || osm_out().compare("") == 0) {
    std::cerr << " ERROR: input is missing!\n";
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // obtain the dirname
  std::string out_folder = vul_file::dirname(osm_out());
  std::string filename = vul_file::strip_extension(vul_file::strip_directory(osm_out()));

  std::stringstream log;
  std::stringstream err_log_file;
  err_log_file << out_folder << "/log_" << filename << ".xml";

  // check the osm_file
  if (!vul_file::exists(osm_file())) {
    log << "ERROR: can not find osm xml file: " << osm_file() << '\n';
    error_report(err_log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  // check the osm_to_volm file
  std::string osm_to_volm_txt = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bbas/volm/osm_to_volm_labels.txt";
  if (!vul_file::exists(osm_to_volm_txt)) {
    log << "ERROR: can not find osm_to_volm txt file: " << osm_to_volm_txt << '\n';
    error_report(err_log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::cout << " =========== Start to create volm_osm object ===============" << std::endl;
  volm_osm_objects objs(osm_file(), osm_to_volm_txt);
  std::cout << " \t number of location points parsed from osm: " << objs.num_locs() << std::endl;
  std::cout << " \t number of line roads parsed from osm: " << objs.num_roads() << std::endl;
  std::cout << " \t number of regions parsed from osm: " << objs.num_regions() << std::endl;
  // write to binary
  objs.write_osm_objects(osm_out());
  // write osm to kml
  if (is_create_osm_kml()) {
    std::string kml_pts   = out_folder + "/" + filename + "_osm_pts.kml";
    std::string kml_roads = out_folder + "/" + filename + "_osm_roads.kml";
    std::string kml_regions = out_folder + "/" + filename + "_osm_regions.kml";
    objs.write_pts_to_kml(kml_pts);
    objs.write_lines_to_kml(kml_roads);
    objs.write_polys_to_kml(kml_regions);
  }
  std::cout << " ========================================== Finish ===============================================" << std::endl;
  std::cout << " \t results are in : " << out_folder << std::endl;
  std::cout << " ========================================== Finish ===============================================" << std::endl;
  return volm_io::SUCCESS;
#if 0
  // error log
  std::stringstream log;
  std::stringstream err_log_file;
  err_log_file << osm_out() << "/log_wr" << world_str() << "_tile_" << tile_id() << ".xml";

  std::string phase_str;
  if (world_str() == "desert" || world_str() == "coast")
    phase_str = "a";
  else if (world_str() == "chile" || world_str() == "india" || world_str() == "jordan" || world_str() == "philippines" || world_str() == "taiwan")
    phase_str = "b";
  else {
    log << "ERROR: unknown ROI world: " << world_str() << '\n';  error_report(err_log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  unsigned world_id;
  if (world_str() == "desert")            world_id = 1;
  else if (world_str() == "coast")        world_id = 2;
  else if (world_str() == "chile")        world_id = 1;
  else if (world_str() == "india")        world_id = 2;
  else if (world_str() == "jordan")       world_id = 3;
  else if (world_str() == "philippines")  world_id = 4;
  else if (world_str() == "taiwan")       world_id = 5;

  // check the osm_file
  if (!vul_file::exists(osm_file())) {
    log << "ERROR: can not find osm xml file: " << osm_file() << '\n';
    error_report(err_log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  // check the osm_to_volm file
  std::string osm_to_volm_txt = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bbas/volm/osm_to_volm_labels.txt";
  if (!vul_file::exists(osm_to_volm_txt)) {
    log << "ERROR: can not find osm_to_volm txt file: " << osm_to_volm_txt << '\n';
    error_report(err_log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // create volm_osm_binary and associate kml files
  std::cout << " =========== Start to create volm_osm date for world " << world_str() << " on tile " << tile_id()
            << " ===============" << std::endl;
  volm_osm_objects objs(osm_file(), osm_to_volm_txt);
  std::cout << " \t number of location points parsed from osm: " << objs.num_locs() << std::endl;
  std::cout << " \t number of line roads parsed from osm: " << objs.num_roads() << std::endl;
  std::cout << " \t number of regions parsed from osm: " << objs.num_regions() << std::endl;
  // write to binary
  std::stringstream osm_bin;
  osm_bin << osm_out() << "/p1" << phase_str << "_wr" << world_id << "_tile_" << tile_id() << "_osm.bin";
  objs.write_osm_objects(osm_bin.str());
  // write osm to kml
  if (is_create_osm_kml()) {
    std::cout << " =========== Parse the osm xml to kml file " << world_id << " on tile " << tile_id()
              << " ===============" << std::endl;
    std::stringstream kml_pts, kml_roads, kml_regions;
    kml_pts << osm_out() << "/p1" << phase_str << "_wr" << world_id << "_tile_" << tile_id() << "_osm_pts.kml";
    kml_roads << osm_out() << "/p1" << phase_str << "_wr" << world_id << "_tile_" << tile_id() << "_osm_roads.kml";
    kml_regions << osm_out() << "/p1" << phase_str << "_wr" << world_id << "_tile_" << tile_id() << "_osm_regions.kml";
    objs.write_pts_to_kml(kml_pts.str());
    objs.write_lines_to_kml(kml_roads.str());
    objs.write_polys_to_kml(kml_regions.str());
  }
  std::cout << " ========================================== Finish ===============================================" << std::endl;
  std::cout << " \t results are in : " << osm_out() << std::endl;
  std::cout << " ========================================== Finish ===============================================" << std::endl;
#endif
}
