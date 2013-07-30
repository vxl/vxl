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
  volm_land_layer() : id_(0), name_("invalid"), level_(0), color_(vil_rgb<vxl_byte>(0,0,0)) {}
  //: constructor
  volm_land_layer(unsigned char const& id, vcl_string const& name,
                  unsigned char const& level, vil_rgb<vxl_byte> const& color)
                  : id_(id), name_(name), level_(level), color_(color) {}
  //: destructor
  ~volm_land_layer() {}

  //: check the existence of certain land layer
  bool contains(vcl_string name);
  //: land id
  unsigned char id_;
  //: land name
  vcl_string name_;
  //: layer priority
  unsigned char level_;
  //: assigned color for this land
  vil_rgb<vxl_byte> color_;
};

class volm_osm_category_io
{
public:
  enum nlcd_values {WATER = 11, ICE_SNOW = 12, SAND = 31,
                    DEVELOPED_LOW = 22, DEVELOPED_MED = 23, DEVELOPED_HIGH = 24, DEVELOPED_OPEN = 2,
                    DECIDUOUS_FOREST = 41, EVERGREEN_FOREST = 42, MIXED_FOREST = 43,
                    DWARF_SCRUB = 51, SHRUB = 52,
                    GRASSLAND = 71, SEDGE = 72, LICHENS = 73, MOSS = 74,
                    PASTURE = 81, CROPS = 82,
                    WOODY_WETLAND = 90, WETLAND = 95};
  
  
  //: table to transfer open street map (osm) to volm available land table
  static bool load_category_table(vcl_string const& filename, vcl_map<vcl_pair<vcl_string, vcl_string>, volm_land_layer>& land_category_table);

  //: table to define default width of road loaded from open street map
  static bool load_road_width_table(vcl_string const& filename, vcl_map<vcl_pair<vcl_string, vcl_string>, float>& road_width_table);

};

#endif // volm_category_io_h_