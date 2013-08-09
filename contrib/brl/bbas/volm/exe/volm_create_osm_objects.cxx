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
#include <volm/volm_tile.h>
#include <vcl_iostream.h>
#include <volm/volm_osm_objects.h>
#include <volm/volm_geo_index2.h>
#include <volm/volm_geo_index2_sptr.h>
#include <volm/volm_io.h>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>
#include <vcl_where_root_dir.h>

void error_report(vcl_string error_file, vcl_string error_msg)
{
  vcl_cerr << error_msg;
  volm_io::write_post_processing_log(error_file, error_msg);
}


int main(int argc, char** argv)
{
  // input
  vul_arg<vcl_string> osm_file("-osm", "open street map xml file","");       // open street map xml file
  vul_arg<vcl_string> osm_out("-out", "output folder", "");                  // output folder to store the create binary file
  vul_arg<bool> is_create_osm_kml("-k", "option to create kml file from parsed osm file", false);
  vul_arg<unsigned> world_id("-w", "world id for different region of interests", 0);
  vul_arg<unsigned> tile_id("-t", "tile id", 100);
  vul_arg_parse(argc, argv);

  // check input
  if (osm_file().compare("") == 0 || osm_out().compare("") == 0 || world_id() == 0 || tile_id() == 100) {
    vcl_cerr << " ERROR: input is missing!\n";
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  // error log
  vcl_stringstream log;
  vcl_stringstream err_log_file;
  err_log_file << osm_out() << "/log_wr" << world_id() << "_tile_" << tile_id() << ".xml";
    
  // check the osm_file
  if (!vul_file::exists(osm_file())) {
    log << "ERROR: can not find osm xml file: " << osm_file() << '\n';
    error_report(err_log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  // check the osm_to_volm file
  vcl_string osm_to_volm_txt = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bbas/volm/osm_to_volm_labels.txt";
  if (!vul_file::exists(osm_to_volm_txt)) {
    log << "ERROR: can not find osm_to_volm txt file: " << osm_to_volm_txt << '\n';
    error_report(err_log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // create volm_osm_binary and associate kml files
  vcl_cout << " =========== Start to create volm_osm date for world " << world_id() << " on tile " << tile_id()
            << " ===============" << vcl_endl;
  volm_osm_objects objs(osm_file(), osm_to_volm_txt);
  vcl_cout << " \t number of location points parsed from osm: " << objs.num_locs() << vcl_endl;
  vcl_cout << " \t number of line roads parsed from osm: " << objs.num_roads() << vcl_endl;
  vcl_cout << " \t number of regions parsed from osm: " << objs.num_regions() << vcl_endl;
  // write to binary
  vcl_stringstream osm_bin;
  osm_bin << osm_out() << "/p1b_wr" << world_id() << "_tile_" << tile_id() << "_osm.bin";
  objs.write_osm_objects(osm_bin.str());
  // write osm to kml
  if (is_create_osm_kml()) {
    vcl_cout << " =========== Parse the osm xml to kml file " << world_id() << " on tile " << tile_id()
              << " ===============" << vcl_endl;
    vcl_stringstream kml_pts, kml_roads, kml_regions;
    kml_pts << osm_out() << "/p1b_wr" << world_id() << "_tile_" << tile_id() << "_osm_pts.kml";
    kml_roads << osm_out() << "/p1b_wr" << world_id() << "_tile_" << tile_id() << "_osm_roads.kml";
    kml_regions << osm_out() << "/p1b_wr" << world_id() << "_tile_" << tile_id() << "_osm_regions.kml";
    objs.write_pts_to_kml(kml_pts.str());
    objs.write_lines_to_kml(kml_roads.str());
    objs.write_polys_to_kml(kml_regions.str());
  }
  vcl_cout << " ========================================== Finish ===============================================" << vcl_endl;
  vcl_cout << " \t results are in : " << osm_out() << vcl_endl;
  vcl_cout << " ========================================== Finish ===============================================" << vcl_endl;
  return volm_io::SUCCESS;
}