#include "volm_category_io.h"
//:
// \file
#include <vul/vul_file.h>
#include <vcl_where_root_dir.h>
#include "volm_utils.h"

vil_rgb<vxl_byte> color(unsigned char id)
{
  if (id == (unsigned char)34)  // tall building as blue
    return {0,0,255};
  else if (id == (unsigned char)29)
    return {255, 0, 0};
  else
    return vil_rgb<vxl_byte>(bvrml_color::heatmap_classic[id][0],
                             bvrml_color::heatmap_classic[id][1],
                             bvrml_color::heatmap_classic[id][2]);
}

bool volm_land_layer::contains(const std::string& name)
{
  return name_.find(name) != std::string::npos ||
         name.find(name_) != std::string::npos;
}


// load the pre-defined transformation from nlcd/geo cover/osm to volm_land
bool volm_osm_category_io::load_category_table(std::string const& filename,
                                               std::map<std::pair<std::string, std::string>, volm_land_layer> & land_category_table)
{
  if (!vul_file::exists(filename))
    return false;
  std::ifstream ifs(filename.c_str());
  std::string header;
  std::getline(ifs, header);
  std::string tag, value, name;
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
    land_category_table[std::pair<std::string, std::string>(tag, value)] = volm_land_layer((unsigned char)id, name, (unsigned char)level, width, id_color);
  }
  return true;
}

// load the pre-defined road width for open street map roads
bool volm_osm_category_io::load_road_width_table(std::string const& filename, std::map<std::pair<std::string, std::string>, float>& osm_road_width)
{
  if (!vul_file::exists(filename))
    return false;
  std::ifstream ifs(filename.c_str());
  std::string header;
  std::getline(ifs, header);
  std::string tag, value;
  float width;
  while (!ifs.eof()) {
    ifs >> tag;  ifs >> value;  ifs >> width;
    osm_road_width[std::pair<std::string, std::string>(tag, value)] = width;
  }
  return true;
}

// create table to transfer nlcd label to volm_land_layer
std::map<int, volm_land_layer> create_nlcd_to_volm_table()
{
  std::map<int, volm_land_layer> m;
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
  m[volm_osm_category_io::NLCD_SHRUB]            = volm_land_layer(10, "Dwarf_Scrub", 0, 0.0, color(10));
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
std::map<int, volm_land_layer> create_geo_cover_to_volm_table()
{
  std::map<int, volm_land_layer> m;
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

bool volm_osm_category_io::load_road_junction_table(std::string const& filename, std::map<std::pair<int, int>, volm_land_layer>& road_junction_table)
{
  if (!vul_file::exists(filename))
    return false;
  std::ifstream ifs(filename.c_str());
  std::string header;
  std::getline(ifs, header);
  int id1, id2;  std::string n1, n2;  float w1, w2;
  std::string name;  unsigned id, level;  double width;
  vil_rgb<vxl_byte> id_color;
  while ( !ifs.eof()) {
    ifs >> id1;  ifs >> n1;    ifs >> w1;
    ifs >> id2;  ifs >> n2;    ifs >> w2;
    ifs >> id;   ifs >> name;  ifs >> level;  ifs >> width;
    id_color = color(id);
    road_junction_table[std::pair<int, int>(id1, id2)] = volm_land_layer((unsigned char)id, name, (unsigned char)level, width, id_color);
    if (id1 != id2)
      road_junction_table[std::pair<int, int>(id2, id1)] = volm_land_layer((unsigned char)id, name, (unsigned char)level, width, id_color);
  }
  return true;
}


std::map<std::pair<int, int>, volm_land_layer> load_osm_road_junction_table()
{
  std::map<std::pair<int, int>, volm_land_layer> m;
  std::string txt_file = volm_utils::volm_src_root() + "road_junction_category.txt";
  std::ifstream ifs(txt_file.c_str());
  if (!ifs.is_open()) {
    std::cerr << " cannot open: " << txt_file << '\n';
    return m;
  }
  std::string header;
  std::getline(ifs, header);
  int id1, id2;  std::string n1, n2;  float w1, w2;
  std::string name;  unsigned id, level;  double width;
  vil_rgb<vxl_byte> id_color;
  while ( !ifs.eof()) {
    ifs >> id1;  ifs >> n1;    ifs >> w1;
    ifs >> id2;  ifs >> n2;    ifs >> w2;
    ifs >> id;   ifs >> name;  ifs >> level;  ifs >> width;
    id_color = color(id);
    m[std::pair<int, int>(id1, id2)] = volm_land_layer((unsigned char)id, name, (unsigned char)level, width, id_color);
    if (id1 != id2)
      m[std::pair<int, int>(id2, id1)] = volm_land_layer((unsigned char)id, name, (unsigned char)level, width, id_color);
  }
  return m;
}

std::map<std::string, volm_land_layer> load_tag_to_volm_land_table()
{
  std::map<std::string, volm_land_layer> m;
  std::string txt_file = volm_utils::volm_src_root() + "bae_tag_to_volm_labels.txt";
  std::ifstream ifs(txt_file.c_str());
  if (!ifs.is_open()) {
    std::cerr << " cannot open: " << txt_file << '\n';
    return m;
  }
  std::string header;
  std::getline(ifs,header);
  std::string tag_name, volm_land_name;
  while ( !ifs.eof()) {
    ifs >> tag_name;  ifs >> volm_land_name;
    if (volm_land_name.compare("invalid") != 0)
      m[tag_name] = volm_osm_category_io::volm_land_table_name[volm_land_name];
  }
  return m;
}

std::map<unsigned, volm_land_layer> create_volm_land_table()
{
  std::map<unsigned, volm_land_layer> m;

  std::map<int, volm_land_layer> nlcd_table = create_nlcd_to_volm_table();
  std::map<int, volm_land_layer> geo_table = create_geo_cover_to_volm_table();
  std::map<std::pair<std::string, std::string>, volm_land_layer> osm_land_table;
  std::map<std::pair<int, int>, volm_land_layer> road_junction_table = load_osm_road_junction_table();

  std::string osm_to_volm_txt = volm_utils::volm_src_root() + "osm_to_volm_labels.txt";
  volm_osm_category_io::load_category_table(osm_to_volm_txt, osm_land_table);

  for (auto & mit : nlcd_table)
    m.insert(std::pair<unsigned, volm_land_layer>(mit.second.id_, mit.second));

  for (auto & mit : geo_table)
    m.insert(std::pair<unsigned, volm_land_layer>(mit.second.id_, mit.second));

  for (auto & mit : osm_land_table)
    m.insert(std::pair<unsigned, volm_land_layer>(mit.second.id_, mit.second));

  for (auto & mit : road_junction_table)
    m.insert(std::pair<unsigned, volm_land_layer>(mit.second.id_, mit.second));
  return m;
}

std::map<std::string, volm_land_layer> create_volm_land_table_name()
{
  std::map<std::string, volm_land_layer> m;
  std::map<unsigned, volm_land_layer> id_table = create_volm_land_table();
  auto mit = id_table.begin();
  for (; mit != id_table.end(); ++mit)
    m.insert(std::pair<std::string, volm_land_layer>(mit->second.name_, mit->second));
  return m;
}

// create a string table containing all defined volm_land_layer name, the order follows the volm_land_layer id
std::vector<std::string> create_volm_land_layer_name_table()
{
  std::vector<std::string> out;
  std::map<unsigned, volm_land_layer> m = create_volm_land_table();
  out.reserve(m.size());
for (auto & mit : m)
    out.push_back(mit.second.name_);
  return out;
}

// create table with the increments to use during hypotheses generation according to each land type, the unit is in meters
std::map<int, double> create_geo_cover_hyp_increments_for_roads()
{
  std::map<int, double> m;
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
  //m[volm_osm_category_io::GEO_WATER]               = 1000;  // almost no hyps on water (for now)
  m[volm_osm_category_io::GEO_ICE]                 = 1000; // almost no hyps on ice (for now)
  m[volm_osm_category_io::GEO_CLOUD]               = 4;  // use the most common -- CAUTION: some cloud coverage may prevent good density on urban areas
  m[volm_osm_category_io::NLCD_DECIDUOUS_FOREST]   = 30;  // almost no hyps on the forest areas
  m[volm_osm_category_io::NLCD_EVERGREEN_FOREST]   = 30;
  m[volm_osm_category_io::NLCD_MIXED_FOREST]       = 30;
  m[volm_osm_category_io::NLCD_DWARF_SCRUB]        = 30;
  m[volm_osm_category_io::NLCD_SHRUB]              = 30;

  m[volm_osm_category_io::NLCD_GRASSLAND]          = 4;
  m[volm_osm_category_io::NLCD_LICHENS]            = 4;
  m[volm_osm_category_io::NLCD_MOSS]               = 4;
  m[volm_osm_category_io::NLCD_SEDGE]              = 4;
  m[volm_osm_category_io::NLCD_CROPS]              = 10;
  m[volm_osm_category_io::NLCD_PASTURE]            = 10;

  m[volm_osm_category_io::NLCD_EMERGENT_WETLAND]   = 4;
  m[volm_osm_category_io::NLCD_WOODY_WETLAND]      = 4;

  m[volm_osm_category_io::NLCD_DEVELOPED_HIGH]     = 4;
  m[volm_osm_category_io::NLCD_DEVELOPED_MED]      = 4;
  m[volm_osm_category_io::NLCD_DEVELOPED_LOW]      = 4;
  m[volm_osm_category_io::NLCD_DEVELOPED_OPEN]     = 10;

  m[volm_osm_category_io::NLCD_WATER]              = 4;
  m[volm_osm_category_io::NLCD_SAND]               = 4;
  m[volm_osm_category_io::NLCD_ICE_SNOW]           = 1000;

  m[volm_osm_category_io::NLCD_INVALID]            = 4;
  return m;
}

std::map<std::pair<int, int>, volm_land_layer> volm_osm_category_io::road_junction_table = load_osm_road_junction_table();
std::map<int, volm_land_layer> volm_osm_category_io::nlcd_land_table = create_nlcd_to_volm_table();
std::map<int, volm_land_layer> volm_osm_category_io::geo_land_table  = create_geo_cover_to_volm_table();
std::map<unsigned, volm_land_layer> volm_osm_category_io::volm_land_table = create_volm_land_table();
std::map<std::string, volm_land_layer> volm_osm_category_io::volm_land_table_name = create_volm_land_table_name();
std::vector<std::string> volm_osm_category_io::volm_category_name_table = create_volm_land_layer_name_table();
std::map<int, double> volm_osm_category_io::geo_land_hyp_increments  = create_geo_cover_hyp_increments_for_roads();
std::map<std::string, volm_land_layer> volm_osm_category_io::tag_to_volm_land_table = load_tag_to_volm_land_table();
