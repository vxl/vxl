#include "volm_category_io.h"
//:
// \file
#include <vul/vul_file.h>

bool volm_land_layer::contains(vcl_string name)
{
  return name_.find(name) != vcl_string::npos ||
         name.find(name_) != vcl_string::npos;
}


// load the pre-defined transformation from nlcd/geo cover/osm to volm_land 
bool volm_osm_category_io::load_category_table(vcl_string const& filename,
                                               vcl_map<vcl_pair<vcl_string, vcl_string>, volm_land_layer> & land_category_table)
{
  if (!vul_file::exists(filename))
    return false;
  vcl_ifstream ifs(filename.c_str());
  vcl_string header;
  vcl_getline(ifs, header);
  vcl_string tag, value, name;
  unsigned id, level;
  while ( !ifs.eof()) {
    ifs >> tag;  ifs >> value;  ifs >> id;  ifs >> name;  ifs >> level;
    vil_rgb<vxl_byte> color(bvrml_color::heatmap_classic[id][0], bvrml_color::heatmap_classic[id][1], bvrml_color::heatmap_classic[id][2]);
    land_category_table[vcl_pair<vcl_string, vcl_string>(tag, value)] = volm_land_layer((unsigned char)id, name, (unsigned char)level, color);
  }
  return true;
}

// load the pre-defined road width for open street map roads
bool volm_osm_category_io::load_road_width_table(vcl_string const& filename, vcl_map<vcl_pair<vcl_string, vcl_string>, float>& osm_road_width)
{
  if (!vul_file::exists(filename))
    return false;
  vcl_ifstream ifs(filename.c_str());
  vcl_string header;
  vcl_getline(ifs, header);
  vcl_string tag, value;
  float width;
  while (!ifs.eof()) {
    ifs >> tag;  ifs >> value;  ifs >> width;
    osm_road_width[vcl_pair<vcl_string, vcl_string>(tag, value)] = width;
  }
  return true;
}