#include "volm_tile.h"
#include "volm_io.h"
//:
// \file
#include <vcl_iomanip.h>
#include <vcl_sstream.h>

#include <brip/brip_vil_float_ops.h>
#include <bkml/bkml_write.h>

volm_tile::volm_tile(float lat, float lon, char hemisphere, char direction, float scale_i, float scale_j, unsigned ni, unsigned nj) : 
        lat_(lat), lon_(lon), hemisphere_(hemisphere), direction_(direction), scale_i_(scale_i), scale_j_(scale_j), ni_(ni), nj_(nj)         
{
  vnl_matrix<double> trans_matrix(4,4,0.0);
  trans_matrix[0][0] = -scale_i_/ni; trans_matrix[1][1] = -scale_j_/nj;
  trans_matrix[0][3] = lon; trans_matrix[1][3] = lat+scale_j_;
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
 
vcl_string volm_tile::get_string()
{
  vcl_stringstream str;
  str << hemisphere_ << lat_ << direction_ << vcl_setfill('0') << vcl_setw(3) << lon_;
  str << "_S" << scale_i_ << "x" << scale_j_;
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

void volm_tile::get_uncertainty_region(float lambda_i, float lambda_j, float cutoff, vbl_array_2d<bool>& mask, vbl_array_2d<float>& kernel)
{
  //brip_vil_float_ops::extrema_kernel_mask(lambda_i, lambda_j, 0.0, kernel, mask, cutoff);
  //brip_vil_float_ops::gaussian_kernel_mask(lambda_i, lambda_j, 0.0, kernel, mask, cutoff, true);
  brip_vil_float_ops::gaussian_kernel_square_mask(lambda_i, kernel, mask, cutoff);
  unsigned nrows = (unsigned)mask.rows();
  unsigned ncols = (unsigned)mask.cols();
  float kernel_max = kernel[nrows/2][ncols/2];
  // normalize kernel
  for (unsigned i = 0; i < ncols; i++)
    for (unsigned j = 0; j < nrows; j++)
      kernel[j][i] /= kernel_max;
      
}
//: mark the uncertainty region around a given location using a gaussian mask, the center has the designated score, the rest diminishes with respect to a cutoff degree
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
    for (int ii = is; ii < ie; ii++)
      for (int jj = js; jj < je; jj++) {
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

//: create a kml file of the tile as a box and with circular marks throughout at every n pixels in each direction
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
  vcl_cout << " lat: " << lat << " lon: " << lon << " scale:" << scale << vcl_endl; 
}
#endif