#include "volm_category_io.h"
//:
// \file
#include <vul/vul_file.h>
#include <vcl_where_root_dir.h>


vil_rgb<vxl_byte> color(unsigned char id)
{
  if (id == (unsigned char)34)  // tall building as blue
    return vil_rgb<vxl_byte>(0,0,255);
  else
    return vil_rgb<vxl_byte>(bvrml_color::heatmap_classic[id][0],
                             bvrml_color::heatmap_classic[id][1],
                             bvrml_color::heatmap_classic[id][2]);
}

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
  double width;
  while ( !ifs.eof()) {
    ifs >> tag;  ifs >> value;  ifs >> id;  ifs >> name;  ifs >> level;  ifs >> width;
    vil_rgb<vxl_byte> id_color;
    if (name == "invalid")
      id_color = vil_rgb<vxl_byte>(0,0,0);
    else if (name == "building")
      id_color = vil_rgb<vxl_byte>(255,255,255);
    else
      id_color = color(id);
    land_category_table[vcl_pair<vcl_string, vcl_string>(tag, value)] = volm_land_layer((unsigned char)id, name, (unsigned char)level, width, id_color);
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

// create table to transfer nlcd label to volm_land_layer
vcl_map<int, volm_land_layer> create_nlcd_to_volm_table()
{
  vcl_map<int, volm_land_layer> m;
  m[volm_osm_category_io::NLCD_WATER] = volm_land_layer(1, "Open_Water", 0, 0.0, color(1));
  m[volm_osm_category_io::NLCD_ICE_SNOW]         = volm_land_layer(2, "Perennial_Ice/Snow", 0, 0.0, color(2));
  m[volm_osm_category_io::NLCD_DEVELOPED_OPEN]   = volm_land_layer(3, "Developed/Open_Space", 0, 0.0, color(3));
  m[volm_osm_category_io::NLCD_DEVELOPED_LOW]    = volm_land_layer(4, "Developed/Low_Intensity", 0, 0.0, color(4));
  m[volm_osm_category_io::NLCD_DEVELOPED_MED]    = volm_land_layer(4, "Developed/Medium_Intensity", 0, 0.0, color(4));
  m[volm_osm_category_io::NLCD_DEVELOPED_HIGH]   = volm_land_layer(5, "Developed/High_Intensity", 0, 0.0, color(5));
  m[volm_osm_category_io::NLCD_SAND]             = volm_land_layer(6, "Barren_Land/Beach", 0, 0.0, color(6));
  m[volm_osm_category_io::NLCD_DECIDUOUS_FOREST] = volm_land_layer(7, "Deciduous_Forest", 0, 0.0, color(7));
  m[volm_osm_category_io::NLCD_EVERGREEN_FOREST] = volm_land_layer(8, "Evergreen_Forest", 0, 0.0, color(8));
  m[volm_osm_category_io::NLCD_MIXED_FOREST]     = volm_land_layer(9, "Mixed_Forest", 0, 0.0, color(9));
  m[volm_osm_category_io::NLCD_DWARF_SCRUB]      = volm_land_layer(10, "Dwarf_Scrub", 0, 0.0, color(10));
  m[volm_osm_category_io::NLCD_SHRUB]            = volm_land_layer(10, "Shrub/Scrub", 0, 0.0, color(10));
  m[volm_osm_category_io::NLCD_GRASSLAND]        = volm_land_layer(11, "Grassland/Herbaceous", 0, 0.0, color(11));
  m[volm_osm_category_io::NLCD_SEDGE]            = volm_land_layer(11, "Sedge/Herbaceous", 0, 0.0, color(11));
  m[volm_osm_category_io::NLCD_LICHENS]          = volm_land_layer(11, "lichens", 0, 0.0, color(11));
  m[volm_osm_category_io::NLCD_MOSS]             = volm_land_layer(11, "Moss(Alaska_only)", 0, 0.0, color(11));
  m[volm_osm_category_io::NLCD_PASTURE]          = volm_land_layer(12, "Pasture_Hay", 0, 0.0, color(12));
  m[volm_osm_category_io::NLCD_CROPS]            = volm_land_layer(13, "Cultivated_Crops", 0, 0.0, color(13));
  m[volm_osm_category_io::NLCD_WOODY_WETLAND]    = volm_land_layer(14, "Woody_Wetlands/Marina", 0, 0.0, color(14));
  m[volm_osm_category_io::NLCD_EMERGENT_WETLAND] = volm_land_layer(14, "Emergent_Herbaceous_Wetlands", 0, 0.0, color(14));
  m[102]                                         = volm_land_layer(17, "beaches", 0, 0.0, color(17));
  m[112]                                         = volm_land_layer(27, "mines", 0, 0.0, color(27));
  m[115]                                         = volm_land_layer(30, "wharves", 3, 0.0, color(30));
  m[118]                                         = volm_land_layer(33, "beach_walkway", 0, 1.0, color(33));
  m[volm_osm_category_io::BUILDING_TALL]         = volm_land_layer(34, "tall_building", 4, 0.0, color(34));

  return m;
}

// create table to transfer geo cover data to volm_land_layer
vcl_map<int, volm_land_layer> create_geo_cover_to_volm_table()
{
  vcl_map<int, volm_land_layer> m;
  m[volm_osm_category_io::GEO_DECIDUOUS_FOREST]    = volm_land_layer(7, "Deciduous_Forest", 0, 0.0, color(7));
  m[volm_osm_category_io::GEO_EVERGREEN_FOREST]    = volm_land_layer(8, "Evergreen_Forest", 0, 0.0, color(8));
  m[volm_osm_category_io::GEO_SHRUB]               = volm_land_layer(10, "Dwarf_Scrub", 0, 0.0, color(10));
  m[volm_osm_category_io::GEO_GRASSLAND]           = volm_land_layer(11, "Grassland/Herbaceous", 0, 0.0, color(11));
  m[volm_osm_category_io::GEO_BARREN]              = volm_land_layer(6, "Barren_Land/Beach", 0, 0.0, color(6));
  m[volm_osm_category_io::GEO_URBAN]               = volm_land_layer(4, "Developed/Medium_Intensity", 0, 0.0, color(4));
  m[volm_osm_category_io::GEO_AGRICULTURE_GENERAL] = volm_land_layer(13, "Cultivated_Crops", 0, 0.0, color(13));
  m[volm_osm_category_io::GEO_AGRICULTURE_RICE]    = volm_land_layer(35, "Cultivated_Rice/Paddy", 0, 0.0, color(35));
  m[volm_osm_category_io::GEO_WETLAND]             = volm_land_layer(14, "Woody_Wetlands/Marina", 0, 0.0, color(14));
  m[volm_osm_category_io::GEO_MANGROVE]            = volm_land_layer(14, "Woody_Wetlands/Marina", 0, 0.0, color(14));
  m[volm_osm_category_io::GEO_WATER]               = volm_land_layer(1, "Open_Water", 0, 0.0, color(1));
  m[volm_osm_category_io::GEO_ICE]                 = volm_land_layer(2, "Perennial_Ice/Snow", 0, 0.0, color(2));
  m[volm_osm_category_io::GEO_CLOUD]               = volm_land_layer(0, "invalid", 0, 0.0, vil_rgb<vxl_byte>(0,0,0));
  return m;
}

bool volm_osm_category_io::load_road_junction_table(vcl_string const& filename, vcl_map<vcl_pair<int, int>, volm_land_layer>& road_junction_table)
{
  if (!vul_file::exists(filename))
    return false;
  vcl_ifstream ifs(filename.c_str());
  vcl_string header;
  vcl_getline(ifs, header);
  int id1, id2;  vcl_string n1, n2;  float w1, w2;
  vcl_string name;  unsigned id, level;  double width;
  vil_rgb<vxl_byte> id_color;
  while ( !ifs.eof()) {
    ifs >> id1;  ifs >> n1;    ifs >> w1;
    ifs >> id2;  ifs >> n2;    ifs >> w2;
    ifs >> id;   ifs >> name;  ifs >> level;  ifs >> width;
    id_color = color(id);
    road_junction_table[vcl_pair<int, int>(id1, id2)] = volm_land_layer((unsigned char)id, name, (unsigned char)level, width, id_color);
    if (id1 != id2)
      road_junction_table[vcl_pair<int, int>(id2, id1)] = volm_land_layer((unsigned char)id, name, (unsigned char)level, width, id_color);
  }
  return true;
}


vcl_map<vcl_pair<int, int>, volm_land_layer> load_osm_road_junction_table()
{
  vcl_map<vcl_pair<int, int>, volm_land_layer> m;
  vcl_string txt_file = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bbas/volm/road_junction_category.txt";
  vcl_ifstream ifs(txt_file.c_str());
  vcl_string header;
  vcl_getline(ifs, header);
  int id1, id2;  vcl_string n1, n2;  float w1, w2;
  vcl_string name;  unsigned id, level;  double width;
  vil_rgb<vxl_byte> id_color;
  while ( !ifs.eof()) {
    ifs >> id1;  ifs >> n1;    ifs >> w1;
    ifs >> id2;  ifs >> n2;    ifs >> w2;
    ifs >> id;   ifs >> name;  ifs >> level;  ifs >> width;
    id_color = color(id);
    m[vcl_pair<int, int>(id1, id2)] = volm_land_layer((unsigned char)id, name, (unsigned char)level, width, id_color);
    if (id1 != id2)
      m[vcl_pair<int, int>(id2, id1)] = volm_land_layer((unsigned char)id, name, (unsigned char)level, width, id_color);
  }
  return m;
}

vcl_map<vcl_string, volm_land_layer> load_tag_to_volm_land_table()
{
  vcl_map<vcl_string, volm_land_layer> m;
  vcl_string txt_file = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bbas/volm/bae_tag_to_volm_labels.txt";
  vcl_ifstream ifs(txt_file.c_str());
  vcl_string header;
  vcl_getline(ifs,header);
  vcl_string tag_name, volm_land_name;
  while ( !ifs.eof()) {
    ifs >> tag_name;  ifs >> volm_land_name;
    if (volm_land_name.compare("invalid") != 0)
      m[tag_name] = volm_osm_category_io::volm_land_table_name[volm_land_name];
  }
  return m;
}

vcl_map<unsigned, volm_land_layer> create_volm_land_table()
{
  vcl_map<unsigned, volm_land_layer> m;

  vcl_map<int, volm_land_layer> nlcd_table = create_nlcd_to_volm_table();
  vcl_map<int, volm_land_layer> geo_table = create_geo_cover_to_volm_table();
  vcl_map<vcl_pair<vcl_string, vcl_string>, volm_land_layer> osm_land_table;
  vcl_map<vcl_pair<int, int>, volm_land_layer> road_junction_table = load_osm_road_junction_table();

  vcl_string osm_to_volm_txt = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bbas/volm/osm_to_volm_labels.txt";
  volm_osm_category_io::load_category_table(osm_to_volm_txt, osm_land_table);

  for (vcl_map<int, volm_land_layer>::iterator mit = nlcd_table.begin(); mit != nlcd_table.end(); ++mit)
    m.insert(vcl_pair<unsigned, volm_land_layer>(mit->second.id_, mit->second));

  for (vcl_map<int, volm_land_layer>::iterator mit = geo_table.begin(); mit != geo_table.end(); mit++)
    m.insert(vcl_pair<unsigned, volm_land_layer>(mit->second.id_, mit->second));

  for (vcl_map<vcl_pair<vcl_string, vcl_string>, volm_land_layer>::iterator mit = osm_land_table.begin();
       mit != osm_land_table.end(); ++mit)
    m.insert(vcl_pair<unsigned, volm_land_layer>(mit->second.id_, mit->second));

  for (vcl_map<vcl_pair<int, int>, volm_land_layer>::iterator mit = road_junction_table.begin();
       mit != road_junction_table.end(); ++mit)
    m.insert(vcl_pair<unsigned, volm_land_layer>(mit->second.id_, mit->second));
  return m;
}

vcl_map<vcl_string, volm_land_layer> create_volm_land_table_name()
{
  vcl_map<vcl_string, volm_land_layer> m;
  vcl_map<unsigned, volm_land_layer> id_table = create_volm_land_table();
  vcl_map<unsigned, volm_land_layer>::iterator mit = id_table.begin();
  for (; mit != id_table.end(); ++mit)
    m.insert(vcl_pair<vcl_string, volm_land_layer>(mit->second.name_, mit->second));
  return m;
}

// create a string table containing all defined volm_land_layer name, the order follows the volm_land_layer id
vcl_vector<vcl_string> create_volm_land_layer_name_table()
{
  vcl_vector<vcl_string> out;
  vcl_map<unsigned, volm_land_layer> m = create_volm_land_table();
  for (vcl_map<unsigned, volm_land_layer>::iterator mit = m.begin(); mit != m.end(); ++mit)
    out.push_back(mit->second.name_);
  return out;
}

// create table with the increments to use during hypotheses generation according to each land type, the unit is in meters
vcl_map<int, double> create_geo_cover_hyp_increments_for_roads()
{
  vcl_map<int, double> m;
  m[volm_osm_category_io::GEO_DECIDUOUS_FOREST]    = 500;  // almost no hyps on the forest areas
  m[volm_osm_category_io::GEO_EVERGREEN_FOREST]    = 500;
  m[volm_osm_category_io::GEO_SHRUB]               = 60;
  m[volm_osm_category_io::GEO_GRASSLAND]           = 4;
  m[volm_osm_category_io::GEO_BARREN]              = 30;
  m[volm_osm_category_io::GEO_URBAN]               = 4;  // 4 meter increments in the urban region
  m[volm_osm_category_io::GEO_AGRICULTURE_GENERAL] = 4;
  m[volm_osm_category_io::GEO_AGRICULTURE_RICE]    = 4;
  m[volm_osm_category_io::GEO_WETLAND]             = 30;
  m[volm_osm_category_io::GEO_MANGROVE]            = 500;  // almost no hyps
  m[volm_osm_category_io::GEO_WATER]               = 1000;  // almost no hyps on water (for now)
  m[volm_osm_category_io::GEO_ICE]                 = 1000; // almost no hyps on ice (for now)
  m[volm_osm_category_io::GEO_CLOUD]               = 4;  // use the most common -- CAUTION: some cloud coverage may prevent good density on urban areas
  return m;
}

vcl_map<vcl_pair<int, int>, volm_land_layer> volm_osm_category_io::road_junction_table = load_osm_road_junction_table();
vcl_map<int, volm_land_layer> volm_osm_category_io::nlcd_land_table = create_nlcd_to_volm_table();
vcl_map<int, volm_land_layer> volm_osm_category_io::geo_land_table  = create_geo_cover_to_volm_table();
vcl_map<unsigned, volm_land_layer> volm_osm_category_io::volm_land_table = create_volm_land_table();
vcl_map<vcl_string, volm_land_layer> volm_osm_category_io::volm_land_table_name = create_volm_land_table_name();
vcl_vector<vcl_string> volm_osm_category_io::volm_category_name_table = create_volm_land_layer_name_table();
vcl_map<int, double> volm_osm_category_io::geo_land_hyp_increments  = create_geo_cover_hyp_increments_for_roads();
vcl_map<vcl_string, volm_land_layer> volm_osm_category_io::tag_to_volm_land_table = load_tag_to_volm_land_table();
