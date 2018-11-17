#include <iostream>
#include <iomanip>
#include "volm_io.h"
//:
// \file
#include <bkml/bkml_parser.h>
#include <bkml/bkml_write.h>
#include <bpgl/bpgl_camera_utils.h>
#include <bvgl/bvgl_labelme_parser.h>
#include <depth_map/depth_map_region_sptr.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_polygon.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <bsol/bsol_algs.h>
#include <vul/vul_file.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "volm_utils.h"
#include <cassert>
#include <utility>
#include <vsl/vsl_vector_io.h>

std::map<std::string, depth_map_region::orientation> create_orient_map()
{
  std::map<std::string, depth_map_region::orientation> m;
  m["horizontal"] = depth_map_region::HORIZONTAL;
  m["front_parallel"] = depth_map_region::FRONT_PARALLEL;
  m["slanted_right"] = depth_map_region::SLANTED_RIGHT;
  m["slanted_left"] = depth_map_region::SLANTED_LEFT;
  m["porous"] = depth_map_region::POROUS;
  m["infinite"] = depth_map_region::INFINT;
  return m;
}

// list of the possible values for indexed orientations from the reference world
std::map<int, vil_rgb<vxl_byte> > create_orient_colors()
{
  std::map<int, vil_rgb<vxl_byte> > m;
  m[0] = vil_rgb<vxl_byte>(255, 0, 0);  // no value
  m[10] = vil_rgb<vxl_byte>(255, 255, 255);  // no value
  m[100] = vil_rgb<vxl_byte>(100, 0, 0);  // no value
  m[1] = vil_rgb<vxl_byte>(0, 255, 0);  // horizontal surfaces (e.g. ground, water, etc.)
  m[2] = vil_rgb<vxl_byte>(0, 255, 255);  // vertical facing west
  m[3] = vil_rgb<vxl_byte>(0, 255, 155);  // vertical facing south west
  m[4] = vil_rgb<vxl_byte>(255, 255, 0);  // vertical facing south
  m[5] = vil_rgb<vxl_byte>(155, 255, 0);    // vertical facing south east
  m[6] = vil_rgb<vxl_byte>(155, 255, 155);  // vertical facing east
  m[7] = vil_rgb<vxl_byte>(155, 155, 155);  // vertical facing north east
  m[8] = vil_rgb<vxl_byte>(155, 0, 155);  // vertical facing north
  m[9] = vil_rgb<vxl_byte>(0, 155, 155);  // vertical facing north west
  return m;
}

std::map<int, volm_attributes > create_label_map()
{
  std::map<int, volm_attributes > m;
  m[0] = volm_attributes(0, "invalid",                                                 vil_rgb<vxl_byte>(255,  0,    0));  // invalid
  m[volm_label_table::WATER] = volm_attributes(1, "Open Water",                         vil_rgb<vxl_byte>(0,    0,    100));
  m[12] = volm_attributes(2, "Perennial Ice/Snow",                                      vil_rgb<vxl_byte>(255,  255,  200));  //
  m[volm_label_table::DEVELOPED_OPEN] = volm_attributes(3, "Developed, Open Space",     vil_rgb<vxl_byte>(50,   0,    0));
  m[volm_label_table::DEVELOPED_LOW] = volm_attributes(4, "Developed, Low Intensity",   vil_rgb<vxl_byte>(100,  0,    10));
  m[volm_label_table::DEVELOPED_MED] = volm_attributes(4, "Developed, Medium Intensity",vil_rgb<vxl_byte>(200,  0,    100));  //  (combined with prev class)
  m[volm_label_table::DEVELOPED_HIGH] = volm_attributes(5, "Developed, High Intensity", vil_rgb<vxl_byte>(220,  0,    100));  //
  m[volm_label_table::SAND] = volm_attributes(6, "Barren Land Beach",  vil_rgb<vxl_byte>(170,  170,  170));  //
  m[41] =                     volm_attributes(7, "Deciduous Forest",                    vil_rgb<vxl_byte>(0,    200,  0));  //
  m[42] =                     volm_attributes(8, "Evergreen Forest",                    vil_rgb<vxl_byte>(0,    250,  0));  //
  m[43] =                     volm_attributes(9, "Mixed Forest",                        vil_rgb<vxl_byte>(0,    100,  0));  //
  m[51] =                     volm_attributes(10, "Dwarf Scrub (Alaska only)",           vil_rgb<vxl_byte>(10,   50,   0));  //
  m[52] =                     volm_attributes(10, "Shrub/Scrub",                         vil_rgb<vxl_byte>(10,   50,   0));  //
  m[71] =                     volm_attributes(11, "Grassland/Herbaceous",               vil_rgb<vxl_byte>(0,    100,  20)); //
  m[72] =                     volm_attributes(11, "Sedge/Herbaceous (Alaska only)",     vil_rgb<vxl_byte>(0,    150,  10)); //
  m[73] =                     volm_attributes(11, "lichens",                            vil_rgb<vxl_byte>(0,    170,  10)); //
  m[74] =                     volm_attributes(11, "Moss (Alaska only)",                 vil_rgb<vxl_byte>(0,    200,  10)); //
  m[81] =                     volm_attributes(12, "Pasture Hay",                        vil_rgb<vxl_byte>(0,    120,  120)); //
  m[82] =                     volm_attributes(13, "Cultivated Crops",                   vil_rgb<vxl_byte>(210,  105,  30)); //
  m[90] =                     volm_attributes(14, "Woody Wetlands Marina",            vil_rgb<vxl_byte>(176,  196,  222)); //
  m[95] =                     volm_attributes(14, "Emergent Herbaceous Wetlands",       vil_rgb<vxl_byte>(176,  196,  255)); //
  m[volm_label_table::BUILDING] = volm_attributes(15, "building",                       vil_rgb<vxl_byte>(255,  255,  255)); //

  m[101] =     volm_attributes(16, "airports",                                          vil_rgb<vxl_byte>(0,    255,  255));
  m[102] =      volm_attributes(17, "beaches",                                          vil_rgb<vxl_byte>(0,    200,  255));
  m[103] =      volm_attributes(18, "bridges",                                          vil_rgb<vxl_byte>(255,  10,   0));
  m[104] =   volm_attributes(19, "cemeteries",                                          vil_rgb<vxl_byte>(176,  176,  176));
  m[105] =   volm_attributes(20, "fcc_towers",                                          vil_rgb<vxl_byte>(255,  20,   0));
  m[volm_label_table::FORT] =        volm_attributes(21, "forts",                                          vil_rgb<vxl_byte>(255,  50,   0));
  m[107] = volm_attributes(22, "golf_courses",                                          vil_rgb<vxl_byte>(0,    255,  200));
  m[108] =      volm_attributes(23, "harbors",                                          vil_rgb<vxl_byte>(255,  100,  0));
  m[109] =       volm_attributes(24, "hotels",                                          vil_rgb<vxl_byte>(255,  255,  200));
  m[110] =  volm_attributes(25, "lighthouses",                                          vil_rgb<vxl_byte>(255,  200,  0));
  m[111] =      volm_attributes(26, "marinas",                                          vil_rgb<vxl_byte>(255,  255,  100));
  m[112] =        volm_attributes(27, "mines",                                          vil_rgb<vxl_byte>(255,  0,    100));
  m[113] =        volm_attributes(28, "parks",                                          vil_rgb<vxl_byte>(10,   255,  1));
  m[volm_label_table::PIER] = volm_attributes(29, "piers",                              vil_rgb<vxl_byte>(0,  0,    0));
  m[115] =      volm_attributes(30, "wharves",                                          vil_rgb<vxl_byte>(255,  0,    41));
  m[116] =      volm_attributes(31, "roads",                                            vil_rgb<vxl_byte>(255,  100,    41));
  m[117] =      volm_attributes(32, "parking_lots",                                     vil_rgb<vxl_byte>(255,  200,    41));
  m[118] =      volm_attributes(33, "beach_walkway",                                    vil_rgb<vxl_byte>(255,  200,    241));
  m[volm_label_table::BUILDING_TALL] =      volm_attributes(34, "tall_building",            vil_rgb<vxl_byte>(255,  200,    41)); // has more than 5 floor height, >20 m of height (building itself), e.g. count at least 5 floors on the building. warning: if a building with 3 floors (12 m) is on top of a hill of ~8 m, it may be categorized as tall

  return m;
}

std::map<std::string, depth_map_region::orientation> volm_orient_table::ori_id = create_orient_map();
std::map<int, volm_attributes > volm_label_table::land_id = create_label_map();
std::map<int, vil_rgb<vxl_byte> > volm_orient_table::ori_index_colors = create_orient_colors();

// assume that ids are ordered from 0 to n so the number of labels is n+1
unsigned volm_label_table::compute_number_of_labels()
{
  return volm_osm_category_io::volm_land_table.size();
#if 0
  unsigned max = 0;
  for (std::map<int, volm_attributes >::iterator it = land_id.begin(); it != land_id.end(); it++) {
    if (it->second.id_ > max)
      max = it->second.id_;
  }
  return max+1;
#endif
}

unsigned volm_label_table::number_of_labels_ = compute_number_of_labels();


void volm_category_attribute::read_category(std::map<std::string, volm_category_attribute> & category_table, const std::string& fname)
{
  std::ifstream ifs(fname.c_str());
  std::string type;
  std::string lnd;
  std::string ori;
  unsigned   is_active;
  while (!ifs.eof()) {
    ifs >> type; ifs >> ori;  ifs >> lnd;  ifs >> is_active;
    std::pair<std::string, volm_category_attribute> pair;
    pair.first = type;
    pair.second = volm_category_attribute(lnd, ori, is_active);
    category_table.insert(pair);
  }
  ifs.close();
}


std::map<unsigned char, std::vector<unsigned char> > create_fallback_label()
{
  // read the pre-defined fallback category table from text file
  // NOTE: this function needs modification if more fallback categroy is added
  std::map<unsigned char, std::vector<unsigned char> > m;
  std::vector<unsigned char> f(4,0);
  // load the text file
  std::string txt_file = volm_utils::volm_src_root() + "fallback_category.txt";
  std::ifstream ifs(txt_file.c_str());
  if (!ifs.is_open()) {
    std::cerr << " cannot open: " << txt_file << '\n';
    return m;
  }
  std::string header;
  std::getline(ifs, header);
  std::string name;
  unsigned level;
  float width;
  unsigned id, f1, f2, f3, f4;
  float w1, w2, w3, w4;
  while( !ifs.eof()) {
    ifs >> id;  ifs >> name;  ifs >> level;  ifs >> width;
    ifs >> f1;  ifs >> f2;  ifs >> f3;  ifs >> f4;
    ifs >> w1;  ifs >> w2;  ifs >> w3;  ifs >> w4;
    f.clear();
    f.push_back(f1);  f.push_back(f2);  f.push_back(f3);  f.push_back(f4);
    m[id] = f;
  }
  return m;
#if 0
  std::map<unsigned char, std::vector<unsigned char> > m;
  std::vector<unsigned char> f(4,0);
  f.clear();  f.push_back(0);  f.push_back(0);  f.push_back(0);  f.push_back(0);  m[0] = f;   // invalid    ----------> [invalid, invalid, invalid, invalid]
  f.clear();  f.push_back(1);  f.push_back(6);  f.push_back(17); f.push_back(3);  m[1] = f;   // open water ----------> [open water, sand, beach, develop(open space)]
  f.clear();  f.push_back(2);  f.push_back(1);  f.push_back(6);  f.push_back(3);  m[2] = f;   // perennial ice/snow --> [perennial ice/snow, water, sand, develop(open)]
  f.clear();  f.push_back(3);  f.push_back(4);  f.push_back(5);  f.push_back(6);  m[3] = f;   //
  f.clear();  f.push_back(4);  f.push_back(15);  f.push_back(5);  f.push_back(6);  m[4] = f;  //
  f.clear();  f.push_back(5);  f.push_back(4);  f.push_back(15); f.push_back(24); m[5] = f;   //
  f.clear();  f.push_back(6);  f.push_back(1);  f.push_back(3);  f.push_back(17); m[6] = f;   //
  f.clear();  f.push_back(7);  f.push_back(8);  f.push_back(9);  f.push_back(3);  m[7] = f;   //
  f.clear();  f.push_back(8);  f.push_back(7);  f.push_back(9);  f.push_back(3);  m[8] = f;   //
  f.clear();  f.push_back(9);  f.push_back(7);  f.push_back(8);  f.push_back(3);  m[9] = f;   //
  f.clear();  f.push_back(10); f.push_back(11); f.push_back(12); f.push_back(3);  m[10] = f;  //
  f.clear();  f.push_back(11); f.push_back(10); f.push_back(3);  f.push_back(12); m[11] = f;  //
  f.clear();  f.push_back(12); f.push_back(11); f.push_back(10); f.push_back(3);  m[12] = f;  //
  f.clear();  f.push_back(13); f.push_back(12); f.push_back(3);  f.push_back(4);  m[13] = f;  //
  f.clear();  f.push_back(14); f.push_back(1);  f.push_back(4);  f.push_back(6); m[14] = f;   //
  f.clear();  f.push_back(15); f.push_back(4);  f.push_back(5);  f.push_back(34); m[15] = f;  //  building --> building ,developed low, developed high, tall_building
  f.clear();  f.push_back(16); f.push_back(4);  f.push_back(3);  f.push_back(5);  m[16] = f;  //
  f.clear();  f.push_back(17); f.push_back(1);  f.push_back(6);  f.push_back(3);  m[17] = f;  //
  f.clear();  f.push_back(18); f.push_back(4);  f.push_back(5);  f.push_back(3);  m[18] = f;  //
  f.clear();  f.push_back(19); f.push_back(4);  f.push_back(5);  f.push_back(3);  m[19] = f;  //
  f.clear();  f.push_back(20); f.push_back(4);  f.push_back(5);  f.push_back(15); m[20] = f;  //
  f.clear();  f.push_back(21); f.push_back(4);  f.push_back(5);  f.push_back(15); m[21] = f;  //
  f.clear();  f.push_back(22); f.push_back(4);  f.push_back(5);  f.push_back(3);  m[22] = f;  //
  f.clear();  f.push_back(23); f.push_back(1);  f.push_back(4);  f.push_back(26); m[23] = f;  //
  f.clear();  f.push_back(24); f.push_back(4);  f.push_back(5);  f.push_back(15); m[24] = f;  //
  f.clear();  f.push_back(25); f.push_back(4);  f.push_back(5);  f.push_back(15); m[25] = f;  //
  f.clear();  f.push_back(26); f.push_back(23); f.push_back(1);  f.push_back(4);  m[26] = f;  //
  f.clear();  f.push_back(27); f.push_back(3);  f.push_back(6);  f.push_back(4);  m[27] = f;  //
  f.clear();  f.push_back(28); f.push_back(4);  f.push_back(5);  f.push_back(3);  m[28] = f;  //
  f.clear();  f.push_back(29); f.push_back(1);  f.push_back(15); f.push_back(6);  m[29] = f;  //
  f.clear();  f.push_back(30); f.push_back(1);  f.push_back(29); f.push_back(23); m[30] = f;  //
  f.clear();  f.push_back(31); f.push_back(4);  f.push_back(5);  f.push_back(3);  m[31] = f;  //
  f.clear();  f.push_back(32); f.push_back(31); f.push_back(4);  f.push_back(5);  m[32] = f;  //
  f.clear();  f.push_back(33); f.push_back(6);  f.push_back(15); f.push_back(1);  m[33] = f;  //
  f.clear();  f.push_back(34); f.push_back(15); f.push_back(4);  f.push_back(5);  m[34] = f;  // tall_building --> high_building, building, developed low, developed high
  return m;
#endif
}

std::map<unsigned char, std::vector<float> > create_fallback_weight()
{
  std::map<unsigned char, std::vector<float> > m;
  std::vector<float> w(4,0.0f);
  std::string txt_file = volm_utils::volm_src_root() + "fallback_category.txt";
  std::ifstream ifs(txt_file.c_str());
  if (!ifs.is_open()) {
    std::cerr << " cannot open: " << txt_file << '\n';
    return m;
  }
  std::string header;
  std::getline(ifs, header);
  std::string name;
  unsigned level;
  float width;
  unsigned id, f1, f2, f3, f4;
  float w1, w2, w3, w4;
  while( !ifs.eof()) {
    ifs >> id;  ifs >> name;  ifs >> level;  ifs >> width;
    ifs >> f1;  ifs >> f2;  ifs >> f3;  ifs >> f4;
    ifs >> w1;  ifs >> w2;  ifs >> w3;  ifs >> w4;
    w.clear();
    w.push_back(w1);  w.push_back(w2);  w.push_back(w3);  w.push_back(w4);
    m[id] = w;
  }
  return m;

#if 0
  std::map<unsigned char, std::vector<float> > m;
  std::vector<float> f(4, 0.0f);
  f.clear();  f.push_back(1.0f);  f.push_back(1.0f);  f.push_back(1.0f);  f.push_back(1.0f);  m[0] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.1f);  f.push_back(0.1f);  f.push_back(0.1f);  m[1] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.5f);  f.push_back(0.4f);  f.push_back(0.1f);  m[2] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.2f);  f.push_back(0.2f);  f.push_back(0.1f);  m[3] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(1.0f);  f.push_back(0.7f);  f.push_back(0.1f);  m[4] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.7f);  f.push_back(1.0f);  f.push_back(1.0f);  m[5] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.1f);  f.push_back(0.7f);  f.push_back(1.0f);  m[6] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.9f);  f.push_back(0.9f);  f.push_back(0.8f);  m[7] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.9f);  f.push_back(0.9f);  f.push_back(0.1f);  m[8] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.9f);  f.push_back(0.9f);  f.push_back(0.1f);  m[9] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.6f);  f.push_back(0.6f);  f.push_back(0.1f);  m[10] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.6f);  f.push_back(0.6f);  f.push_back(0.8f);  m[11] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.8f);  f.push_back(0.6f);  f.push_back(0.6f);  m[12] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.8f);  f.push_back(0.6f);  f.push_back(0.6f);  m[13] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.8f);  f.push_back(0.6f);  f.push_back(0.6f);  m[14] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.8f);  f.push_back(0.8f);  f.push_back(0.5f);  m[15] = f; // building --> building ,developed low, developed high, tall_building
  f.clear();  f.push_back(1.0f);  f.push_back(0.7f);  f.push_back(0.8f);  f.push_back(0.7f);  m[16] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(1.0f);  f.push_back(1.0f);  f.push_back(0.7f);  m[17] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.8f);  f.push_back(0.8f);  f.push_back(0.7f);  m[18] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.8f);  f.push_back(0.8f);  f.push_back(0.7f);  m[19] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.8f);  f.push_back(0.8f);  f.push_back(0.6f);  m[20] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.8f);  f.push_back(0.8f);  f.push_back(0.6f);  m[21] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.8f);  f.push_back(0.8f);  f.push_back(0.6f);  m[22] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.9f);  f.push_back(0.7f);  f.push_back(0.8f);  m[23] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.9f);  f.push_back(0.9f);  f.push_back(0.9f);  m[24] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.8f);  f.push_back(0.8f);  f.push_back(0.9f);  m[25] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.8f);  f.push_back(0.9f);  f.push_back(0.8f);  m[26] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.8f);  f.push_back(0.6f);  f.push_back(0.6f);  m[27] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.8f);  f.push_back(0.8f);  f.push_back(0.5f);  m[28] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.7f);  f.push_back(0.1f);  f.push_back(0.5f);  m[29] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(1.0f);  f.push_back(0.7f);  f.push_back(0.6f);  m[30] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.8f);  f.push_back(0.8f);  f.push_back(0.6f);  m[31] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.9f);  f.push_back(0.8f);  f.push_back(0.8f);  m[32] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.8f);  f.push_back(0.7f);  f.push_back(0.7f);  m[33] = f; //
  f.clear();  f.push_back(1.0f);  f.push_back(0.4f);  f.push_back(0.8f);  f.push_back(0.8f);  m[34] = f; // tall_building --> high_building, building, developed low, developed high
  return m;
#endif
}

std::map<unsigned char, std::vector<unsigned char> > volm_fallback_label::fallback_id = create_fallback_label();
std::map<unsigned char, std::vector<float> > volm_fallback_label::fallback_weight = create_fallback_weight();

void volm_fallback_label::print_fallback_table()
{
  for (auto & iter : fallback_id) {
    std::cout << volm_label_table::land_string(iter.first) << " (" << (int)iter.first << "):\t";
    for (unsigned k = 0; k < iter.second.size(); k++) {
      std::cout << volm_label_table::land_string(iter.second[k]) << '(' << (int)iter.second[k] << ", w: " << fallback_weight[iter.first][k] << ")\t";
    }
    std::cout << '\n';
  }
}

std::string volm_label_table::land_string(unsigned char id)
{
  auto mit = volm_label_table::land_id.begin();
  for (; mit != volm_label_table::land_id.end(); ++mit) {
    if ( mit->second.id_ == id ) {
      return mit->second.name_;
    }
  }
  return "invalid";
}

void volm_io_extract_values(unsigned char combined_value, unsigned char& orientation_value, unsigned char& label_value)
{
#if 0
  MOG_TYPE current = aux_args.label_data[data_ptr];
  if (current >= 2)  // 0 is invalid, 1 is horizontal, 2 and up are vertical
    current = 2;     // make all types of vertical 2, so we have 3 values as orientation
  current << 6;      // shift 6 bits to the left, so upper 2 bits are orientation bits
  current += (MOG_TYPE)aux_args.label;   // add the land type, so the lower 6 bits will represent the land type
  aux_args.label_data[data_ptr] = current;
#endif // 0

  orientation_value = combined_value >> 6;
  label_value = combined_value - (orientation_value << 6);
}

bool volm_attributes::contains(const std::string& name)
{
  return name_.find(name) != std::string::npos
     ||  name.find(name_) != std::string::npos;
}

unsigned char volm_label_table::get_id_closest_name(const std::string& name)
{
  for (auto & iter : land_id) {
    if (iter.second.contains(name))
      return iter.first;
  }
  return volm_label_table::INVALID;
}

//: pass the id of the class labeled in the query (volm_attribute.id_)
vil_rgb<vxl_byte> volm_label_table::get_color(unsigned char id)
{
  auto mit = volm_label_table::land_id.begin();
  for (; mit != volm_label_table::land_id.end(); ++mit) {
    if ( mit->second.id_ == id ) {
      return mit->second.color_;
    }
  }
  std::cerr << "cannot find id: " << (int)id << " (and thus color) in the land class table, returning invalid color!\n";
  return {255,0,0}; // default invalid color
}

bool volm_io::read_camera(const std::string& kml_file,
                          unsigned const& ni, unsigned const& nj,
                          double& heading,   double& heading_dev,
                          double& tilt,      double& tilt_dev,
                          double& roll,      double& roll_dev,
                          double& top_fov,   double& top_fov_dev,
                          double& altitude, double& lat, double& lon)
{
  heading_dev = 0;
  tilt_dev = 0;
  roll_dev = 0;
  top_fov_dev = 0;
  heading = 0;
  auto* parser = new bkml_parser();
  std::FILE* xmlFile = std::fopen(kml_file.c_str(), "r");
  if (!xmlFile) {
    std::cerr << kml_file.c_str() << " ERROR in camera kml: can not open the given camera kml file.\n";
    delete parser;
    return false;
  }
  if (!parser->parseFile(xmlFile)) {
    std::cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
             << parser->XML_GetCurrentLineNumber() << '\n';
    delete parser;
    return false;
  }

  lat = 0; lon = 0;
  roll = 0.0;
  if (parser->heading_)       heading = parser->heading_;
  if (parser->heading_dev_)   heading_dev = parser->heading_dev_;
  if (parser->tilt_)          tilt = parser->tilt_;
  if (parser->tilt_dev_)      tilt_dev = parser->tilt_dev_;
  if (parser->roll_)          roll = parser->roll_;
  if (parser->altitude_)      altitude = parser->altitude_;
  if (parser->latitude_)      lat = parser->latitude_;
  if (parser->longitude_)     lon = parser->longitude_;

  double dtor = vnl_math::pi_over_180;
  double ppu = 0.5*ni;
  double ppv = 0.5*nj;
  // check the consistency of input parameters
  if ( parser->right_fov_dev_ && !parser->right_fov_ ) {
    std::cerr << " ERROR in camera kml: deviation of right_fov is defined without given initial right_fov.\n";
    return false;
  }
  if ( parser->top_fov_dev_ && !parser->top_fov_) {
    std::cerr << " ERROR in camera kml: deviation of top_fov is defined without given initial top_fov.\n";
    return false;
  }

  // define the viewing volume
  if ( parser->right_fov_ && parser->top_fov_ ) {  // use averaged value to define top_fov_
    double tr = std::tan(parser->right_fov_*dtor), tt = std::tan(parser->top_fov_*dtor);
    double fr = ppu/tr, ft = ppv/tt;
    double f = 0.5*(fr+ft);
    top_fov = std::atan(0.5*nj/f)/dtor;
  }
  else if ( parser->right_fov_ ) {  // transfer right_fov to top_fov to serve query purpose
    double tr = std::tan(parser->right_fov_*dtor);
    double fr = ni/tr;
    top_fov = std::atan(nj/fr)/dtor;
  }
  else if ( parser->top_fov_ ) {   // use top_fov directly in query
    top_fov = parser->top_fov_;
  }
  // define the deviation of viewing volume
  if ( parser->right_fov_dev_ && parser->top_fov_dev_ ) {
    // If both given, use the one that gives larger focal range
    double trd = std::tan( dtor*(parser->right_fov_ + parser->right_fov_dev_) );
    double ttd = std::tan( dtor*(parser->top_fov_ + parser->top_fov_dev_) );
    double frd = ppu/trd, ftd = ppv/ttd;
    if (frd >= ftd)
      top_fov_dev = parser->top_fov_dev_;
    else
      top_fov_dev = parser->right_fov_dev_;
  }
  else if ( parser->right_fov_dev_ ) {
    // transfer right_fov_dev to top_fov_dev
    double trd = std::tan( dtor*(parser->right_fov_ + parser->right_fov_dev_) );
    top_fov_dev = std::atan(nj*trd/ni)/dtor - top_fov;
  }
  else if ( parser->top_fov_dev_ ) {
    // use top_fov
    top_fov_dev = parser->top_fov_dev_;
  }

  delete parser;
  std::fclose(xmlFile);
  return true;
}

bool volm_io::read_labelme(std::string xml_file, std::string category_file, depth_map_scene_sptr& depth_scene, std::string& img_category)
{
  bvgl_labelme_parser parser(xml_file);
  std::vector<vgl_polygon<double> > polys = parser.polygons();
  std::vector<std::string>& object_names = parser.obj_names();
  std::vector<std::string>& object_types = parser.obj_types();
  std::vector<int>& object_orders = parser.obj_depth_orders();
  std::vector<float>& object_mindist = parser.obj_mindists();
  std::vector<float>& object_maxdist = parser.obj_maxdists();
  if (polys.size() != object_names.size()   ||
      polys.size() != object_types.size()   ||
      polys.size() != object_maxdist.size() ||
      polys.size() != object_mindist.size() ||
      polys.size() != object_orders.size()     ) {
    std::cerr << " ERROR in labelme xml file: imcomplete object properties definition, check object attributes\n";
    return false;
  }
  if (!parser.image_ni() || !parser.image_nj()) {
    std::cerr << " ERROR in labelme xml file: Missing image size information, check <nrows> and <ncols>\n";
    return false;
  }
  if (parser.image_category() == "") {
    std::cerr << " ERROR in labelme xml file: Missing image category information, check <ImageCategory>\n";
    return false;
  }
  // load the image category
  img_category = parser.image_category();
  if ( img_category != "desert" && img_category != "coast" ) {
    std::cout << " image_category is " << img_category << std::endl;
    std::cerr << " WARNING in labelme xml file: undefined img_category found\n";
  }
  // load the image size
  unsigned ni = parser.image_ni();
  unsigned nj = parser.image_nj();
  depth_scene->set_image_size(nj, ni);

  // create depth_map_scene from labelme
  std::map<std::string, volm_category_attribute> category_table;
  volm_category_attribute::read_category(category_table, std::move(category_file));

  for (unsigned i = 0; i < polys.size(); i++) {
    vsol_polygon_2d_sptr poly = bsol_algs::poly_from_vgl(polys[i]);
    double min_dist = object_mindist[i], max_dist = object_maxdist[i];
    unsigned order = object_orders[i];
    std::string type = object_types[i];
    std::string name = object_names[i];
    // sky region
    if (type == "sky")
      depth_scene->add_sky(poly, 255, object_names[i]);
    // ground plane
    else if ( min_dist < 4 && (type == "beach" || type == "water" || type == "flat" || type == "road" || type == "grass" || type == "sand" ||
                               type == "dock" || type == "street") ) {
      unsigned land_id = volm_label_table::land_id[volm_label_table::get_id_closest_name(category_table[type].lnd_)].id_;
      depth_scene->add_ground(poly, 0.0, 0.0, 0, name, land_id);
    }
    else {
      if (category_table[type].is_active_) {
        unsigned land_id = volm_label_table::land_id[volm_label_table::get_id_closest_name(category_table[type].lnd_)].id_;
        std::string orient = category_table[type].ori_;
        vgl_vector_3d<double> np;
        if (orient == "horizontal")          np.set(0.0, 0.0, 1.0);
        else if (orient == "front_parallel") np.set(1.0, 1.0, 0.0);
        else                                 np.set(1.0, 1.0, 1.0);
        depth_scene->add_region(poly, np, min_dist, max_dist, name, volm_orient_table::ori_id[orient], order, land_id);
      }
    }
  }
  return true;
}

bool volm_io::read_query_tags(std::string xml_file,
                              depth_map_scene_sptr& depth_scene,
                              std::vector<volm_weight>& weights,
                              std::string& world_region,
                              unsigned& ni,
                              unsigned& nj,
                              std::string& query_name)
{
  bvgl_labelme_parser parser(xml_file);
  std::vector<vgl_polygon<double> > polys = parser.polygons();
  std::vector<vgl_point_2d<double> > points = parser.pixels();
  std::vector<std::string>& object_names = parser.obj_names();
  std::vector<std::string>& object_land_types = parser.obj_land_categories();
  std::vector<float>& object_mindist = parser.obj_mindists();
  std::vector<float>& object_maxdist = parser.obj_maxdists();
  std::vector<float>& object_weight =  parser.obj_weights();

  unsigned num_objs = object_names.size();
  if (num_objs != object_land_types.size() || num_objs != object_mindist.size() ||
      num_objs != object_maxdist.size()    || num_objs != object_weight.size())
  {
    std::cerr << "ERROR parsing tag file " << xml_file << ": incomplete object properties definition, check tags in xml file\n";
    return false;
  }
  if (!parser.image_ni() || !parser.image_nj()) {
    std::cerr << "ERROR parsing tag file " << xml_file  << ": missing image size information, check <nrows> and <ncols>\n";
    return false;
  }

  // insert empty polygon if no geometry in tag file
  if (polys.empty()) {
    for (unsigned i = 0; i < num_objs; i++) {
      vgl_polygon<double> poly;
      poly.new_sheet();
      poly.push_back(points[i].x(),   points[i].y());
      poly.push_back(points[i].x()+1, points[i].y());
      poly.push_back(points[i].x()+1, points[i].y()+1);
      poly.push_back(points[i].x(),   points[i].y()+1);
      polys.push_back(poly);
    }
  }

  // parse the world region
  world_region = parser.region();
  // parse the query name
  query_name = parser.image_name();

  // load the image dimension
  ni = parser.image_ni();
  nj = parser.image_nj();
  depth_scene->set_image_size(ni,nj);

  weights.clear();
  // create depth_map scene from loaded tag (here we treat all labeled object as non-sky, non-ground object)
  for (unsigned i = 0; i < num_objs; i++) {
    std::string land_type = object_land_types[i];
    if (volm_osm_category_io::tag_to_volm_land_table.find(land_type) == volm_osm_category_io::tag_to_volm_land_table.end())
      continue;
    vsol_polygon_2d_sptr poly = bsol_algs::poly_from_vgl(polys[i]);
    vgl_vector_3d<double> np; // surface normal
    np.set(1.0,1.0,1.0);
    depth_scene->add_region(poly, np, object_mindist[i], object_maxdist[i], object_names[i],
                            depth_map_region::FRONT_PARALLEL, 0,
                            volm_osm_category_io::tag_to_volm_land_table.find(land_type)->second.id_);
    // add the weight
    weights.emplace_back(object_names[i], object_names[i], 0.0, 0.0, 0.0, 0.0, object_weight[i]);
  }
  return true;
}

bool volm_io::read_conf_query_tags(std::string xml_file,
                                  float const& floor_height,
                                  depth_map_scene_sptr& depth_scene,
                                  std::string& world_region,
                                  unsigned& img_ni, unsigned& img_nj,
                                  std::string& query_name)
{
  if (!vul_file::exists(xml_file))
    return false;
  bvgl_labelme_parser parser(xml_file);
  std::vector<vgl_point_2d<double> > points = parser.pixels(); // <point> tag
  std::vector<std::string>& object_names = parser.obj_names();  // <name> tag
  std::vector<std::string>& object_land_types = parser.obj_land_categories();  // <land> tag
  std::vector<float>& object_mindist = parser.obj_mindists();
  std::vector<float>& object_maxdist = parser.obj_maxdists();
  std::vector<int>& object_heights = parser.obj_heights();
  std::vector<bool>& object_references = parser.obj_references();
  // check the consistency
  unsigned num_objs = object_names.size();
  if (num_objs != points.size() || num_objs != object_land_types.size() || num_objs != object_references.size() ||
      num_objs != object_mindist.size() || num_objs != object_maxdist.size() || num_objs != object_heights.size() )
  {
    std::cerr << "ERROR parsing tag file " << xml_file << ": incomplete object properties definition, check tags in xml file\n";
    return false;
  }
  if (!parser.image_ni() || !parser.image_nj())
  {
    std::cerr << "ERROR parsing tag file " << xml_file << ": missing image size or image name, check <nrows>, <ncols> or <filename>\n";
    return false;
  }

  if (parser.image_name().compare("") == 0)
  {
    std::cerr << "ERROR parsing tag file " << xml_file << ": missing image name tag <filename>\n";
    return false;
  }
  if (parser.region().compare("") == 0)
  {
    std::cerr << "ERROR parsing tag file " << xml_file << ": missing <region> tag to specify the world region\n";
    return false;
  }

  // create polygon from points -- expands to 4 points
  std::vector<vgl_polygon<double> > polys;
  for (unsigned i = 0; i < num_objs; i++)
  {
    vgl_polygon<double> poly;
    poly.new_sheet();
    poly.push_back(points[i].x(), points[i].y());
    poly.push_back(points[i].x(), points[i].y()+1);
    poly.push_back(points[i].x()+1, points[i].y()+1);
    poly.push_back(points[i].x()+1, points[i].y());
    polys.push_back(poly);
  }

  // parse the world region
  world_region = parser.region();
  // parse the query name
  std::string filename = parser.image_name();
  query_name = vul_file::strip_extension(filename);
  // parser the image dimension
  img_ni = parser.image_ni();
  img_nj = parser.image_nj();

  // construct the depth map scene
  depth_scene->set_image_size(img_ni, img_nj);
  for (unsigned i = 0; i < num_objs; i++)
  {
    std::string land_type = object_land_types[i];
    if (volm_osm_category_io::tag_to_volm_land_table.find(land_type) == volm_osm_category_io::tag_to_volm_land_table.end())
      continue;
    if (volm_osm_category_io::tag_to_volm_land_table.find(land_type)->second.name_ == "invalid")
      continue;
    // calculate the height value
    double height = -1.0;
    if (object_heights[i] != -1 && object_heights[i] != 0)
      height = object_heights[i] * floor_height;

    vsol_polygon_2d_sptr poly = bsol_algs::poly_from_vgl(polys[i]);
    vgl_vector_3d<double> np;  // surface normal
    np.set(1.0, 1.0, 1.0);
    // define orientation from height value
    if (height == -1.0)
      depth_scene->add_region(poly, np, object_mindist[i], object_maxdist[i], object_names[i],
                              depth_map_region::HORIZONTAL, 1,
                              volm_osm_category_io::tag_to_volm_land_table.find(land_type)->second.id_,
                              height, object_references[i]);
    else
      depth_scene->add_region(poly, np, object_mindist[i], object_maxdist[i], object_names[i],
                              depth_map_region::FRONT_PARALLEL, 1,
                              volm_osm_category_io::tag_to_volm_land_table.find(land_type)->second.id_,
                              height, object_references[i]);
  }
  return true;

}

bool volm_io::write_status(const std::string& out_folder, int status_code, int percent, const std::string& log_message, const std::string& status_file)
{
  std::ofstream file;
  std::string out_file = out_folder + "/" + status_file;
  file.open (out_file.c_str());
  file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<status>\n";
  switch (status_code) {
    case volm_io::CAM_FILE_IO_ERROR:
      file << "Camera FILE IO Error\n<percent>100</percent>\n"; break;
    case volm_io::EXE_ARGUMENT_ERROR:
      file << "Error in executable arguments\n<percent>100</percent>\n"; break;
    case volm_io::SUCCESS:
      file << "Post Process Completed Successfully, rendering the rationale image\n<percent>95</percent>\n"; break;
    case volm_io::MATCHER_EXE_STARTED:
      file << "Matcher Exe Started\n<percent>0</percent>\n"; break;
    case volm_io::MATCHER_EXE_FINISHED:
      file << "Matcher Exe Finished, composer starting..\n<percent>90</percent>\n"; break;
    case volm_io::COMPOSE_STARTED:
      file << "Composing output tiles\n<percent>90</percent>\n"; break;
    case volm_io::EXE_RUNNING:
      file << "Matcher Exe Running\n<percent>\n" << percent << "\n</percent>\n"; break;
    case volm_io::LABELME_FILE_IO_ERROR:
      file << "LABELME FILE IO Error\n<percent>100</percent>\n"; break;
    case volm_io::COMPOSE_HALT:
      file << "COMPOSER waiting for matcher to complete\n<percent>90</percent>\n"; break;
    case volm_io::EXE_STARTED:
      file << "PREP exe starterd\n<percent>0</percent>\n"; break;
    case volm_io::PRE_PROCESS_STARTED :
      file << "PREP exe started\n<percent>0</percent>\n"; break;
    case volm_io::PRE_PROCESS_FAILED :
      file << "PREP exe failed\n<percent>100</percent>\n"; break;
    case volm_io::PRE_PROCESS_FINISHED :
      file << "PREP exe finished\n<percent>30</percent>\n"; break;
    default:
      file << "Unidentified status code!\n";
      std::cerr << "Unidentified status code!\n";
      break;
  }
  file << "<code>\n" << status_code << "\n</code>\n"
       << "<log>\n" << log_message << "\n</log>\n"
       << "</status>\n";
  file.close();
  return true;
}

//: return true if MATCHER_EXE_FINISHED, otherwise return false
bool volm_io::check_matcher_status(const std::string& out_folder)
{
  std::ifstream ifs;
  std::string file = out_folder + "/status.xml";
  ifs.open(file.c_str());
  char buf[10000];
  ifs.getline(buf, 10000);
  ifs.close();
  std::stringstream str(buf);
  std::string dummy;
  str >> dummy;
  while (dummy.compare("<code>") != 0)
    str >> dummy;
  int status_code;
  str >> status_code;
  return status_code == volm_io::MATCHER_EXE_FINISHED;
}


bool volm_io::write_log(const std::string& out_folder, const std::string& log)
{
  std::ofstream file;
  std::string out_file = out_folder + "/log.xml";
  file.open (out_file.c_str());
  file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<status>\n"
       << "<log>\n"<<log<<"</log>\n";
  file.close();
  return true;
}

bool volm_io::write_composer_log(const std::string& out_folder, const std::string& log)
{
  std::ofstream file;
  std::string out_file = out_folder + "/composer_log.xml";
  file.open (out_file.c_str());
  file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<status>\n"
       << "<log>\n"<<log<<"</log>\n";
  file.close();
  return true;
}

bool volm_io::write_post_processing_log(const std::string& log_file, const std::string& log)
{
  std::ofstream file;
  file.open(log_file.c_str());
  file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<status>\n"
       << "<log>\n"<<log<<"</log>\n</status>";
  file.close();
  return true;
}

bool volm_io::write_error_log(std::string const& log_file, std::string const& log)
{
  std::cerr << log;  volm_io::write_post_processing_log(log_file, log);
  return true;
}

//: piecewise linear s.t. [1,127) -> [0,t), [127,255] -> [t,1]
float volm_io::scale_score_to_0_1(unsigned char pix_value, float threshold)
{
  if (pix_value < 127)
    return ((float)pix_value/127)*threshold;
  else
    return ((float)(pix_value-127)/128)*(1-threshold) + threshold;
}

//: piecewise linear s.t. [0,t) -> [1,127), [t,1] -> [127,255]"
unsigned char volm_io::scale_score_to_1_255(float threshold, float score)
{
  if (score < threshold)
    return (unsigned char) ((score/threshold)*127);
  else if ( (score-1)*(score-1)<1E-8 )
    return (unsigned char) 255;
  else
    return (unsigned char) (((score-threshold)/(1-threshold))*128 + 127);
}

unsigned char volm_io::scale_score_to_1_255_sig(float const& kl, float const &  /*ku*/, float const& threshold, float const& score)
{
  float t = threshold;
  float x;
  if (score < t) {
    x = -1*kl*(score-threshold);
    return 1 + (unsigned char)(253.999f / ( 1+std::exp(x)));
  }
  else
    return  volm_io::scale_score_to_1_255(threshold, score);
}

float volm_io::scale_score_to_0_1_sig(float const& kl, float const&  /*ku*/, float const& threshold, unsigned char pix_value)
{
  if (pix_value < 127)
    if (pix_value == 1)
      return 0.0f;
    else
      return (threshold - std::log(253.999/(pix_value-1)-1)/kl);
  else {
    return volm_io::scale_score_to_0_1(pix_value, threshold);
  }
}

//: piecewise linear s.t. [0,t) -> [1,63), [t,1] -> [63,127]"
unsigned char volm_io::scale_score_to_1_127(float threshold, float score)
{
  if (score < threshold)
    return (unsigned char) ((score/threshold)*63);
  else
    return (unsigned char) (((score-threshold)/(1-threshold))*64 + 63);
}

bool operator>(const std::pair<float, volm_rationale>& a, const std::pair<float, volm_rationale>& b)
{  return a.first>b.first; }

bool volm_rationale::write_top_matches(std::multiset<std::pair<float, volm_rationale>, std::greater<std::pair<float, volm_rationale> > >& top_matches, std::string& filename)
{
  std::ofstream ofs(filename.c_str());
  if (!ofs.is_open()) {
    std::cerr << " cannot open: " << filename << " for write!\n";
    return false;
  }
  std::multiset<std::pair<float, volm_rationale>, std::greater<std::pair<float, volm_rationale> > >::iterator iter;
  ofs << top_matches.size() << std::endl;
  for (iter = top_matches.begin(); iter != top_matches.end(); iter++) {
    ofs << iter->first
        << ' ' << iter->second.lat
        << ' ' << iter->second.lon
        << ' ' << iter->second.elev
        << ' ' << iter->second.index_id
        << ' ' << iter->second.cam_id << '\n'
        << iter->second.index_file << '\n'
        << iter->second.score_file << std::endl;
  }
  ofs.close();
  return true;
}

bool volm_rationale::read_top_matches(std::multiset<std::pair<float, volm_rationale>, std::greater<std::pair<float, volm_rationale> > >& top_matches, std::string& filename)
{
  std::ifstream ifs(filename.c_str());
  if (!ifs.is_open()) {
    std::cerr << " cannot open: " << filename << '\n';
    return false;
  }
  unsigned cnt = 0;
  ifs >> cnt;
  for (unsigned i = 0; i < cnt; i++) {
    float score;
    ifs >> score;
    volm_rationale r;
    ifs >> r.lat >> r.lon >> r.elev >> r.index_id >> r.cam_id;
    ifs >> r.index_file >> r.score_file;
    top_matches.insert(std::pair<float, volm_rationale>(score, r));
  }
  ifs.close();
  return true;
}

// x is lon, y is lat
void volm_io::read_polygons(const std::string& poly_file, vgl_polygon<double>& out)
{
  std::ifstream ifs(poly_file.c_str());
  unsigned np, nvert;
  double x,y;

  ifs >> np;
  for (unsigned i = 0; i < np; i++) {
    ifs >> nvert;
    if (nvert < 3) {
      for (unsigned j = 0; j < nvert; j++) {
        ifs >> x; ifs >> y;
      }
      continue; // do not insert as a sheet
    }
    out.new_sheet();

    for (unsigned j = 0; j < nvert; j++) {
      ifs >> x; ifs >> y;
      vgl_point_2d<double> pt(x,y);
      out[i].push_back(pt);
    }
    unsigned cnt = 0;
    for (int j = (int)nvert-1; j > 0; j--)
      if (out[i][j] == out[i][0])
        cnt++;
    if (cnt > 0) {
      out[i].erase(out[i].begin()+nvert-cnt, out[i].end());
    }
  }
  assert(np == out.num_sheets());
}

void volm_io::convert_polygons(vgl_polygon<double> const& in, vgl_polygon<float>& out)
{
  // convert poly
  for (unsigned i = 0; i < in.num_sheets(); i++)
    out.new_sheet();

  for (unsigned i = 0; i < in.num_sheets(); i++) {
    for (unsigned j = 0; j < in[i].size(); j++) {
      vgl_point_2d<float> pt((float)in[i][j].x(), (float)in[i][j].y());
      out[i].push_back(pt);
    }
  }
}

void volm_io::convert_polygons(vgl_polygon<float> const& in, vgl_polygon<double>& out)
{
  // convert polygon from type float to type double
  for (unsigned i = 0; i < in.num_sheets(); i++)
    out.new_sheet();

  for (unsigned i = 0; i < in.num_sheets(); i++) {
    for (unsigned j = 0; j < in[i].size(); j++) {
      vgl_point_2d<double> pt((double)in[i][j].x(), (double)in[i][j].y());
      out[i].push_back(pt);
    }
  }
}

//: binary IO write
void volm_score::b_write(vsl_b_ostream& os)
{
  unsigned ver = this->version();
  vsl_b_write(os, ver);
  vsl_b_write(os, leaf_id_);
  vsl_b_write(os, hypo_id_);
  vsl_b_write(os, max_score_);
  vsl_b_write(os, max_cam_id_);
  vsl_b_write(os, cam_id_);
}

//: binary IO read
void volm_score::b_read(vsl_b_istream& is)
{
  unsigned ver;
  vsl_b_read(is, ver);
  if (ver == 1) {
    vsl_b_read(is, leaf_id_);
    vsl_b_read(is, hypo_id_);
    vsl_b_read(is, max_score_);
    vsl_b_read(is, max_cam_id_);
    vsl_b_read(is, cam_id_);
  }
  else
  {
    std::cerr << "I/O ERROR: volm_score::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }
}

int volm_io::read_gt_file(const std::string& gt_file, std::vector<std::pair<vgl_point_3d<double>, std::pair<std::pair<std::string, int>, std::string> > >& samples)
{
  std::ifstream ifs(gt_file.c_str());
  int cnt; ifs >> cnt;
  for (int j = 0; j < cnt; j++) {
    std::string name; ifs >> name; std::string type;
    char name_buf[1000];
    for (unsigned kk = 0; kk < name.size(); kk++)
      name_buf[kk] = name[kk];

    char *tok = std::strtok(name_buf, "-");
    char *tok2 = std::strtok(tok, "_");
    tok2 = std::strtok(nullptr, "_"); // tokenize the remaining string
    int img_id;
    std::stringstream tv(tok2); tv >> img_id;

    double lat, lon, elev;
    ifs >> lat; ifs >> lon; ifs >> elev;
    ifs >> type;
    vgl_point_3d<double> pt(lon, lat, elev);
    std::pair<std::string, int> np(name, img_id);
    std::pair<std::pair<std::string, int>, std::string> p(np, type);
    samples.emplace_back(pt,  p );
  }
  ifs.close();
  return cnt;
}

void volm_score::write_scores(std::vector<volm_score_sptr>& scores, std::string const& file_name)
{
  vsl_b_ofstream ofs(file_name);
  vsl_b_write(ofs, (unsigned)(scores.size()));
  for (auto & score : scores)
    score->b_write(ofs);
  ofs.close();
}

void volm_score::read_scores(std::vector<volm_score_sptr>& scores, std::string const& file_name)
{
  vsl_b_ifstream ifs(file_name);
  unsigned size;
  vsl_b_read(ifs, size);
  scores.clear();
  for (unsigned i = 0; i < size; i++) {
    volm_score_sptr s = new volm_score;
    s->b_read(ifs);
    scores.push_back(s);
  }
  ifs.close();
}

void volm_weight::read_weight(std::vector<volm_weight>& weights, std::string const& file_name)
{
  std::ifstream ifs(file_name.c_str());
  std::string dummy;
  std::getline(ifs, dummy); std::getline(ifs, dummy); std::getline(ifs, dummy); std::getline(ifs, dummy);

  std::string w_typ;
  std::string w_name;
  float w_ori, w_lnd, w_dst, w_ord, w_obj;
  while (true) {
    ifs >> w_name;
    ifs >> w_typ;
    ifs >> w_ori;
    ifs >> w_lnd;
    ifs >> w_dst;
    ifs >> w_ord;
    ifs >> w_obj;
    weights.emplace_back(w_name, w_typ, w_ori, w_lnd, w_ord, w_dst, w_obj);
    if (ifs.eof()) break;
  }
  ifs.close();
}

bool volm_weight::check_weight(std::vector<volm_weight> const& /*weight*/)
{
  return true;
}

void volm_weight::equal_weight(std::vector<volm_weight>& weights, const depth_map_scene_sptr& dms)
{
  float w_avg;
  float w_obj;
  if (!dms->sky().empty() && !dms->ground_plane().empty()) {
    w_avg = 1.0f / (2 + dms->scene_regions().size());
    float w_sky = w_avg * 1.5f;
    float w_grd = w_avg * 1.0f;
    w_obj = (1.0f - w_sky - w_grd) / dms->scene_regions().size();
    weights.emplace_back("sky", "sky", 0.0f, 0.0f, 0.0f, 1.0f, w_sky);
    weights.emplace_back("ground_plane", "ground_plane", 0.3f, 0.4f, 0.0f, 0.3f, w_grd);
  }
  else if (!dms->sky().empty()) {
    w_avg = 1.0f / (1 + dms->scene_regions().size());
    float w_sky = w_avg * 1.5f;
    w_obj = (1.0f - w_sky) / dms->scene_regions().size();
    weights.emplace_back("sky", "sky", 0.0f, 0.0f, 0.0f, 1.0f, w_sky);
  }
  else if (!dms->ground_plane().empty()) {
    w_avg = 1.0f / (1 + dms->scene_regions().size());
    float w_grd = w_avg * 1.0f;
    w_obj = (1.0f - w_grd) / dms->scene_regions().size();
    weights.emplace_back("ground_plane", "ground_plane", 0.3f, 0.4f, 0.0f, 0.3f, w_grd);
  }
  else {
    w_avg = 1.0f / dms->scene_regions().size();
    w_obj = w_avg;
  }
  for (unsigned i = 0; i < dms->scene_regions().size(); i++) {
      weights.emplace_back(dms->scene_regions()[i]->name(), dms->scene_regions()[i]->name(), 0.25f, 0.25f, 0.25f, 0.25f, w_obj);
  }

#if 0
  // create a set of equal weight parameters
  float w_obj;
  if (!dms->sky().empty() && !dms->ground_plane().empty()) {
    unsigned tot_num_obj = 2 + dms->scene_regions().size();
    w_obj = 1.0f/tot_num_obj;
    weights.push_back(volm_weight("sky", 0.0f, 0.0f, 0.0f, 1.0f, w_obj));
    weights.push_back(volm_weight("ground", 0.3333f, 0.3333f, 0.0f, 0.3333f, w_obj));
  }
  else if (!dms->sky().empty()) {
    unsigned tot_num_obj = 1 + dms->scene_regions().size();
    w_obj = 1.0f/tot_num_obj;
    weights.push_back(volm_weight("sky", 0.0f, 0.0f, 0.0f, 1.0f, w_obj));
  }
  else if (!dms->ground_plane().empty()) {
    unsigned tot_num_obj = 1 + dms->scene_regions().size();
    w_obj = 1.0f/tot_num_obj;
    weights.push_back(volm_weight("ground", 0.3333f, 0.3333f, 0.0f, 0.3333f, w_obj));
  }

  std::vector<depth_map_region_sptr> obj_reg = dms->scene_regions();
  for ( std::vector<depth_map_region_sptr>::iterator vit = obj_reg.begin(); vit != obj_reg.end(); ++vit)
    weights.push_back(volm_weight("object", 0.25f, 0.25f, 0.25f, 0.25f, w_obj));
#endif
}

void volm_io_expt_params::read_params(std::string const& params_file)
{
  std::ifstream ifs(params_file.c_str());
  std::string dummy;
  ifs >> dummy; ifs >> fov_inc;     std::cout << dummy << ' ' << fov_inc << ' ';
  ifs >> dummy; ifs >> tilt_inc;    std::cout << dummy << ' ' << tilt_inc << ' ';
  ifs >> dummy; ifs >> roll_inc;    std::cout << dummy << ' ' << roll_inc << ' ';
  ifs >> dummy; ifs >> head_inc;    std::cout << dummy << ' ' << head_inc << ' ';
  ifs >> dummy; ifs >> vmin;        std::cout << dummy << ' ' << vmin << ' ';
  ifs >> dummy; ifs >> solid_angle; std::cout << dummy << ' ' << solid_angle << ' ';
  ifs >> dummy; ifs >> dmax;        std::cout << dummy << ' ' << dmax << ' ';
  ifs >> dummy; ifs >> cap_angle;   std::cout << dummy << ' ' << cap_angle << ' ';
  ifs >> dummy; ifs >> point_angle; std::cout << dummy << ' ' << point_angle << ' ';
  ifs >> dummy; ifs >> top_angle;   std::cout << dummy << ' ' << top_angle << ' ';
  ifs >> dummy; ifs >> bottom_angle;std::cout << dummy << ' ' << bottom_angle << '\n';
  ifs.close();
}

void volm_io_expt_params::read_cam_inc_params(std::string const& params_file)
{
  std::ifstream ifs(params_file.c_str());
  std::string dummy;
  ifs >> dummy; ifs >> fov_inc;     std::cout << dummy << ' ' << fov_inc << ' ';
  ifs >> dummy; ifs >> tilt_inc;    std::cout << dummy << ' ' << tilt_inc << ' ';
  ifs >> dummy; ifs >> roll_inc;    std::cout << dummy << ' ' << roll_inc << ' ';
  ifs >> dummy; ifs >> head_inc;    std::cout << dummy << ' ' << head_inc << '\n';
  ifs.close();
}

bool volm_io::read_ray_index_data(const std::string& path, std::vector<unsigned char>& data)
{
  std::ifstream is(path.c_str());
  if (!is.is_open())
    return false;
  int nrays;
  is >> nrays;
  if (nrays <= 0)
    return false;
  data.resize(nrays);
  for (int i = 0; i< nrays; ++i)
    is >> data[i];
  return true;
}

//: read the building footprint file
bool volm_io::read_building_file(const std::string& file, std::vector<std::pair<vgl_polygon<double>, vgl_point_2d<double> > >& builds, std::vector<double>& heights)
{
  std::cout << "\t\t !!!!!!!!!!!!!! reading file: " << file << std::endl;
  std::ifstream ifs(file.c_str());
  if (!ifs.is_open())
    return false;
  while (!ifs.eof()) {
    // each line is one building
    double height, volume, area, confidence, cent_lon, cent_lat, lon, lat;
    char buffer[100000];
    ifs.getline(buffer, 100000);
    std::string temp_buf(buffer);
    if (ifs.eof()) break;
    char *tok = std::strtok(buffer, ",");
    std::stringstream th(tok); th >> height;

    tok = std::strtok(nullptr, ","); // tokenize the remaining string
    std::stringstream tv(tok); tv >> volume;

    tok = std::strtok(nullptr, ",");
    std::stringstream ta(tok); ta >> area;

    tok = std::strtok(nullptr, ",");
    std::stringstream tc(tok); tc >> confidence;

    tok = std::strtok(nullptr, ",");
    std::stringstream tcl(tok); tcl >> cent_lon;

    tok = std::strtok(nullptr, ",");
    std::stringstream tcla(tok); tcla >> cent_lat;

    vgl_point_2d<double> cent_pt(cent_lon, cent_lat);

    vgl_polygon<double> poly(1);
    tok = std::strtok(nullptr, ",");
    while (tok != nullptr) {
      std::stringstream tl(tok);
      tl >> lon;

      tok = std::strtok(nullptr, ",");
      std::stringstream tlat(tok);
      tlat >> lat;

      vgl_point_2d<double> pt(lon, lat);  // lon is x
      poly[0].push_back(pt);
      tok = std::strtok(nullptr, ",");
    }
    // remove the duplicated points inside polygon
    vgl_polygon<double> prune_poly(1);
    prune_poly[0].push_back(poly[0][0]);
    for (unsigned i = 1; i < poly[0].size(); i++)
      if (poly[0][i] != poly[0][i-1])
        prune_poly[0].push_back(poly[0][i]);
    builds.emplace_back(prune_poly, cent_pt);
    heights.push_back(height);
  }
  return true;
}

//: read the sme labels
bool volm_io::read_sme_file(const std::string& file, std::vector<std::pair<vgl_point_2d<double>, int> >& objects)
{
  std::cout << "\t\t !!!!!!!!!!!!!! reading file: " << file << std::endl;
  std::ifstream ifs(file.c_str());
  if (!ifs.is_open())
    return false;

  while (!ifs.eof()) {
    // each line is one object
    std::string name, type;
    double lon, lat;
    char buffer[100000];
    ifs.getline(buffer, 100000);
    std::string temp_buf(buffer);
    if (ifs.eof()) break;

    char *tok = std::strtok(buffer, ",");
    std::stringstream th(tok); th >> name;

    tok = std::strtok(nullptr, ","); // tokenize the remaining string
    std::stringstream tv(tok); tv >> lon;

    tok = std::strtok(nullptr, ",");
    std::stringstream ta(tok); ta >> lat;

    tok = std::strtok(nullptr, ",");
    std::stringstream tc(tok); tc >> type;

    vgl_point_2d<double> pt(lon, lat);

    // find its label code
    int label = 0;
    auto mit = volm_osm_category_io::volm_land_table_name.find(type);
    if (mit != volm_osm_category_io::volm_land_table_name.end())
      label = mit->second.id_;
#if 0
    for (std::map<int, volm_attributes >::iterator iter = volm_label_table::land_id.begin();
         iter != volm_label_table::land_id.end(); ++iter)
      if (iter->second.name_.compare(type) == 0)
        label = iter->first;
#endif

    objects.emplace_back(pt, label);
  }
  return true;
}

bool volm_io::write_sme_kml(const std::string& file, std::vector<std::pair<vgl_point_2d<double>, int> >& objects)
{
  std::ofstream ofs(file.c_str());
  bkml_write::open_document(ofs);
  for (unsigned i = 0; i < objects.size(); i++) {
    std::stringstream ids; ids << i;
    std::string name = volm_label_table::land_id[objects[i].second].name_;
    bkml_write::write_location(ofs, name, ids.str(), objects[i].first.y(), objects[i].first.x(), 0.0);
  }
  bkml_write::close_document(ofs);
  return true;
}

bool volm_io::write_sme_kml_type(const std::string& file, const std::string& type_name, std::vector<std::pair<vgl_point_2d<double>, int> >& objects)
{
  std::ofstream ofs(file.c_str());
  bkml_write::open_document(ofs);
  for (unsigned i = 0; i < objects.size(); i++) {
    std::string name = volm_label_table::land_id[objects[i].second].name_;
    if (name.compare(type_name) != 0)
      continue;
    std::stringstream ids; ids << i;
    bkml_write::write_location(ofs, name, ids.str(), objects[i].first.y(), objects[i].first.x(), 0.0);
  }
  bkml_write::close_document(ofs);
  return true;
}

bool volm_io::read_dem_peak_file(std::string const& file, std::vector<std::pair<vgl_point_2d<double>, double> >& objects)
{
  if (!vul_file::exists(file)) {
    std::cerr << "In volm_io::read_dem_peak_file: can not find input file: " << file << "!\n";
    return false;
  }
  std::ifstream ifs(file.c_str());
  if (!ifs.is_open())
    return false;

  while (!ifs.eof()) {
    double lon, lat, height;
    char buffer[10000];
    ifs.getline(buffer, 10000);
    std::string temp_buf(buffer);
    if (ifs.eof()) break;
    char *tok = std::strtok(buffer, " ");
    std::stringstream tlat(tok);  tlat >> lat;

    tok = std::strtok(nullptr, " ");
    std::stringstream tlon(tok);  tlon >> lon;

    tok = std::strtok(nullptr, " ");
    std::stringstream theight(tok);  theight >> height;

    vgl_point_2d<double> peak_loc(lon, lat);
    objects.emplace_back(peak_loc, height);
  }
  return true;
}
