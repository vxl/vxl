#include "boxm2_volm_locations.h"
//:
// \file
#include <vbl/vbl_array_2d.h>
#include <vgl/vgl_box_3d.h>
#include <bkml/bkml_write.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: construct the locs_ and pixels_ vectors by generating a hypothesis according to interval amounts in given the tile.
//  Intervals are in meters.
//  Only one elev hypothesis per location for now..
//  keep only the locations that the scene covers
void
boxm2_volm_loc_hypotheses::add_dems(const boxm2_scene_sptr& scene,
                                    unsigned interval_i, unsigned interval_j,
                                    float altitude,
                                    std::vector<vil_image_view<float> >& dems, std::vector<vpgl_geo_camera*>& cams)
{
  std::cout << "tile lat: " << tile_.lat_ << " lon: " << tile_.lon_ << std::endl
           << "dem ni: " << dems[0].ni() << " nj: " << dems[0].nj() << std::endl;

  vgl_box_3d<double> sbox = scene->bounding_box();

  for (unsigned int i = 0; i < tile_.ni_; i += interval_i)
    for (unsigned int j = 0; j < tile_.nj_; j += interval_j) {
      // fetch the global location from the tile using its transformation matrix saved in vpgl_geo_camera member
      double lon, lat;
      tile_.img_to_global(i,j,lon,lat); // instead of: double lon=i, lat=j; // instead of: tile_.img_to_global(i,j,lon,lat);

      float elev = 0;
      // find the dem that includes this location to fetch elev
      for (unsigned k = 0; k < dems.size(); k++) {
        double u,v;
        cams[k]->global_to_img(lon, lat, 0, u, v);
        // find the nearest pixel in the dem (no interpolation of any kind)
        int uu = (int)std::floor(u+0.5);
        int vv = (int)std::floor(v+0.5);
        //std::cout << "i: " << i << " j: " << j << " lon: " << lon << " lat: " << lat << " u: " << u << " v: " << v << " uu: " << uu << " vv: " << vv << std::endl;
        if (uu >= 0 && vv >= 0 && uu < (int)dems[k].ni() && vv < (int)dems[k].nj()) {
          elev = dems[k]((unsigned)uu,(unsigned)vv);
          break;
        }
      }
      if (elev > 0) { // add this as a viable location hypotheses
        this->add(scene, sbox, lon, lat, elev+altitude, i, j);
      }
    }
}

//: add a hypothesis given as a global lon, lat and elev for the pixel (i,j) of the tile
bool boxm2_volm_loc_hypotheses::add(const boxm2_scene_sptr& scene, vgl_box_3d<double>& scene_bounding_box, double lon, double lat, double elev, unsigned i, unsigned j)
{
  double lx, ly, lz;
  scene->lvcs().global_to_local(lon, lat, elev, vpgl_lvcs::wgs84, lx, ly, lz);
  vgl_point_3d<double> local(lx, ly, lz);
  if (scene_bounding_box.contains(local)) {
    vgl_point_3d<float> ll((float)lx, (float)ly, (float)lz);
    locs_.push_back(ll);
    pixels_.emplace_back(i,j);
    return true;
  }
  return false;
}
//: add a hypothesis given as a global lon, lat and as local coords for the scene
bool boxm2_volm_loc_hypotheses::add(double lon, double lat, float cent_x, float cent_y, float cent_z)
{
  unsigned i, j;
  if (!tile_.global_to_img(lon, lat, i, j))
    return false;
  vgl_point_3d<float> ll(cent_x, cent_y, cent_z);
  locs_.push_back(ll);
  pixels_.emplace_back(i,j);
  return true;
}




//: construct by reading from a binary file
boxm2_volm_loc_hypotheses::boxm2_volm_loc_hypotheses(const std::string& bin_file)
{
  vsl_b_ifstream is(bin_file.c_str());
  if (!is) {
    std::cerr << "In boxm2_volm_loc_hypotheses::boxm2_volm_loc_hypotheses() -- cannot open: " << bin_file << std::endl;
    return;
  }
  this->b_read(is);
}


//: construct the output tile image using the score, score vector's size need to be same as locs_ size
void boxm2_volm_loc_hypotheses::generate_output_tile(std::vector<float>& scores, int uncertainty_size_i, int uncertainty_size_j, float cut_off, vil_image_view<unsigned int>& out)
{
  assert(scores.size() == locs_.size());
  vbl_array_2d<bool> mask;
  vbl_array_2d<float> kernel;

  volm_tile::get_uncertainty_region((float)uncertainty_size_i, (float)uncertainty_size_j, cut_off, mask, kernel);

  #if 0
  std::cout << "kernel_max: " << kernel[kernel.rows()/2][kernel.cols()/2]
           << "uncertainty mask:\n";
  for (unsigned i = 0; i < mask.cols(); i++) {
    for (unsigned j = 0; j < mask.rows(); j++)
      std::cout << mask[i][j] << ' ';
    std::cout << '\n';
  }
  for (unsigned i = 0; i < kernel.cols(); i++) {
    for (unsigned j = 0; j < kernel.rows(); j++)
      std::cout << kernel[i][j] << ' ';
    std::cout << '\n';
  }
  #endif
  for (unsigned i = 0; i < scores.size(); i++)
    volm_tile::mark_uncertainty_region(pixels_[i].first,  pixels_[i].second, scores[i], mask, kernel, out);
}

bool boxm2_volm_loc_hypotheses::write_hypotheses(const std::string& out_file)
{
  vsl_b_ofstream os(out_file.c_str());
  if (!os)
    return false;
  this->b_write(os);
  os.close();
  return true;
}

//: Binary save self to stream.
void boxm2_volm_loc_hypotheses::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  tile_.b_write(os);
  vsl_b_write(os, pixels_.size());
  for (const auto & pixel : pixels_) {
    vsl_b_write(os, pixel.first);
    vsl_b_write(os, pixel.second);
  }
  vsl_b_write(os, locs_.size());
  for (auto loc : locs_) {
    vsl_b_write(os, loc.x());
    vsl_b_write(os, loc.y());
    vsl_b_write(os, loc.z());
  }
}

//: Binary load self from stream.
void boxm2_volm_loc_hypotheses::b_read(vsl_b_istream &is)
{
  if (!is) return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1: {
     tile_.b_read(is);
     unsigned s;
     vsl_b_read(is, s);
     pixels_.resize(s, std::pair<unsigned, unsigned>(0,0));
     for (auto & pixel : pixels_) {
       vsl_b_read(is, pixel.first);
       vsl_b_read(is, pixel.second);
     }
     vsl_b_read(is, s);
     locs_.resize(s);
     for (auto & loc : locs_) {
       float x,y,z;
       vsl_b_read(is, x);
       vsl_b_read(is, y);
       vsl_b_read(is, z);
       loc = vgl_point_3d<float>(x,y,z);
     }
     break; }
   default:
    std::cerr << "I/O ERROR: boxm2_volm_loc_hypotheses::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//: for debugging purposes
bool boxm2_volm_loc_hypotheses::write_hypotheses_kml(const boxm2_scene_sptr& scene, const std::string& kml_file)
{
  std::ofstream ofs(kml_file.c_str());
  if (!ofs) {
    std::cerr << "In boxm2_volm_loc_hypotheses::write_hypotheses_kml() -- cannot open: " << kml_file << std::endl;
    return false;
  }
  bkml_write::open_document(ofs);
  for (unsigned i = 0; i < locs_.size(); i++) {
    double lon, lat, gz;
    scene->lvcs().local_to_global(locs_[i].x(), locs_[i].y(), locs_[i].z(), vpgl_lvcs::wgs84, lon, lat, gz);
    std::stringstream str; str << "loc hypotheses " << i;
    vnl_double_2 ul(lat, lon); vnl_double_2 ur(lat+0.0005, lon); vnl_double_2 ll(lat, lon-0.0005); vnl_double_2 lr(lat+0.0005, lon-0.0005);
    bkml_write::write_box(ofs, str.str(), str.str(), ul, ur, ll, lr);
  }
  bkml_write::close_document(ofs);
  return true;
}
