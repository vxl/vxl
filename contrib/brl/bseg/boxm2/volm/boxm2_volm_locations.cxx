#include "boxm2_volm_locations.h"
//:
// \file
#include <vbl/vbl_array_2d.h>
#include <vcl_cassert.h>

//: construct the locs_ and lat_, lon_ vectors by generating a hypothesis according to interval amounts.
//  Intervals are in meters.
//  Only one elev hypothesis per location for now..
boxm2_volm_loc_hypotheses::boxm2_volm_loc_hypotheses(vpgl_lvcs_sptr lvcs,
                                                     volm_tile tile,
                                                     unsigned interval_i, unsigned interval_j,
                                                     float altitude,
                                                     vcl_vector<vil_image_view<float> >& dems,
                                                     vcl_vector<vpgl_geo_camera*>& cams) : tile_(tile)
{
  vcl_cout << "tile lat: " << tile.lat_ << " lon: " << tile.lon_ << vcl_endl
           << "dem ni: " << dems[0].ni() << " nj: " << dems[0].nj() << vcl_endl;
#if 0 // unused:
  double arcsecond_i = (interval_i/2.0) * (1.0/3600.0);
  double arcsecond_j = (interval_j/2.0) * (1.0/3600.0);
#endif
  for (int i = 0; i < tile_.lon_; i += interval_i)
    for (int j = 0; j < tile_.lat_; j += interval_j) {
      // fetch the global location from the tile
      double lon=i, lat=j; // instead of: tile_.img_to_global(i,j,lon,lat);

      float elev = 0;
      // find the dem that includes this location to fetch elev
      for (unsigned k = 0; k < dems.size(); k++) {
        double u,v;
        cams[k]->global_to_img(lon, lat, 0, u, v);
        int uu = (int)vcl_floor(u+0.5);
        int vv = (int)vcl_floor(v+0.5);
        vcl_cout << "i: " << i << " j: " << j << " lon: " << lon << " lat: " << lat << " u: " << u << " v: " << v << " uu: " << uu << " vv: " << vv << vcl_endl;
        if (uu > 0 && vv > 0 && uu < (int)dems[k].ni() && vv < (int)dems[k].nj()) {
          elev = dems[k]((unsigned)uu,(unsigned)vv);
          break;
        }
      }
      if (elev > 0) { // add this as a viable location hypotheses
        double lx, ly, lz;
        lvcs->global_to_local(lon, lat, elev+altitude, vpgl_lvcs::wgs84, lx, ly, lz);
        vgl_point_3d<float> local((float)lx, (float)ly, (float)lz);
        locs_.push_back(local);
        pixels_.push_back(vcl_pair<unsigned, unsigned>(i,j));
      }
    }
}

//: construct the output tile image using the score, score vector's size need to be same as locs_ size
void boxm2_volm_loc_hypotheses::generate_output_tile(vcl_vector<float>& scores, int uncertainty_size_i, int uncertainty_size_j, float cut_off, vil_image_view<unsigned int>& out)
{
  assert(!"boxm2_volm_loc_hypotheses::generate_output_tile() not yet implemented...");
#if 0
  assert(scores.size() == locs_.size());
  vbl_array_2d<bool> mask;
  vbl_array_2d<float> kernel;

  volm_tile::get_uncertainty_region((float)uncertainty_size_i, (float)uncertainty_size_j, cut_off, mask, kernel);

  vcl_cout << "kernel_max: " << kernel[kernel.rows()/2][kernel.cols()/2]
           << "uncertainty mask:\n";
  for (unsigned i = 0; i < mask.cols(); i++) {
    for (unsigned j = 0; j < mask.rows(); j++)
      vcl_cout << mask[i][j] << ' ';
    vcl_cout << '\n';
  }
  for (unsigned i = 0; i < kernel.cols(); i++) {
    for (unsigned j = 0; j < kernel.rows(); j++)
      vcl_cout << kernel[i][j] << ' ';
    vcl_cout << '\n';
  }

  for (unsigned i = 0; i < scores.size(); i++)
    volm_tile::mark_uncertainty_region(pixels_[i].first,  pixels_[i].second, scores[i], mask, kernel, out);
#endif
}

