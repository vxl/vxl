// This is brl/bbas/volm/volm_category_io.h
#ifndef volm_category_io_h_
#define volm_category_io_h_
//: 
// \file
// \brief A io to read the pre-defined available land types for volumetric matcher related
// \author Yi Dong
// \date July 26, 2013
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_utility.h>
#include <vcl_map.h>
#include <vcl_set.h>
#include <vil/vil_rgb.h>
#include <vxl_config.h>
#include <bvrml/bvrml_write.h>

//: class to store all available land categories
class volm_land_layer
{
public:
  //: default constructor creates an invalid land category
  volm_land_layer() : id_(0), name_("invalid"), level_(0), width_(0.0), color_(vil_rgb<vxl_byte>(0,0,0)) {}
  //: constructor
  volm_land_layer(unsigned char const& id, vcl_string const& name,
                  unsigned char const& level, double const& width, vil_rgb<vxl_byte> const& color)
                  : id_(id), name_(name), level_(level), width_(width), color_(color) {}
  volm_land_layer(unsigned char const& id, vcl_string const& name, unsigned char const& level, double const& width)
    : id_(id), name_(name), level_(level), width_(width),
      color_(bvrml_color::heatmap_classic[id][0], bvrml_color::heatmap_classic[id][1], bvrml_color::heatmap_classic[id][2]) {}
                  
  //: destructor
  ~volm_land_layer() {}

  //: check the existence of certain land layer
  bool contains(vcl_string name);
  //: screen print
  void print() const
  {
    vcl_cout << " (id: " << (int)id_ << ", name: " << name_ 
             << ", level: " << (int)level_ <<  ", width: " << width_ << ')';
  }
  //: land id
  unsigned char id_;
  //: land name
  vcl_string name_;
  //: layer priority
  unsigned char level_;
  //: assigned color for this land
  vil_rgb<vxl_byte> color_;
  //: width (non zero for road and zero for all others)
  double width_;

};

class volm_osm_category_io
{
public:
  enum nlcd_values {NLCD_WATER = 11, NLCD_ICE_SNOW = 12, NLCD_SAND = 31,
                    NLCD_DEVELOPED_LOW = 22, NLCD_DEVELOPED_MED = 23, NLCD_DEVELOPED_HIGH = 24, NLCD_DEVELOPED_OPEN = 21,
                    NLCD_DECIDUOUS_FOREST = 41, NLCD_EVERGREEN_FOREST = 42, NLCD_MIXED_FOREST = 43,
                    NLCD_DWARF_SCRUB = 51, NLCD_SHRUB = 52,
                    NLCD_GRASSLAND = 71, NLCD_SEDGE = 72, NLCD_LICHENS = 73, NLCD_MOSS = 74,
                    NLCD_PASTURE = 81, NLCD_CROPS = 82,
                    NLCD_WOODY_WETLAND = 90, NLCD_EMERGENT_WETLAND = 95,
                    BUILDING_TALL = 119};

  enum geo_cover_values {GEO_DECIDUOUS_FOREST = 1, GEO_EVERGREEN_FOREST = 2, GEO_SHRUB = 3,
                         GEO_GRASSLAND = 4,  GEO_BARREN = 5,  GEO_URBAN = 6,
                         GEO_AGRICULTURE_GENERAL = 7, GEO_AGRICULTURE_RICE = 8,
                         GEO_WETLAND = 9, GEO_MANGROVE = 10,
                         GEO_WATER = 11, GEO_ICE = 12, GEO_CLOUD = 13};
  
  //: table to transfer open street map (osm) to volm available land table
  static bool load_category_table(vcl_string const& filename, vcl_map<vcl_pair<vcl_string, vcl_string>, volm_land_layer>& land_category_table);

  //: table of all defined road junctions loaded from text file
  static vcl_map<vcl_pair<int, int>, volm_land_layer> road_junction_table;

  //: function to load road junction file
  static bool load_road_junction_table(vcl_string const& filename, vcl_map<vcl_pair<int, int>, volm_land_layer>& road_junction_table);

  //: table to define default width of road loaded from open street map
  static bool load_road_width_table(vcl_string const& filename, vcl_map<vcl_pair<vcl_string, vcl_string>, float>& road_width_table);

  //: table to transfer nlcd label to volm label
  static vcl_map<int, volm_land_layer> nlcd_land_table;

  //: table to transfer geo_cover
  static vcl_map<int, volm_land_layer> geo_land_table;

  //: table of all possible volm_object
  static vcl_vector<vcl_string> volm_category_name_table;

  //: table of all defined volm_land_layer from NLCD, GEO_COVER and OSM list (key is the land_layer id)
  static vcl_map<unsigned, volm_land_layer> volm_land_table;

  //: table of all defined volm_land_layer from NLCD, GEO_COVER and OSM list (key is the land_layer name, note that the land name is unique)
  static vcl_map<vcl_string, volm_land_layer> volm_land_table_name;

  //: use the corresponding increment during hypotheses generation, 
  //  the unit of the increments is in meters (user of this table shall convert to degrees if necessary depending on lat, lon of the area)
  static vcl_map<int, double> geo_land_hyp_increments;

  //: table to transfer land tags in xml file to volm land category
  static vcl_map<vcl_string, volm_land_layer> tag_to_volm_land_table;

};

#endif // volm_category_io_h_
