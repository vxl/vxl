//:
// \file
// \brief  executable to create kml files for certain category open street map xml.
//         Note the categories being parsed is defined in file /contrib/brl/bbas/volm/osm_to_volm_labels.txt
//
// \author Yi Dong
// \date December, 2013
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <vul/vul_file.h>
#include <vul/vul_arg.h>
#include <vcl_where_root_dir.h>
#include <bkml/bkml_write.h>
#include <volm/volm_osm_parser.h>
#include <volm/volm_io.h>

static void error(vcl_string error_file, vcl_string error_msg)
{
  vcl_cerr << error_msg;
  volm_io::write_post_processing_log(error_file, error_msg);
}

static void write_pts_to_kml(vcl_ofstream& ofs, vcl_string const& key, vcl_string const& value,
                             vcl_map<vcl_pair<vcl_string, vcl_string>, volm_land_layer> const& osm_land_table,
                             vcl_vector<vgl_point_2d<double> > const& osm_pts,
                             vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > > const& osm_pt_keys);

static void write_lines_to_kml(vcl_ofstream& ofs, vcl_string const& key, vcl_string const& value,
                               vcl_map<vcl_pair<vcl_string, vcl_string>, volm_land_layer> const& osm_land_table,
                               vcl_vector<vcl_vector<vgl_point_2d<double> > > osm_lines,
                               vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > > osm_line_keys);

static void write_polys_to_kml(vcl_ofstream& ofs, vcl_string const& key, vcl_string const& value,
                               vcl_map<vcl_pair<vcl_string, vcl_string>, volm_land_layer> const& osm_land_table,
                               vcl_vector<vgl_polygon<double> > osm_polys,
                               vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > > osm_poly_keys);

int main(int argc, char** argv)
{
  //input
  vul_arg<vcl_string> osm_file("-osm", "open street map xml file", "");                                                               // open street map xml file
  vul_arg<vcl_string> out("-out", "output folder where kml file will reside", "");                                                                     // output kml file
  vul_arg<vcl_string> key("-key", "the key of desired osm category, write \"all\" (default) to parse all categories","all");
  vul_arg<vcl_string> value("-value", "the value of desired osm category, write \"all\" (default) to parse all categories","all");
  vul_arg<vcl_string> geometry("-geo","the geometry of desired osm category, can be point/line/poly, write \"all\" (default) to parse all geometries","all");
  vul_arg<unsigned> world_id("-world","world id (for phase1b)",999);
  vul_arg<unsigned> tile_id("-tile", "tile id", 999);
  vul_arg_parse(argc, argv);

  // check input
  if (osm_file().compare("") == 0 || out().compare("") == 0) {
    vcl_cerr << " ERROR: input is missing!\n";
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  // error log
  vcl_stringstream log;
  vcl_stringstream log_file;
  log_file << out() << "/log_osm_key_" << key() << "_value_" << value() << ".xml";

  // parse the osm files
  if (!vul_file::exists(osm_file())) {
    log << "error: can not find open street map xml file: " << osm_file() << "!\n";
    error(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // load osm_to_volm table
  vcl_string osm_to_volm_txt = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bbas/volm/osm_to_volm_labels.txt";
  if (!vul_file::exists(osm_to_volm_txt)) {
    log << "ERROR: can not find osm_to_volm txt file: " << osm_to_volm_txt << '\n';
    error(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vcl_map<vcl_pair<vcl_string, vcl_string>, volm_land_layer> osm_land_table;
  volm_osm_category_io::load_category_table(osm_to_volm_txt, osm_land_table);

  // create kml file
  vcl_stringstream kml_name;
  if (world_id() != 999 && tile_id() != 999)
    kml_name << out() << "/osm_key_" << key() << "_value_" << value() << "_wr" << world_id() << "_tile_" << tile_id() << ".kml";
  else
    kml_name << out() << "/osm_key_" << key() << "_value_" << value() << "_geo_" << geometry() << ".kml";
  vcl_ofstream ofs(kml_name.str().c_str());
  bkml_write::open_document(ofs);
  // write the bounding box
  vcl_stringstream str_box;
  double lon_min, lat_min, lon_max, lat_max;
  vgl_box_2d<double> bbox = volm_osm_parser::parse_bbox(osm_file());
  lon_min = bbox.min_x();  lat_min = bbox.min_y();
  lon_max = bbox.max_x();  lat_max = bbox.max_y();
  str_box << vcl_setprecision(6) << lat_min << 'x' << vcl_setprecision(6) << lon_min << "to"
          << vcl_setprecision(6) << lat_max << 'x' << vcl_setprecision(6) << lon_max;
  bkml_write::write_box(ofs, "region", "", bbox);

  vcl_vector<vgl_point_2d<double> > osm_pts;
  vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > > osm_pt_keys;
  vcl_vector<vcl_vector<vgl_point_2d<double> > > osm_lines;
  vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > > osm_line_keys;
  vcl_vector<vgl_polygon<double> > osm_polys;
  vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > > osm_poly_keys;

  vcl_cout << "Input osm file:  " << osm_file() << vcl_endl;
  vcl_cout << "Output kml file: " << kml_name.str() << vcl_endl;
  vcl_cout << "START..." << vcl_endl;
  if (geometry().compare("point") == 0)
  {
    vcl_cout << " parser all points having key \"" << key() << "\" and value \"" << value() << "\" in osm to kml..." << vcl_endl;
    volm_osm_parser::parse_points(osm_pts, osm_pt_keys, osm_file());
    write_pts_to_kml(ofs, key(), value(), osm_land_table, osm_pts, osm_pt_keys);
  }
  else if (geometry().compare("lines") == 0)
  {
    vcl_cout << " parser all roads having key \"" << key() << "\" and value \"" << value() << "\" in osm to kml..." << vcl_endl;
    volm_osm_parser::parse_lines(osm_lines, osm_line_keys, osm_file());
    write_lines_to_kml(ofs, key(), value(), osm_land_table, osm_lines, osm_line_keys);
  }
  else if (geometry().compare("poly") == 0)
  {
    vcl_cout << " parser all regions having key \"" << key() << "\" and value \"" << value() << "\" in osm to kml..." << vcl_endl;
    volm_osm_parser::parse_polygons(osm_polys, osm_poly_keys, osm_file());
    write_polys_to_kml(ofs, key(), value(), osm_land_table, osm_polys, osm_poly_keys);
  }
  else {
    vcl_cout << " parser all possible geometries having key \"" << key() << "\" and value \"" << value() << "\" in osm to kml..." << vcl_endl;
    volm_osm_parser::parse_points(osm_pts, osm_pt_keys, osm_file());
    volm_osm_parser::parse_lines(osm_lines, osm_line_keys, osm_file());
    volm_osm_parser::parse_polygons(osm_polys, osm_poly_keys, osm_file());
    write_pts_to_kml(ofs, key(), value(), osm_land_table, osm_pts, osm_pt_keys);
    write_lines_to_kml(ofs, key(), value(), osm_land_table, osm_lines, osm_line_keys);
    write_polys_to_kml(ofs, key(), value(), osm_land_table, osm_polys, osm_poly_keys);
  }
  bkml_write::close_document(ofs);
  ofs.close();
  vcl_cout << "FINISH!" << vcl_endl;
  return volm_io::SUCCESS;
}


void write_pts_to_kml(vcl_ofstream& ofs, vcl_string const& key, vcl_string const& value,
                      vcl_map<vcl_pair<vcl_string, vcl_string>, volm_land_layer> const& osm_land_table,
                      vcl_vector<vgl_point_2d<double> > const& osm_pts,
                      vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > > const& osm_pt_keys)
{
  vcl_map<vcl_pair<vcl_string, vcl_string>, volm_land_layer>::const_iterator mit;
  unsigned num_pts = (unsigned)osm_pts.size();
  for (unsigned i = 0; i < num_pts; i++) {
    vcl_vector<vcl_pair<vcl_string, vcl_string> > curr_keys = osm_pt_keys[i];
    bool found = false;
    for (vcl_vector<vcl_pair<vcl_string, vcl_string> >::iterator vit = curr_keys.begin(); (vit != curr_keys.end() && !found); ++vit) {
      mit = osm_land_table.find(*vit);
      if (mit != osm_land_table.end()) {
        // put the points into kml file (only parse the points have key and value defined in volm_to_osm_txt)
        found = true;
        vcl_string name = mit->first.first;  vcl_string description = mit->first.second;
        if (key == "all" && value == "all")
          bkml_write::write_location(ofs, osm_pts[i], description, name, 0.6);  // put all points
        else if (name.compare(key) == 0 && value == "all")
          bkml_write::write_location(ofs, osm_pts[i], description, name, 0.6);  // put all points having given value
        else if (key == "all" && description.compare(value) == 0)
          bkml_write::write_location(ofs, osm_pts[i], description, name, 0.6);  // put all points having given key
        else if (name.compare(key)== 0 && description.compare(value) == 0)
          bkml_write::write_location(ofs, osm_pts[i], description, name, 0.6);  // put points having given key and value
        else
          continue;
      }
    }
  }
}

void write_lines_to_kml(vcl_ofstream& ofs, vcl_string const& key, vcl_string const& value,
                        vcl_map<vcl_pair<vcl_string, vcl_string>, volm_land_layer> const& osm_land_table,
                        vcl_vector<vcl_vector<vgl_point_2d<double> > > osm_lines,
                        vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > > osm_line_keys)
{
  vcl_map<vcl_pair<vcl_string, vcl_string>, volm_land_layer>::const_iterator mit;
  unsigned num_lines = (unsigned)osm_lines.size();
  for (unsigned i = 0; i < num_lines; i++) {
    vcl_vector<vcl_pair<vcl_string, vcl_string> > curr_keys = osm_line_keys[i];
    bool found = false;
    for (vcl_vector<vcl_pair<vcl_string, vcl_string> >::iterator vit = curr_keys.begin(); (vit != curr_keys.end() && !found); ++vit) {
      mit = osm_land_table.find(*vit);
      if (mit != osm_land_table.end()) {
        found = true;
        vcl_string name = mit->first.first;  vcl_string description = mit->first.second;
        double width = mit->second.width_;
        if (key == "all" && value == "all")
          bkml_write::write_path(ofs, osm_lines[i], description, name, 1.0, width);  // put all lines into file
        else if (value == "all" && name.compare(key) == 0)
          bkml_write::write_path(ofs, osm_lines[i], description, name, 1.0, width);  // put all lines having given value
        else if (key == "all" && description.compare(value) == 0)
          bkml_write::write_path(ofs, osm_lines[i], description, name, 1.0, width);  // put all lines having given key
        else if (name.compare(key)== 0 && description.compare(value) == 0)
          bkml_write::write_path(ofs, osm_lines[i], description, name, 1.0, width);  // put all lines having given value
        else
          continue;
      }
    }
  }
}

void write_polys_to_kml(vcl_ofstream& ofs, vcl_string const& key, vcl_string const& value,
                        vcl_map<vcl_pair<vcl_string, vcl_string>, volm_land_layer> const& osm_land_table,
                        vcl_vector<vgl_polygon<double> > osm_polys,
                        vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > > osm_poly_keys)
{
  vcl_map<vcl_pair<vcl_string, vcl_string>, volm_land_layer>::const_iterator mit;
  unsigned num_polys = (unsigned)osm_polys.size();
  for (unsigned i = 0; i < num_polys; i++) {
    vcl_vector<vcl_pair<vcl_string, vcl_string> > curr_keys = osm_poly_keys[i];
    bool found = false;
    for (vcl_vector<vcl_pair<vcl_string, vcl_string> >::iterator vit = curr_keys.begin(); (vit != curr_keys.end() && !found); ++vit) {
      mit = osm_land_table.find(*vit);
      if (mit != osm_land_table.end()) {
        found = true;
        vcl_string name = mit->first.first;  vcl_string description = mit->first.second;
        unsigned char level = mit->second.level_;
        unsigned char r,g,b;
        if (level == 0)      { r = 85;   g = 255;  b = 255; }
        else if (level == 1) { r = 255;  g = 170;  b = 127; }
        else if (level == 2) { r = 85,   g = 255;  b = 0;   }
        else if (level == 3) { r = 170,  g = 170;  b = 255; }
        else if (level == 4) { r = 0;    g = 255;  b = 127; }
        else                 { r = 170;  g = 170;  b = 170; }
        if (key.compare("all") == 0 && value.compare("all") == 0)
          bkml_write::write_polygon(ofs, osm_polys[i],description, name, 1.0, 3.0, 0.4, r, g, b);  // put all regions
        else if (name.compare(key) == 0 && value.compare("all") == 0)
          bkml_write::write_polygon(ofs, osm_polys[i],description, name, 1.0, 3.0, 0.4, r, g, b);  // put all regions having given key
        else if (name.compare("all") == 0 && description.compare(value) == 0)
          bkml_write::write_polygon(ofs, osm_polys[i],description, name, 1.0, 3.0, 0.4, r, g, b);  // put all regions having given value
        else if (name.compare(key) == 0 && description.compare(value) == 0)
          bkml_write::write_polygon(ofs, osm_polys[i],description, name, 1.0, 3.0, 0.4, r, g, b);  // put all regions having given key and value
        else
          continue;
      }
    }
  }
}