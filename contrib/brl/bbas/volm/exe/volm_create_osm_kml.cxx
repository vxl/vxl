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

#include <algorithm>
#include <bkml/bkml_write.h>
#include <iostream>
#include <utility>
#include <vcl_where_root_dir.h>
#include <volm/volm_io.h>
#include <volm/volm_osm_parser.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void error(std::string error_file, const std::string& error_msg)
{
  std::cerr << error_msg;
  volm_io::write_post_processing_log(std::move(error_file), error_msg);
}

static void write_pts_to_kml(std::ofstream& ofs, std::string const& key, std::string const& value,
                             std::vector<vgl_point_2d<double> > const& osm_pts,
                             std::vector<std::vector<std::pair<std::string, std::string> > > const& osm_pt_keys);

static void write_lines_to_kml(std::ofstream& ofs, std::string const& key, std::string const& value,
                               std::vector<std::vector<vgl_point_2d<double> > > osm_lines,
                               std::vector<std::vector<std::pair<std::string, std::string> > > osm_line_keys,
                               unsigned char const& r, unsigned char const& g, unsigned char const& b);

static void write_polys_to_kml(std::ofstream& ofs, std::string const& key, std::string const& value,
                               std::vector<vgl_polygon<double> > osm_polys,
                               std::vector<std::vector<std::pair<std::string, std::string> > > osm_poly_keys,
                               unsigned char const& r, unsigned char const& g, unsigned char const& b);

static bool find_key_value_from_tags(std::string const& key, std::string const& value,
                                     std::vector<std::pair<std::string, std::string> >& keys,
                                     std::string& name);

static void generate_description(std::vector<std::pair<std::string, std::string> >& keys, std::string& description);

int main(int argc, char** argv)
{
  //input
  vul_arg<std::string> osm_file("-osm", "open street map xml file", "");                                                               // open street map xml file
  vul_arg<std::string> out_file("-out", "output file name where kml file will reside", "");                                                                     // output kml file
  vul_arg<std::string> key("-key", "the key of desired osm category, write \"all\" (default) to parse all categories","all");
  vul_arg<std::string> value("-value", "the value of desired osm category, write \"all\" (default) to parse all categories","all");
  vul_arg<std::string> geometry("-geo","the geometry of desired osm category, can be point/line/poly, write \"all\" (default) to parse all geometries","all");
  vul_arg<unsigned> rgb_r("-r", "rgb color", 255);
  vul_arg<unsigned> rgb_g("-g", "rgb color", 255);
  vul_arg<unsigned> rgb_b("-b", "rgb color", 255);
  /*vul_arg<unsigned> world_id("-world","world id (for phase1b)",999);
  vul_arg<unsigned> tile_id("-tile", "tile id", 999);*/
  vul_arg_parse(argc, argv);

  // check input
  if (osm_file().compare("") == 0 || out_file().compare("") == 0) {
    std::cerr << " ERROR: input is missing!\n";
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  // error log
  std::stringstream log;
  std::string log_file;
  log_file = vul_file::strip_extension(out_file());

  // parse the osm files
  if (!vul_file::exists(osm_file())) {
    log << "error: can not find open street map xml file: " << osm_file() << "!\n";
    error(log_file, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // load osm_to_volm table
  std::string osm_to_volm_txt = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bbas/volm/osm_to_volm_labels.txt";
  if (!vul_file::exists(osm_to_volm_txt)) {
    log << "ERROR: can not find osm_to_volm txt file: " << osm_to_volm_txt << '\n';
    error(log_file, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  std::map<std::pair<std::string, std::string>, volm_land_layer> osm_land_table;
  volm_osm_category_io::load_category_table(osm_to_volm_txt, osm_land_table);

  // create kml file
  std::ofstream ofs(out_file().c_str());
  bkml_write::open_document(ofs);
  // write the bounding box
  double lon_min, lat_min, lon_max, lat_max;
  vgl_box_2d<double> bbox = volm_osm_parser::parse_bbox(osm_file());
  lon_min = bbox.min_x();  lat_min = bbox.min_y();
  lon_max = bbox.max_x();  lat_max = bbox.max_y();
  std::vector<vgl_point_2d<double> > outlines;
  outlines.emplace_back(lon_min, lat_min);
  outlines.emplace_back(lon_max, lat_min);
  outlines.emplace_back(lon_max, lat_max);
  outlines.emplace_back(lon_min, lat_max);
  outlines.emplace_back(lon_min, lat_min);
  bkml_write::write_path(ofs, outlines, "outline", "outline", 1.0, 3.0, 1.0, 255, 255, 255);

  std::vector<vgl_point_2d<double> > osm_pts;
  std::vector<std::vector<std::pair<std::string, std::string> > > osm_pt_keys;
  std::vector<std::vector<vgl_point_2d<double> > > osm_lines;
  std::vector<std::vector<std::pair<std::string, std::string> > > osm_line_keys;
  std::vector<vgl_polygon<double> > osm_polys;
  std::vector<std::vector<std::pair<std::string, std::string> > > osm_poly_keys;

  std::cout << "Input osm file:  " << osm_file() << std::endl;
  std::cout << "Output kml file: " << out_file() << std::endl;
  std::cout << "START..." << std::endl;
  if (geometry().compare("point") == 0)
  {
    std::cout << " parsing all points having key \"" << key() << "\" and value \"" << value() << "\" in osm to kml..." << std::endl;
    volm_osm_parser::parse_points(osm_pts, osm_pt_keys, osm_file());
    write_pts_to_kml(ofs, key(), value(), osm_pts, osm_pt_keys);
  }
  else if (geometry().compare("lines") == 0)
  {
    std::cout << " parsing all roads having key \"" << key() << "\" and value \"" << value() << "\" in osm to kml..." << std::endl;
    volm_osm_parser::parse_lines(osm_lines, osm_line_keys, osm_file());
    write_lines_to_kml(ofs, key(), value(), osm_lines, osm_line_keys, (unsigned char)rgb_r(), (unsigned char)rgb_g(), (unsigned char)rgb_b());
  }
  else if (geometry().compare("poly") == 0)
  {
    std::cout << " parsing all regions having key \"" << key() << "\" and value \"" << value() << "\" in osm to kml..." << std::endl;
    volm_osm_parser::parse_polygons(osm_polys, osm_poly_keys, osm_file());
    write_polys_to_kml(ofs, key(), value(), osm_polys, osm_poly_keys, (unsigned char)rgb_r(), (unsigned char)rgb_g(), (unsigned char)rgb_b());
  }
  else {
    std::cout << " parsing location points having key \"" << key() << "\" and value \"" << value() << "\" in osm to kml..." << std::endl;
    volm_osm_parser::parse_points(osm_pts, osm_pt_keys, osm_file());
    std::cout << osm_pts.size() << " points are parsed from osm file" << std::flush << std::endl;
    std::cout << " parsing lines having key \"" << key() << "\" and value \"" << value() << "\" in osm to kml..." << std::endl;
    volm_osm_parser::parse_lines(osm_lines, osm_line_keys, osm_file());
    std::cout << osm_lines.size() << " lines are parsed from osm file" << std::flush << std::endl;
    std::cout << " parsing regions having key \"" << key() << "\" and value \"" << value() << "\" in osm to kml..." << std::endl;
    volm_osm_parser::parse_polygons(osm_polys, osm_poly_keys, osm_file());
    std::cout << osm_polys.size() << " regions are parsed from osm file" << std::flush << std::endl;
    write_pts_to_kml(ofs, key(), value(), osm_pts, osm_pt_keys);
    write_lines_to_kml(ofs, key(), value(), osm_lines, osm_line_keys, (unsigned char)rgb_r(), (unsigned char)rgb_g(), (unsigned char)rgb_b());
    write_polys_to_kml(ofs, key(), value(), osm_polys, osm_poly_keys, (unsigned char)rgb_r(), (unsigned char)rgb_g(), (unsigned char)rgb_b());
  }
  bkml_write::close_document(ofs);
  ofs.close();
  std::cout << "FINISH!" << std::endl;
  return volm_io::SUCCESS;
}


void write_pts_to_kml(std::ofstream& ofs, std::string const& key, std::string const& value,
                      std::vector<vgl_point_2d<double> > const& osm_pts,
                      std::vector<std::vector<std::pair<std::string, std::string> > > const& osm_pt_keys)
{

  std::map<std::pair<std::string, std::string>, volm_land_layer>::const_iterator mit;
  auto num_pts = (unsigned)osm_pts.size();
  for (unsigned i = 0; i < num_pts; i++) {
    std::vector<std::pair<std::string, std::string> > curr_keys = osm_pt_keys[i];
    std::string name, description;
    bool found = find_key_value_from_tags(key, value, curr_keys, name);
    if (found) {
      // generate description which contains all tags for that locations
      generate_description(curr_keys, description);
      // replace unsupported character in name
      std::replace(name.begin(), name.end(), '&', '_');
      std::replace(name.begin(), name.end(), '<', '_');
      std::replace(name.begin(), name.end(), '>', '_');
      // put location into kml
      bkml_write::write_location(ofs, osm_pts[i], name, description, 0.6);
    }
  }

#if 0
    bool found = false;
    for (std::vector<std::pair<std::string, std::string> >::iterator vit = curr_keys.begin(); (vit != curr_keys.end() && !found); ++vit) {
      mit = osm_land_table.find(*vit);
      if (mit != osm_land_table.end()) {
        // put the points into kml file (only parse the points have key and value defined in volm_to_osm_txt)
        found = true;
        std::string name = mit->first.first;  std::string description = mit->first.second;
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
#endif

}

void write_lines_to_kml(std::ofstream& ofs, std::string const& key, std::string const& value,
                        std::vector<std::vector<vgl_point_2d<double> > > osm_lines,
                        std::vector<std::vector<std::pair<std::string, std::string> > > osm_line_keys,
                        unsigned char const& r, unsigned char const& g, unsigned char const& b)
{
  std::map<std::pair<std::string, std::string>, volm_land_layer>::const_iterator mit;
  auto num_lines = (unsigned)osm_lines.size();
  for (unsigned i = 0; i < num_lines; i++) {
    std::vector<std::pair<std::string, std::string> > curr_keys = osm_line_keys[i];
    std::string name, description;
    bool found = find_key_value_from_tags(key, value, curr_keys, name);
    if (found) {
      generate_description(curr_keys, description);
      // replace unsupported character in name
      std::replace(name.begin(), name.end(), '&', '_');
      std::replace(name.begin(), name.end(), '<', '_');
      std::replace(name.begin(), name.end(), '>', '_');
      bkml_write::write_path(ofs, osm_lines[i], name, description, 1.0, 2.0, 1.0, r, g, b);
    }
  }
#if 0
  for (unsigned i = 0; i < num_lines; i++) {
    std::vector<std::pair<std::string, std::string> > curr_keys = osm_line_keys[i];
    bool found = false;
    for (std::vector<std::pair<std::string, std::string> >::iterator vit = curr_keys.begin(); (vit != curr_keys.end() && !found); ++vit) {
      mit = osm_land_table.find(*vit);
      if (mit != osm_land_table.end()) {
        found = true;
        std::string name = mit->first.first;  std::string description = mit->first.second;
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
#endif
}

void write_polys_to_kml(std::ofstream& ofs, std::string const& key, std::string const& value,
                        std::vector<vgl_polygon<double> > osm_polys,
                        std::vector<std::vector<std::pair<std::string, std::string> > > osm_poly_keys,
                        unsigned char const& r, unsigned char const& g, unsigned char const& b)
{
  std::map<std::pair<std::string, std::string>, volm_land_layer>::const_iterator mit;
  auto num_polys = (unsigned)osm_polys.size();
  for (unsigned i = 0; i < num_polys; i++) {
    std::vector<std::pair<std::string, std::string> > curr_keys = osm_poly_keys[i];
    std::string name, description;
    bool found = find_key_value_from_tags(key, value, curr_keys, name);
    if (found) {
      generate_description(curr_keys, description);
      // replace unsupported character in name
      std::replace(name.begin(), name.end(), '&', '_');
      std::replace(name.begin(), name.end(), '<', '_');
      std::replace(name.begin(), name.end(), '>', '_');
      bkml_write::write_polygon(ofs, osm_polys[i], name, description, 1.0, 3.0, 0.45, r, g, b);
    }
  }

#if 0
  for (unsigned i = 0; i < num_polys; i++) {
    std::vector<std::pair<std::string, std::string> > curr_keys = osm_poly_keys[i];
    bool found = false;
    for (std::vector<std::pair<std::string, std::string> >::iterator vit = curr_keys.begin(); (vit != curr_keys.end() && !found); ++vit) {
      mit = osm_land_table.find(*vit);
      if (mit != osm_land_table.end()) {
        found = true;
        std::string name = mit->first.first;  std::string description = mit->first.second;
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
#endif
}

bool find_key_value_from_tags(std::string const& key, std::string const& value,
                              std::vector<std::pair<std::string, std::string> >& tags,
                              std::string& name)
{
  bool found = false;
  if (key == "all" && value == "all") {
    name = "all=all";
    return true;
  }
  for (auto vit = tags.begin(); ( vit != tags.end() && !found); ++vit)
  {
    if (key.compare("all") == 0 && value.compare(vit->second) == 0)
    {  found = true;  name = vit->first + "=" + vit->second; }
    else if (key.compare(vit->first) == 0 && value.compare("all") == 0)
    {  found = true;  name = vit->first + "=" + vit->second; }
    else if (key.compare(vit->first) == 0 && value.compare(vit->second) == 0)
    {  found = true;  name = vit->first + "=" + vit->second; }
  }
  return found;
}

void generate_description(std::vector<std::pair<std::string, std::string> >& tags, std::string& description)
{
  description.clear();
  for (auto & tag : tags)
  {
    description += tag.first + "=" + tag.second + " ";
  }
  // replace all unsupported characters in kml to "_"
  std::replace(description.begin(), description.end(), '&', '_');
  std::replace(description.begin(), description.end(), '<', '_');
  std::replace(description.begin(), description.end(), '>', '_');
}
