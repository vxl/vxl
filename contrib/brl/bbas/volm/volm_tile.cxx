#include "volm_tile.h"
//:
// \file
#include <vcl_iomanip.h>
#include <vcl_sstream.h>

vcl_vector<volm_tile> volm_tile::generate_p1_tiles()
{
  vcl_vector<volm_tile> p1_tiles;
  p1_tiles.push_back(volm_tile(37, 118, 'N', 'W'));
  p1_tiles.push_back(volm_tile(37, 119, 'N', 'W'));
  p1_tiles.push_back(volm_tile(38, 118, 'N', 'W'));
  p1_tiles.push_back(volm_tile(38, 119, 'N', 'W'));
  p1_tiles.push_back(volm_tile(30, 82, 'N', 'W'));
  p1_tiles.push_back(volm_tile(31, 81, 'N', 'W'));
  p1_tiles.push_back(volm_tile(31, 82, 'N', 'W'));
  p1_tiles.push_back(volm_tile(32, 80, 'N', 'W'));
  p1_tiles.push_back(volm_tile(32, 81, 'N', 'W'));
  p1_tiles.push_back(volm_tile(33, 78, 'N', 'W'));
  p1_tiles.push_back(volm_tile(33, 79, 'N', 'W'));
  p1_tiles.push_back(volm_tile(33, 80, 'N', 'W'));
  p1_tiles.push_back(volm_tile(34, 77, 'N', 'W'));
  p1_tiles.push_back(volm_tile(34, 78, 'N', 'W'));
  p1_tiles.push_back(volm_tile(34, 79, 'N', 'W'));
  p1_tiles.push_back(volm_tile(35, 76, 'N', 'W'));
  p1_tiles.push_back(volm_tile(35, 77, 'N', 'W'));
  p1_tiles.push_back(volm_tile(36, 76, 'N', 'W'));
  return p1_tiles;
}

vcl_string volm_tile::get_string()
{
  vcl_stringstream str;
  str << hemisphere_ << lat_ << direction_ << vcl_setfill('0') << vcl_setw(3) << lon_;
  return str.str();
}
