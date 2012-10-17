//This is brl/bbas/volm/volm_tile.h
#ifndef volm_tile_h_
#define volm_tile_h_
//:
// \file
// \brief A class to hold volumetric matchers
// units are in meters
//
// \author Ozge C. Ozcanli
// \date September 18, 2012

#include <vcl_string.h>
#include <vcl_vector.h>

class volm_tile
{
 public:
  int lat_, lon_;
  char hemisphere_;  // 'N' or 'S'
  char direction_;   // 'E' or 'W'

  volm_tile(int lat, int lon, char hemisphere, char direction) : lat_(lat), lon_(lon), hemisphere_(hemisphere), direction_(direction) {}
  vcl_string get_string();

  static vcl_vector<volm_tile> generate_p1_tiles();
  void write_kml(vcl_string, int) { /* NOT YET IMPLEMENTED */ }
};


#endif // volm_tile_h_
