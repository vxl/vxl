#include "volm_tile.h"
#include "volm_io.h"
//:
// \file
#include <vcl_iomanip.h>
#include <vcl_sstream.h>
#include <vcl_cassert.h>

#include <brip/brip_vil_float_ops.h>
#include <bkml/bkml_write.h>
#include <vpgl/vpgl_utm.h>
#include <vul/vul_file.h>

//: specify lat lon in positive numbers but specify hemisphere ('N' or 'S') and direction ('W' or 'E')
volm_tile::volm_tile(float lat, float lon, char hemisphere, char direction, float scale_i, float scale_j, unsigned ni, unsigned nj) :
        lat_(lat), lon_(lon), hemisphere_(hemisphere), direction_(direction), scale_i_(scale_i), scale_j_(scale_j), ni_(ni), nj_(nj)
{
  vnl_matrix<double> trans_matrix(4,4,0.0);
  //divide by ni-1 to account for 1 pixel overlap with the next tile
  if (direction_ == 'E') {
    trans_matrix[0][3] = lon_ - 0.5/(ni-1.0);
    trans_matrix[0][0] = scale_i_/(ni-1.0);
  }
  else {
    trans_matrix[0][3] = lon_ + 0.5/(ni-1.0);
    trans_matrix[0][0] = -scale_i_/(ni-1.0);
  }
  if (hemisphere_ == 'N') {
    trans_matrix[1][1] = -scale_j_/(nj-1.0);
    trans_matrix[1][3] = lat_+scale_j_+0.5/(nj-1.0);
  }
  else {
    trans_matrix[1][1] = scale_j_/(nj-1.0);
    trans_matrix[1][3] = lat_-scale_j_-0.5/(nj-1.0);
  }
  // just pass an empty lvcs, this geo cam will only be used to compute image pixels to global coords mappings
  vpgl_lvcs_sptr lv = new vpgl_lvcs;
  vpgl_geo_camera cam(trans_matrix, lv); cam.set_scale_format(true);
  cam_ = cam;
}

//: parse the name string and construct tile
volm_tile::volm_tile(vcl_string file_name, unsigned ni, unsigned nj) : ni_(ni), nj_(nj)
{
  vcl_string name = vul_file::strip_directory(file_name);
  name = name.substr(name.find_first_of('_')+1, name.size());
  
  vcl_string n_coords = name.substr(0, name.find_first_of('_'));
  vcl_string n_scale = name.substr(name.find_first_of('_')+1, name.find_last_of('_')-name.find_first_of('_')-1);

  // determine the lat, lon, hemisphere (North or South) and direction (East or West)
  vcl_size_t n = n_coords.find("N");
  if (n < n_coords.size())  hemisphere_ = 'N';
  else                      hemisphere_ = 'S';
  n = n_coords.find("E");
  if (n < n_coords.size())  direction_ = 'E';
  else                      direction_ = 'W';

  vcl_string n_str = n_coords.substr(n_coords.find_first_of(hemisphere_)+1,
                                     n_coords.find_first_of(direction_)-n_coords.find_first_of(hemisphere_)-1);
  vcl_stringstream str(n_str);  str >> lat_;

  n_str = n_coords.substr(n_coords.find_first_of(direction_)+1, n_coords.size());
  vcl_stringstream str2(n_str);  str2 >> lon_;

  n_str = n_scale.substr(n_scale.find_first_of('S')+1, n_scale.find_first_of('x')-n_scale.find_first_of('S')-1);
  vcl_stringstream str3(n_str);  str3 >> scale_i_;  scale_j_ = scale_i_;

#if 1
  if (hemisphere_ == 'N')  vcl_cout << " upper left corner in the image is: " << hemisphere_ << lat_+scale_i_ << direction_ << lon_ << vcl_endl;
  else                    vcl_cout << " upper left corner in the image is: " << hemisphere_ << lat_-scale_i_ << direction_ << lon_ << vcl_endl;
  if (direction_ == 'W')   vcl_cout << " lower right corner in the image is: " << hemisphere_ << lat_ << direction_ << lon_-scale_j_ << vcl_endl;
  else                    vcl_cout << " lower right corner in the image is: " << hemisphere_ << lat_ << direction_ << lon_+scale_j_ << vcl_endl;
#endif
  vnl_matrix<double> trans_matrix(4,4,0.0);
  //divide by ni-1 to account for 1 pixel overlap with the next tile
  if (direction_ == 'E') {
    trans_matrix[0][3] = lon_ - 0.5/(ni-1.0);
    trans_matrix[0][0] = scale_i_/(ni-1.0);
  }
  else {
    trans_matrix[0][3] = lon_ + 0.5/(ni-1.0);
    trans_matrix[0][0] = -scale_i_/(ni-1.0);
  }
  if (hemisphere_ == 'N') {
    trans_matrix[1][1] = -scale_j_/(nj-1.0);
    trans_matrix[1][3] = lat_+scale_j_+0.5/(nj-1.0);
  }
  else {
    trans_matrix[1][1] = scale_j_/(nj-1.0);
    trans_matrix[1][3] = lat_-scale_j_-0.5/(nj-1.0);
  }
  vpgl_lvcs_sptr dummy_lvcs = new vpgl_lvcs;
  cam_ = vpgl_geo_camera(trans_matrix, dummy_lvcs);
  cam_.set_scale_format(true);

#if 0
#ifdef DEBUG
  vcl_cout << "will determine transformation matrix from the file name: " << name << vcl_endl;
#endif
  vcl_string n = name.substr(name.find_first_of('N')+1, name.find_first_of('W'));
  assert(n.size() != 0);  // for now there is no support for 'S' and/or 'E'
  hemisphere_ = 'N';
  direction_ = 'W';

  vcl_stringstream str(n); str >> lat_;
  n = name.substr(name.find_first_of('W')+1, name.find_first_of('_'));
  vcl_stringstream str2(n); str2 >> lon_;

  name = name.substr(name.find_first_of('_'), name.size());
  n = name.substr(name.find_first_of('S')+1, name.find_first_of('x'));
  vcl_stringstream str3(n); str3 >> scale_i_;
  n = name.substr(name.find_first_of('x')+1, name.find_last_of('.'));
  vcl_stringstream str4(n); str4 >> scale_j_;
#ifdef DEBUG
  vcl_cout << " lat: " << lat_ << " lon: " << lon_ << " scale_i:" << scale_i_ << " scale_j: " << scale_j_ << vcl_endl;

  // determine the upper left corner to use a vpgl_geo_cam, subtract from lat
  vcl_cout << "upper left corner in the image is: " << lat_+scale_j_ << " N " << lon_ << " W\n"
           << "lower right corner in the image is: " << lat_ << " N " << lon_-scale_i_ << " W" << vcl_endl;
#endif
  vnl_matrix<double> trans_matrix(4,4,0.0);
  //divide by ni-1 to account for 1 pixel overlap with the next tile
  trans_matrix[0][3] = lon_;
  if (direction_ == 'E')
    trans_matrix[0][0] = scale_i_/(ni-1.0);
  else
    trans_matrix[0][0] = -scale_i_/(ni-1.0);
  if (hemisphere_ == 'N') {
    trans_matrix[1][1] = -scale_j_/(nj-1.0);
    trans_matrix[1][3] = lat_+scale_j_+1/3600.0;
  }
  else {
    trans_matrix[1][1] = scale_j_/(nj-1.0);
    trans_matrix[1][3] = lat_-scale_j_-1/3600.0;
  }
  vpgl_lvcs_sptr dummy_lvcs = new vpgl_lvcs;
  cam_ = vpgl_geo_camera(trans_matrix, dummy_lvcs);
  cam_.set_scale_format(true);
#endif
}


//: specify lat lon as regular, e.g. negative lon for 'W'
volm_tile::volm_tile(float lat, float lon, float scale_i, float scale_j, unsigned ni, unsigned nj) :
        scale_i_(scale_i), scale_j_(scale_j), ni_(ni), nj_(nj)
{
  if (lat < 0) { lat_ = -lat; hemisphere_ = 'S'; } else { lat_ = lat; hemisphere_ = 'N'; }
  if (lon < 0) { lon_ = -lon; direction_ = 'W'; } else { lon_ = lon; direction_ = 'E'; }

  vnl_matrix<double> trans_matrix(4,4,0.0);
  //divide by ni-1 to account for 1 pixel overlap with the next tile
  if (direction_ == 'E') {
    trans_matrix[0][3] = lon_ - 0.5/(ni-1.0);
    trans_matrix[0][0] = scale_i_/(ni-1.0);
  }
  else {
    trans_matrix[0][3] = lon_ + 0.5/(ni-1.0);
    trans_matrix[0][0] = -scale_i_/(ni-1.0);
  }
  if (hemisphere_ == 'N') {
    trans_matrix[1][1] = -scale_j_/(nj-1.0);
    trans_matrix[1][3] = lat_+scale_j_+0.5/(nj-1.0);
  }
  else {
    trans_matrix[1][1] = scale_j_/(nj-1.0);
    trans_matrix[1][3] = lat_-scale_j_-0.5/(nj-1.0);
  }
  // just pass an empty lvcs, this geo cam will only be used to compute image pixels to global coords mappings
  vpgl_lvcs_sptr lv = new vpgl_lvcs;
  vpgl_geo_camera cam(trans_matrix, lv); cam.set_scale_format(true);
  cam_ = cam;
}

vcl_vector<volm_tile> volm_tile::generate_p1_tiles()
{
  vcl_vector<volm_tile> p1_tiles;
  unsigned ni = 3601;
  unsigned nj = 3601;
  float scale_i = 1;
  float scale_j = 1;
  p1_tiles.push_back(volm_tile(37, 118, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(37, 119, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(38, 118, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(38, 119, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(30, 82, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(31, 81, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(31, 82, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(32, 80, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(32, 81, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(33, 78, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(33, 79, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(33, 80, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(34, 77, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(34, 78, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(34, 79, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(35, 76, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(35, 77, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(36, 76, 'N', 'W', scale_i, scale_j, ni, nj));
  return p1_tiles;
}

vcl_vector<volm_tile> volm_tile::generate_p1_wr1_tiles()
{
  vcl_vector<volm_tile> p1_tiles;
  unsigned ni = 3601;
  unsigned nj = 3601;
  float scale_i = 1;
  float scale_j = 1;
  p1_tiles.push_back(volm_tile(37, 118, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(37, 119, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(38, 118, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(38, 119, 'N', 'W', scale_i, scale_j, ni, nj));
  return p1_tiles;
}

vcl_vector<volm_tile> volm_tile::generate_p1_wr2_tiles()
{
  vcl_vector<volm_tile> p1_tiles;
  unsigned ni = 3601;
  unsigned nj = 3601;
  float scale_i = 1;
  float scale_j = 1;
  p1_tiles.push_back(volm_tile(30, 82, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(31, 81, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(31, 82, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(32, 80, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(32, 81, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(33, 78, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(33, 79, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(33, 80, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(34, 77, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(34, 78, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(34, 79, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(35, 76, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(35, 77, 'N', 'W', scale_i, scale_j, ni, nj));
  p1_tiles.push_back(volm_tile(36, 76, 'N', 'W', scale_i, scale_j, ni, nj));
  return p1_tiles;
}

vcl_vector<volm_tile> volm_tile::generate_p1b_wr1_tiles()
{
  vcl_vector<volm_tile> p1b_tiles;
  unsigned ni = 3601;
  unsigned nj = 3601;
  float scale_i = 1;
  float scale_j = 1;
  //p1b_tiles.push_back(volm_tile(33, 71, 'S', 'W', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(33, 72, 'S', 'W', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(34, 71, 'S', 'W', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(34, 72, 'S', 'W', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(35, 71, 'S', 'W', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(35, 72, 'S', 'W', scale_i, scale_j, ni, nj));
  return p1b_tiles;
}

vcl_vector<volm_tile> volm_tile::generate_p1b_wr2_tiles()
{
  vcl_vector<volm_tile> p1b_tiles;
  unsigned ni = 3601;
  unsigned nj = 3601;
  float scale_i = 1;
  float scale_j = 1;
  //p1b_tiles.push_back(volm_tile(11, 71, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(12, 76, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(12, 77, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(12, 78, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(13, 76, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(13, 77, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(13, 78, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(18, 72, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(18, 73, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(18, 74, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(19, 72, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(19, 73, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(19, 74, 'N', 'E', scale_i, scale_j, ni, nj));
  return p1b_tiles;
}

vcl_vector<volm_tile> volm_tile::generate_p1b_wr3_tiles()
{
  vcl_vector<volm_tile> p1b_tiles;
  unsigned ni = 3601;
  unsigned nj = 3601;
  float scale_i = 1;
  float scale_j = 1;
  //p1b_tiles.push_back(volm_tile(30, 34, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(30, 35, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(30, 36, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(30, 37, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(31, 34, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(31, 35, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(31, 36, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(31, 37, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(32, 34, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(32, 35, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(32, 36, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(32, 37, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(33, 35, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(33, 36, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(33, 37, 'N', 'E', scale_i, scale_j, ni, nj));
  return p1b_tiles;
}

vcl_vector<volm_tile> volm_tile::generate_p1b_wr4_tiles()
{
  vcl_vector<volm_tile> p1b_tiles;
  unsigned ni = 3601;
  unsigned nj = 3601;
  float scale_i = 1;
  float scale_j = 1;
  p1b_tiles.push_back(volm_tile(5, 124, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(5, 125, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(6, 123, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(6, 124, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(6, 125, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(6, 126, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(7, 123, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(7, 124, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(7, 125, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(7, 126, 'N', 'E', scale_i, scale_j, ni, nj));
  return p1b_tiles;
}

vcl_vector<volm_tile> volm_tile::generate_p1b_wr5_tiles()
{
  vcl_vector<volm_tile> p1b_tiles;
  unsigned ni = 3601;
  unsigned nj = 3601;
  float scale_i = 1;
  float scale_j = 1;
  //p1b_tiles.push_back(volm_tile(21, 120, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(22, 120, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(22, 121, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(23, 120, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(23, 121, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(24, 120, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(24, 121, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(24, 122, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(25, 120, 'N', 'E', scale_i, scale_j, ni, nj));
  p1b_tiles.push_back(volm_tile(25, 121, 'N', 'E', scale_i, scale_j, ni, nj));
  //p1b_tiles.push_back(volm_tile(25, 122, 'N', 'E', scale_i, scale_j, ni, nj));
  return p1b_tiles;
}

vcl_vector<volm_tile> volm_tile::generate_p1b_wr_tiles(int world_id)
{
  vcl_vector<volm_tile> tiles;
  switch (world_id) {
  case 1: { tiles = volm_tile::generate_p1b_wr1_tiles(); break; }
  case 2: { tiles = volm_tile::generate_p1b_wr2_tiles(); break; }
  case 3: { tiles = volm_tile::generate_p1b_wr3_tiles(); break; }
  case 4: { tiles = volm_tile::generate_p1b_wr4_tiles(); break; }
  case 5: { tiles = volm_tile::generate_p1b_wr5_tiles(); break; }
  default: {
    vcl_cerr << "Unknown world id: " << world_id << vcl_endl;
           } }
  return tiles;
}

vcl_string volm_tile::get_string()
{
  vcl_stringstream str;
  str << hemisphere_ << lat_ << direction_ << vcl_setfill('0') << vcl_setw(3) << lon_
      << "_S" << scale_i_ << 'x' << scale_j_;
  return str.str();
}

void volm_tile::img_to_global(unsigned i, unsigned j, double& lon, double& lat)
{
  cam_.img_to_global(i, j, lon, lat);
  if (direction_ == 'W')
    lon = -lon;
  if (hemisphere_ == 'S')
    lat = -lat;
}

//: do the conversions in reporting lon,lat
double volm_tile::lower_left_lon()
{
  if (direction_ == 'W')
    return -lon_;
  else
    return lon_;
}

double volm_tile::lower_left_lat()
{
  if (hemisphere_ == 'S')
    return -lat_;
  else
    return lat_;
}


bool volm_tile::global_to_img(double lon, double lat, unsigned& i, unsigned& j)
{
  double u,v; double dummy_elev = 0;

  if (direction_ == 'W')
    lon = -lon;
  if (hemisphere_ == 'S')
    lat = -lat;
  cam_.global_to_img(lon, lat, dummy_elev, u, v);
  if (u < 0 || v < 0 || u >= this->ni_ || v >= this->nj_)
    return false;
  i = (unsigned)vcl_floor( (int)(u*100+0.5)/100+0.5);  // truncation up to 0.01 floating precion 
  j = (unsigned)vcl_floor( (int)(v*100+0.5)/100+0.5);  // this may be ceil cause image direction is in reverse in latitude
  if (j == this->nj_) j--;         // v may be larger than nj+0.5 due to the floating point precision
  return true;
}

//: calculate width of the tile
double volm_tile::calculate_width()
{
  vpgl_utm utm;
  double x, x2, y, y2; int utm_zone;
  utm.transform(lat_, lon_, x,  y, utm_zone);
  utm.transform(lat_, lon_+scale_i_, x2, y2, utm_zone);
  return vcl_abs(x-x2);
}

//: calculate width of the tile
double volm_tile::calculate_height()
{
  vpgl_utm utm;
  double x, x2, y, y2; int utm_zone;
  utm.transform(lat_, lon_, x,  y, utm_zone);
  utm.transform(lat_+scale_j_, lon_, x2, y2, utm_zone);
  return vcl_abs(y-y2);
}

void volm_tile::get_uncertainty_region(float lambda_i, float lambda_j, float cutoff, vbl_array_2d<bool>& mask, vbl_array_2d<float>& kernel)
{
#if 0
  //brip_vil_float_ops::extrema_kernel_mask(lambda_i, lambda_j, 0.0, kernel, mask, cutoff);
  //brip_vil_float_ops::gaussian_kernel_mask(lambda_i, lambda_j, 0.0, kernel, mask, cutoff, true);
#else
  assert(lambda_i == lambda_j);
  brip_vil_float_ops::gaussian_kernel_square_mask(lambda_i, kernel, mask, cutoff);
#endif
  unsigned nrows = (unsigned)mask.rows();
  unsigned ncols = (unsigned)mask.cols();
  float kernel_max = kernel[nrows/2][ncols/2];
  // normalize kernel
  for (unsigned i = 0; i < ncols; ++i)
    for (unsigned j = 0; j < nrows; ++j)
      kernel[j][i] /= kernel_max;
}

// mark the uncertainty region around a given location using a gaussian mask, the center has the designated score, the rest diminishes with respect to a cutoff degree
void volm_tile::mark_uncertainty_region(int i, int j, float score, vbl_array_2d<bool>& mask, vbl_array_2d<float>& kernel, vil_image_view<unsigned int>& img)
{
  unsigned nrows = (unsigned)mask.rows();
  unsigned ncols = (unsigned)mask.cols();

  int js = (int)vcl_floor(j - (float)nrows/2.0f + 0.5f);
  int is = (int)vcl_floor(i - (float)ncols/2.0f + 0.5f);
  int je = (int)vcl_floor(j + (float)nrows/2.0f + 0.5f);
  int ie = (int)vcl_floor(i + (float)ncols/2.0f + 0.5f);

  int ni = (int)img.ni();
  int nj = (int)img.nj();
  if (score > 0.0f) {
    for (int ii = is; ii < ie; ++ii)
      for (int jj = js; jj < je; ++jj) {
        int mask_i = ii - is;
        int mask_j = jj - js;
        if (mask[mask_j][mask_i] && ii >= 0 && jj >= 0 && ii < ni && jj < nj) {
          float val = score*kernel[mask_j][mask_i];
          unsigned int pix_val = (unsigned int)(val*volm_io::SCALE_VALUE) + 1;  // scale it
          if (img(ii,jj) > 0)
            img(ii,jj) = (img(ii,jj)+pix_val)/2;  // overwrites whatever values was in the image
          else
            img(ii,jj) = pix_val;
        }
      }
  }
}

// just overwrite whatever was in the image, cause it assumes that current value of the pixel is compared to this score before this function is called
void volm_tile::mark_uncertainty_region(int i, int j, float score, vbl_array_2d<bool>& mask, vbl_array_2d<float>& kernel, vil_image_view<vxl_byte>& img)
{
  unsigned nrows = (unsigned)mask.rows();
  unsigned ncols = (unsigned)mask.cols();

  int js = (int)vcl_floor(j - (float)nrows/2.0f + 0.5f);
  int is = (int)vcl_floor(i - (float)ncols/2.0f + 0.5f);
  int je = (int)vcl_floor(j + (float)nrows/2.0f + 0.5f);
  int ie = (int)vcl_floor(i + (float)ncols/2.0f + 0.5f);

  int ni = (int)img.ni();
  int nj = (int)img.nj();
  if (score > 0.0f) {

    for (int ii = is; ii < ie; ++ii)
      for (int jj = js; jj < je; ++jj) {
        int mask_i = ii - is;
        int mask_j = jj - js;
        if (mask[mask_j][mask_i] && ii >= 0 && jj >= 0 && ii < ni && jj < nj) {
          float val = score*kernel[mask_j][mask_i];
          //unsigned int pix_val = (unsigned int)(val*volm_io::SCALE_VALUE) + 1;  // scale it
          unsigned char pix_val = (unsigned char)(val*volm_io::SCALE_VALUE) + 1;
#if 0
          if (pix_val < volm_io::UNKNOWN) {
            pix_val = volm_io::STRONG_NEGATIVE;
          }
          else if (pix_val == volm_io::UNKNOWN) {
            pix_val = volm_io::UNKNOWN;
          }
          else {
            pix_val = volm_io::STRONG_POSITIVE;
          }
          if (img(ii,jj) > 0)
            img(ii,jj) = (img(ii,jj)+pix_val)/2;
          else
            img(ii,jj) = pix_val;  // overwrites whatever value was in the image
#endif // 0
            img(ii,jj) = pix_val;
        }
      }
  }
}

// create a kml file of the tile as a box and with circular marks throughout at every n pixels in each direction
void volm_tile::write_kml(vcl_string name, int n)
{
  vcl_ofstream ofs(name.c_str());
  bkml_write::open_document(ofs);

  double lon, lat;
  this->img_to_global(0,0,lon,lat);
  vnl_double_2 ul; ul[0] = lat; ul[1] = lon;
  this->img_to_global(0, this->nj_, lon, lat);
  vnl_double_2 ll; ll[0] = lat; ll[1] = lon;
  this->img_to_global(this->ni_, this->nj_, lon, lat);
  vnl_double_2 lr; lr[0] = lat; lr[1] = lon;
  this->img_to_global(this->ni_, 0, lon, lat);
  vnl_double_2 ur; ur[0] = lat; ur[1] = lon;
  bkml_write::write_box(ofs, this->get_string(), this->get_string(), ul, ur, ll, lr);
  if (n != 0) {
  double arcsecond = (n/2.0) * (1.0/3600.0);
  for (unsigned i = 0; i < this->ni_; i+=n)
    for (unsigned j = 0; j < this->nj_; j+=n) {
      this->img_to_global(i,j,lon,lat);
      ul[0] = lat; ul[1] = lon;
      ll[0] = lat-arcsecond; ll[1] = lon;
      lr[0] = lat-arcsecond; lr[1] = lon+arcsecond;
      ur[0] = lat; ur[1] = lon+arcsecond;
      bkml_write::write_box(ofs, this->get_string(), this->get_string(), ul, ur, ll, lr);
    }
  }
  bkml_write::close_document(ofs);
  ofs.close();
}

#if 0 //TODO
volm_tile volm_tile::parse_string(vcl_string& filename)
{
  vcl_string name = vul_file::strip_directory(filename);
  name = name.substr(name.find_first_of('_')+1, name.size());
  if (name.find_first_of('N') != name.end())
  vcl_string n = name.substr(name.find_first_of('N')+1, name.find_first_of('W'));
  float lon, lat, scale;
  vcl_stringstream str(n); str >> lat;
  n = name.substr(name.find_first_of('W')+1, name.find_first_of('_'));
  vcl_stringstream str2(n); str2 >> lon;
  n = name.substr(name.find_first_of('x')+1, name.find_last_of('.'));
  vcl_stringstream str3(n); str3 >> scale;
  vcl_cout << " lat: " << lat << " lon: " << lon << " scale: " << scale << vcl_endl;
}
#endif


//: Binary save self to stream.
void volm_tile::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, lat_);
  vsl_b_write(os, lon_);
  vsl_b_write(os, hemisphere_);
  vsl_b_write(os, direction_);
  vsl_b_write(os, scale_i_);
  vsl_b_write(os, scale_j_);
  vsl_b_write(os, ni_);
  vsl_b_write(os, nj_);
  cam_.b_write(os);
}

//: Binary load self from stream.
void volm_tile::b_read(vsl_b_istream &is)
{
  if (!is) return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsl_b_read(is, lat_);
    vsl_b_read(is, lon_);
    vsl_b_read(is, hemisphere_);
    vsl_b_read(is, direction_);
    vsl_b_read(is, scale_i_);
    vsl_b_read(is, scale_j_);
    vsl_b_read(is, ni_);
    vsl_b_read(is, nj_);
    cam_.b_read(is);
    break;

   default:
    vcl_cerr << "I/O ERROR: vpgl_geo_camera::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

vgl_box_2d<float> volm_tile::bbox()
{
  vgl_point_2d<float> p1((float)this->lower_left_lon(), (float)this->lower_left_lat());
  vgl_point_2d<float> p2(p1.x()+this->scale_i_, p1.y()+this->scale_j_);
  vgl_box_2d<float> box(p1, p2);
  return box;
}

vgl_box_2d<double> volm_tile::bbox_double()
{
  vgl_point_2d<double> p1(this->lower_left_lon(), this->lower_left_lat());
  vgl_point_2d<double> p2(p1.x()+this->scale_i_, p1.y()+this->scale_j_);
  vgl_box_2d<double> box(p1, p2);
  return box;
}
