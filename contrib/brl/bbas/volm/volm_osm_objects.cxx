#include "volm_osm_objects.h"
//:
// \file
#include <vul/vul_file.h>
#include <bkml/bkml_write.h>
#include <vcl_algorithm.h>

// create volm_osm_objects from open street map xml file
volm_osm_objects::volm_osm_objects(vcl_string const& osm_file, vcl_string const& osm_to_volm_file)
{
  // load osm_to_volm table
  vcl_map<vcl_pair<vcl_string, vcl_string>, volm_land_layer> osm_land_table;
  volm_osm_category_io::load_category_table(osm_to_volm_file, osm_land_table);

  // load all open street map objects from 
  vcl_vector<vgl_point_2d<double> > osm_pts;
  vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > > osm_pt_keys;
  volm_osm_parser::parse_points(osm_pts, osm_pt_keys, osm_file);

  vcl_vector<vcl_vector<vgl_point_2d<double> > > osm_lines;
  vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > > osm_line_keys;
  volm_osm_parser::parse_lines(osm_lines, osm_line_keys, osm_file);

  vcl_vector<vgl_polygon<double> > osm_polys;
  vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > > osm_poly_keys;
  volm_osm_parser::parse_polygons(osm_polys, osm_poly_keys, osm_file);

  // transfer osm objects to volm_osm_objects (ignore the osm objects whose properties is not defined in osm_to_volm table)
  vcl_map<vcl_pair<vcl_string, vcl_string>, volm_land_layer>::iterator mit;
  unsigned num_pts = (unsigned)osm_pts.size();
  for (unsigned i = 0; i < num_pts; i++)
  {
    vcl_vector<vcl_pair<vcl_string, vcl_string> > curr_keys = osm_pt_keys[i];
    bool found = false;
    for (vcl_vector<vcl_pair<vcl_string, vcl_string> >::iterator vit = curr_keys.begin(); (vit != curr_keys.end() && !found); ++vit) {
      mit = osm_land_table.find(*vit);
      if (mit != osm_land_table.end()) { // the key is in the table
        found = true;
        volm_land_layer prop = mit->second;
        volm_osm_object_point_sptr loc_pt = new volm_osm_object_point(prop, osm_pts[i]);
        loc_pts_.push_back(loc_pt);
      }
    }
  }

  unsigned num_lines = (unsigned)osm_lines.size();
  for (unsigned i = 0; i < num_lines; i++)
  {
    vcl_vector<vcl_pair<vcl_string, vcl_string> > curr_keys = osm_line_keys[i];
    bool found = false;
    for (vcl_vector<vcl_pair<vcl_string, vcl_string> >::iterator vit = curr_keys.begin(); (vit != curr_keys.end() && !found); ++vit) {
      mit = osm_land_table.find(*vit);
      if (mit != osm_land_table.end()) {
        found = true;
        volm_osm_object_line_sptr loc_line = new volm_osm_object_line(mit->second, osm_lines[i]);
        loc_lines_.push_back(loc_line);
      }
    }
  }

  unsigned num_polys = (unsigned)osm_polys.size();
  for (unsigned i = 0; i < num_polys; i++)
  {
    vcl_vector<vcl_pair<vcl_string, vcl_string> > curr_keys = osm_poly_keys[i];
    bool found = false;
    for (vcl_vector<vcl_pair<vcl_string, vcl_string> >::iterator vit = curr_keys.begin(); (vit != curr_keys.end() && !found); ++vit) {
      mit = osm_land_table.find(*vit);
      if (mit != osm_land_table.end()) {
        found = true;
        volm_osm_object_polygon_sptr loc_poly = new volm_osm_object_polygon(mit->second, osm_polys[i]);
        loc_polys_.push_back(loc_poly);
      }
    }
  }

}

volm_osm_objects::volm_osm_objects(vcl_string const& bin_file)
{
  vsl_b_ifstream is(bin_file.c_str());
  if (!is) {
    vcl_cerr << "In volm_osm_object::volm_osm_object() -- cannot open: " << bin_file << vcl_endl;
    return;
  }
  this->b_read(is);
  is.close();
}

bool volm_osm_objects::write_osm_objects(vcl_string const& bin_file)
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
  for (unsigned i = 0; i < loc_pts_.size(); i++)
    vsl_b_write(os, loc_pts_[i]);
  vsl_b_write(os, loc_lines_.size());
  for (unsigned i = 0; i < loc_lines_.size(); i++)
    vsl_b_write(os, loc_lines_[i]);
  vsl_b_write(os, loc_polys_.size());
  for (unsigned i = 0; i < loc_polys_.size(); i++)
    vsl_b_write(os, loc_polys_[i]);
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
    vcl_cout << "volm_osm_objects -- unknown binary io version " << ver << '\n';
    return;
  }
}

bool volm_osm_objects::write_pts_to_kml(vcl_string const& kml_file)
{
  vcl_ofstream ofs(kml_file.c_str());
  bkml_write::open_document(ofs);

  // write the points into kml
  unsigned cnt = 0;
  for (vcl_vector<volm_osm_object_point_sptr>::iterator vit = loc_pts_.begin(); vit != loc_pts_.end(); ++vit)
  {
    vcl_string name = (*vit)->prop().name_;
    vcl_stringstream str_w;  str_w << "id=" << cnt++;
    bkml_write::write_location(ofs, (*vit)->loc(), name, str_w.str(), 0.6);
  }
  bkml_write::close_document(ofs);
  ofs.close();
  return true;
}

bool volm_osm_objects::write_lines_to_kml(vcl_string const& kml_file)
{
  vcl_ofstream ofs(kml_file.c_str());
  bkml_write::open_document(ofs);
  
  // write the roads into kml
  unsigned cnt = 0;
  for (vcl_vector<volm_osm_object_line_sptr>::iterator vit = loc_lines_.begin(); vit != loc_lines_.end(); ++vit)
  {
    double width = (*vit)->prop().width_;
    vcl_string name = (*vit)->prop().name_;
    vcl_stringstream str_w;
    str_w << "width=" << width << ", id=" << cnt++;
    bkml_write::write_path(ofs, (*vit)->line(), name, str_w.str(), 1.0, width);
  }
  bkml_write::close_document(ofs);
  ofs.close();
  return true;
}

bool volm_osm_objects::write_polys_to_kml(vcl_string const& kml_file)
{
  vcl_ofstream ofs(kml_file.c_str());
  bkml_write::open_document(ofs);

  // write the regions into kml
  unsigned cnt = 0;
  for (vcl_vector<volm_osm_object_polygon_sptr>::iterator vit = loc_polys_.begin(); vit != loc_polys_.end(); ++vit)
  {
    unsigned char level = (*vit)->prop().level_;
    vcl_string name = (*vit)->prop().name_;
    vcl_stringstream str_l;
    str_l << "level=" << (int)level << ", id=" << cnt++;
    unsigned char r,g,b;
    if (level == 0)      { r = 85;   g = 255;  b = 255; }
    else if (level == 1) { r = 255;  g = 170;  b = 127; }
    else if (level == 2) { r = 85,   g = 255;  b = 0;   }
    else if (level == 3) { r = 170,  g = 170;  b = 255; }
    else if (level == 4) { r = 0;    g = 255;  b = 127; }
    else                 { r = 170;  g = 170;  b = 170; }
    bkml_write::write_polygon(ofs, (*vit)->poly(), name, str_l.str(), 1.0, 3.0, 0.4, r, g, b);
  }
  bkml_write::close_document(ofs);
  ofs.close();
  return true;
}

volm_osm_object_ids::volm_osm_object_ids(vcl_string const& bin_file)
{
  vsl_b_ifstream is(bin_file.c_str());
  if (!is) {
    vcl_cerr << "In volm_osm_object_ids::volm_osm_object_ids() -- cannot open file: " << bin_file << vcl_endl;
    return;
  }
  this->b_read(is);
  is.close();
}

void volm_osm_object_ids::add_pt(unsigned const& pt_id)
{
  vcl_vector<unsigned>::iterator vit = vcl_find(pt_ids_.begin(), pt_ids_.end(), pt_id);
  if (vit == pt_ids_.end())
    pt_ids_.push_back(pt_id);
}

void volm_osm_object_ids::add_line(unsigned const& line_id)
{
  vcl_vector<unsigned>::iterator vit = vcl_find(line_ids_.begin(), line_ids_.end(), line_id);
  if (vit == line_ids_.end())
    line_ids_.push_back(line_id);
}

void volm_osm_object_ids::add_region(unsigned const& region_id)
{
  vcl_vector<unsigned>::iterator vit = vcl_find(region_ids_.begin(), region_ids_.end(), region_id);
  if (vit == region_ids_.end())
    region_ids_.push_back(region_id);
}

//: write self to binary
bool volm_osm_object_ids::write_osm_ids(vcl_string const& bin_file)
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
  for (unsigned i = 0; i < pt_ids_.size(); i++)
    vsl_b_write(os, pt_ids_[i]);
  vsl_b_write(os, line_ids_.size());
  for (unsigned i = 0; i < line_ids_.size(); i++)
    vsl_b_write(os, line_ids_[i]);
  vsl_b_write(os, region_ids_.size());
  for (unsigned i = 0; i < region_ids_.size(); i++)
    vsl_b_write(os, region_ids_[i]);
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
    vcl_cout << "volm_osm_object_ids -- unknown binary io version " << ver << '\n';
    return;
  }
}