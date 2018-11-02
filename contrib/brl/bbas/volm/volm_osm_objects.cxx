#include <iostream>
#include <algorithm>
#include "volm_osm_objects.h"
//:
// \file
#include <vul/vul_file.h>
#include <bkml/bkml_write.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// create volm_osm_objects from open street map xml file
volm_osm_objects::volm_osm_objects(std::string const& osm_file, std::string const& osm_to_volm_file)
{
  // load osm_to_volm table
  std::map<std::pair<std::string, std::string>, volm_land_layer> osm_land_table;
  volm_osm_category_io::load_category_table(osm_to_volm_file, osm_land_table);

  // load all open street map objects from
  std::vector<vgl_point_2d<double> > osm_pts;
  std::vector<std::vector<std::pair<std::string, std::string> > > osm_pt_keys;
  volm_osm_parser::parse_points(osm_pts, osm_pt_keys, osm_file);

  std::vector<std::vector<vgl_point_2d<double> > > osm_lines;
  std::vector<std::vector<std::pair<std::string, std::string> > > osm_line_keys;
  volm_osm_parser::parse_lines(osm_lines, osm_line_keys, osm_file);

  std::vector<vgl_polygon<double> > osm_polys;
  std::vector<std::vector<std::pair<std::string, std::string> > > osm_poly_keys;
  volm_osm_parser::parse_polygons(osm_polys, osm_poly_keys, osm_file);

  // transfer osm objects to volm_osm_objects (ignore the osm objects whose properties is not defined in osm_to_volm table)
  std::map<std::pair<std::string, std::string>, volm_land_layer>::iterator mit;
  auto num_pts = (unsigned)osm_pts.size();
  for (unsigned i = 0; i < num_pts; i++)
  {
    std::vector<std::pair<std::string, std::string> > curr_keys = osm_pt_keys[i];
    // find pier first as first priority
    bool pier_found = false;
    for (auto vit = curr_keys.begin(); (vit != curr_keys.end() && !pier_found); ++vit) {
      if (vit->first == "man_made" && vit->second == "pier") {
        mit = osm_land_table.find(*vit);
        if (mit != osm_land_table.end()) {
          pier_found = true;  volm_land_layer prop = mit->second;  volm_osm_object_point_sptr loc_pt = new volm_osm_object_point(prop, osm_pts[i]);
          loc_pts_.push_back(loc_pt);
        }
      }
    }
    if (pier_found)
      continue;
    bool found = false;
    for (auto vit = curr_keys.begin(); (vit != curr_keys.end() && !found); ++vit) {
      mit = osm_land_table.find(*vit);
      if (mit != osm_land_table.end()) { // the key is in the table
        found = true;
        volm_land_layer prop = mit->second;
        volm_osm_object_point_sptr loc_pt = new volm_osm_object_point(prop, osm_pts[i]);
        loc_pts_.push_back(loc_pt);
      }
    }
  }

  auto num_lines = (unsigned)osm_lines.size();
  for (unsigned i = 0; i < num_lines; i++)
  {
    std::vector<std::pair<std::string, std::string> > curr_keys = osm_line_keys[i];
    // find pier first as first priority
    bool pier_found = false;
    for (auto vit = curr_keys.begin(); (vit != curr_keys.end() && !pier_found); ++vit) {
      if (vit->first == "man_made" && vit->second == "pier") {
        mit = osm_land_table.find(*vit);
        if (mit != osm_land_table.end()) {
          pier_found = true;  volm_land_layer prop = mit->second;  volm_osm_object_line_sptr loc_line = new volm_osm_object_line(mit->second, osm_lines[i]);
          loc_lines_.push_back(loc_line);
        }
      }
    }
    if (pier_found)
      continue;
    bool found = false;
    for (auto vit = curr_keys.begin(); (vit != curr_keys.end() && !found); ++vit) {
      mit = osm_land_table.find(*vit);
      if (mit != osm_land_table.end()) {
        found = true;
        volm_osm_object_line_sptr loc_line = new volm_osm_object_line(mit->second, osm_lines[i]);
        loc_lines_.push_back(loc_line);
      }
    }
  }

  auto num_polys = (unsigned)osm_polys.size();
  for (unsigned i = 0; i < num_polys; i++)
  {
    std::vector<std::pair<std::string, std::string> > curr_keys = osm_poly_keys[i];
    // find pier first
    bool pier_found = false;
    for (auto vit = curr_keys.begin(); (vit != curr_keys.end() && !pier_found); ++vit) {
      if (vit->first == "man_made" && vit->second == "pier") {
        mit = osm_land_table.find(*vit);
        if (mit != osm_land_table.end()) {
          pier_found = true;  volm_osm_object_polygon_sptr loc_poly = new volm_osm_object_polygon(mit->second, osm_polys[i]);
          loc_polys_.push_back(loc_poly);
        }
      }
    }

    bool found = false;
    for (auto vit = curr_keys.begin(); (vit != curr_keys.end() && !found); ++vit) {
      mit = osm_land_table.find(*vit);
      if (mit != osm_land_table.end()) {
        found = true;
        volm_osm_object_polygon_sptr loc_poly = new volm_osm_object_polygon(mit->second, osm_polys[i]);
        loc_polys_.push_back(loc_poly);
      }
    }
  }

}

volm_osm_objects::volm_osm_objects(std::string const& bin_file)
{
  vsl_b_ifstream is(bin_file.c_str());
  if (!is) {
    std::cerr << "In volm_osm_object::volm_osm_object() -- cannot open: " << bin_file << std::endl;
    return;
  }
  this->b_read(is);
  is.close();
}

bool volm_osm_objects::write_osm_objects(std::string const& bin_file)
{
  vsl_b_ofstream os(bin_file.c_str());
  if (!os) {
    return false;
  }
  this->b_write(os);
  os.close();
  return true;
}


void volm_osm_objects::b_write(vsl_b_ostream& os)
{
  vsl_b_write(os, version());
  vsl_b_write(os, loc_pts_.size());
  for (const auto & loc_pt : loc_pts_)
    vsl_b_write(os, loc_pt);
  vsl_b_write(os, loc_lines_.size());
  for (const auto & loc_line : loc_lines_)
    vsl_b_write(os, loc_line);
  vsl_b_write(os, loc_polys_.size());
  for (const auto & loc_poly : loc_polys_)
    vsl_b_write(os, loc_poly);
}

void volm_osm_objects::b_read(vsl_b_istream& is)
{
  if (!is) return;
  short ver;
  vsl_b_read(is,ver);
  if (ver == 1) {
    unsigned loc_pts_size;
    vsl_b_read(is, loc_pts_size);
    for (unsigned i = 0; i < loc_pts_size; i++) {
      volm_osm_object_point_sptr loc_pt;
      vsl_b_read(is, loc_pt);
      loc_pts_.push_back(loc_pt);
    }
    unsigned loc_lines_size;
    vsl_b_read(is, loc_lines_size);
    for (unsigned i = 0; i < loc_lines_size; i++) {
      volm_osm_object_line_sptr loc_line;
      vsl_b_read(is, loc_line);
      loc_lines_.push_back(loc_line);
    }
    unsigned loc_polys_size;
    vsl_b_read(is, loc_polys_size);
    for (unsigned i = 0; i < loc_polys_size; i++) {
      volm_osm_object_polygon_sptr loc_poly;
      vsl_b_read(is, loc_poly);
      loc_polys_.push_back(loc_poly);
    }
  }
  else {
    std::cout << "volm_osm_objects -- unknown binary io version " << ver << '\n';
    return;
  }
}

bool volm_osm_objects::write_pts_to_kml(std::string const& kml_file)
{
  std::ofstream ofs(kml_file.c_str());
  bkml_write::open_document(ofs);

  // write the points into kml
  unsigned cnt = 0;
  for (auto & loc_pt : loc_pts_)
  {
    std::string name = loc_pt->prop().name_;
    std::stringstream str_w;  str_w << "id=" << cnt++;
    bkml_write::write_location(ofs, loc_pt->loc(), name, str_w.str(), 0.6);
  }
  bkml_write::close_document(ofs);
  ofs.close();
  return true;
}

bool volm_osm_objects::write_lines_to_kml(std::string const& kml_file)
{
  std::ofstream ofs(kml_file.c_str());
  bkml_write::open_document(ofs);

  // write the roads into kml
  unsigned cnt = 0;
  for (auto & loc_line : loc_lines_)
  {
    double width = loc_line->prop().width_;
    std::string name = loc_line->prop().name_;
    std::stringstream str_w;
    str_w << "width=" << width << ", id=" << cnt++;
    bkml_write::write_path(ofs, loc_line->line(), name, str_w.str(), 1.0, width);
  }
  bkml_write::close_document(ofs);
  ofs.close();
  return true;
}

bool volm_osm_objects::write_polys_to_kml(std::string const& kml_file)
{
  std::ofstream ofs(kml_file.c_str());
  bkml_write::open_document(ofs);

  // write the regions into kml
  unsigned cnt = 0;
  for (auto & loc_poly : loc_polys_)
  {
    unsigned char level = loc_poly->prop().level_;
    std::string name = loc_poly->prop().name_;
    std::stringstream str_l;
    str_l << "level=" << (int)level << ", id=" << cnt++;
    unsigned char r,g,b;
    if (level == 0)      { r = 85;   g = 255;  b = 255; }
    else if (level == 1) { r = 255;  g = 170;  b = 127; }
    else if (level == 2) { r = 85,   g = 255;  b = 0;   }
    else if (level == 3) { r = 170,  g = 170;  b = 255; }
    else if (level == 4) { r = 0;    g = 255;  b = 127; }
    else                 { r = 170;  g = 170;  b = 170; }
    bkml_write::write_polygon(ofs, loc_poly->poly(), name, str_l.str(), 1.0, 3.0, 0.4, r, g, b);
  }
  bkml_write::close_document(ofs);
  ofs.close();
  return true;
}

volm_osm_object_ids::volm_osm_object_ids(std::string const& bin_file)
{
  vsl_b_ifstream is(bin_file.c_str());
  if (!is) {
    std::cerr << "In volm_osm_object_ids::volm_osm_object_ids() -- cannot open file: " << bin_file << std::endl;
    return;
  }
  this->b_read(is);
  is.close();
}

void volm_osm_object_ids::add_pt(unsigned const& pt_id)
{
  auto vit = std::find(pt_ids_.begin(), pt_ids_.end(), pt_id);
  if (vit == pt_ids_.end())
    pt_ids_.push_back(pt_id);
}

void volm_osm_object_ids::add_line(unsigned const& line_id)
{
  auto vit = std::find(line_ids_.begin(), line_ids_.end(), line_id);
  if (vit == line_ids_.end())
    line_ids_.push_back(line_id);
}

void volm_osm_object_ids::add_region(unsigned const& region_id)
{
  auto vit = std::find(region_ids_.begin(), region_ids_.end(), region_id);
  if (vit == region_ids_.end())
    region_ids_.push_back(region_id);
}

//: write self to binary
bool volm_osm_object_ids::write_osm_ids(std::string const& bin_file)
{
  vsl_b_ofstream os(bin_file.c_str());
  if (!os)
    return false;
  this->b_write(os);
  os.close();
  return true;
}

//: binary save self to stream
void volm_osm_object_ids::b_write(vsl_b_ostream& os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, pt_ids_.size());
  for (unsigned int pt_id : pt_ids_)
    vsl_b_write(os, pt_id);
  vsl_b_write(os, line_ids_.size());
  for (unsigned int line_id : line_ids_)
    vsl_b_write(os, line_id);
  vsl_b_write(os, region_ids_.size());
  for (unsigned int region_id : region_ids_)
    vsl_b_write(os, region_id);
}

//: binary load self from stream
void volm_osm_object_ids::b_read(vsl_b_istream& is)
{
  if (!is) return;
  short ver;
  vsl_b_read(is, ver);
  if (ver == 1) {
    unsigned num_pts;
    vsl_b_read(is, num_pts);
    for (unsigned i = 0; i < num_pts; i++) {
      unsigned id;
      vsl_b_read(is, id);  pt_ids_.push_back(id);
    }
    unsigned num_lines;
    vsl_b_read(is, num_lines);
    for (unsigned i = 0; i < num_lines; i++) {
      unsigned id;
      vsl_b_read(is, id);  line_ids_.push_back(id);
    }
    unsigned num_regions;
    vsl_b_read(is, num_regions);
    for (unsigned i = 0; i < num_regions; i++) {
      unsigned id;
      vsl_b_read(is, id);  region_ids_.push_back(id);
    }
  }
  else {
    std::cout << "volm_osm_object_ids -- unknown binary io version " << ver << '\n';
    return;
  }
}
