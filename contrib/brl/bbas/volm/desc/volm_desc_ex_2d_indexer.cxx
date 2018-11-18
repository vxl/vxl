#include <iostream>
#include <algorithm>
#include "volm_desc_ex_2d_indexer.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_load.h>
#include <vgl/vgl_area.h>

std::string& volm_desc_ex_2d_indexer::get_name()
{
  static std::string name = "ex_2d";
  return name;
}


volm_desc_ex_2d_indexer::
volm_desc_ex_2d_indexer(std::string const& input_folder,
                        std::string const& out_index_folder,
                        std::vector<double> const& radius,
                        unsigned const& nlands) : volm_desc_indexer(out_index_folder), nlands_(nlands)
{
  volm_io_tools::load_imgs(input_folder, classification_maps_, false, true, false);  // load them just like NLCD but do not load img resources

  if (radius.empty())
    radius_.push_back(1.0);
  else
    radius_ = radius;
  // sort the radius to ensure the bin order
  std::sort(radius_.begin(), radius_.end());
  ndists_ = (unsigned)radius.size() + 1;
  layer_size_ = ndists_ * nlands_;
  largest_rad_ = radius_[radius_.size()-1];
  largest_rad_seconds_ = ((largest_rad_ / 30.0) + 1.0) * (1.0 / 3600.0);  // 1 arcseconds is ~ 30 meter, 1 arcseconds is 1/3600 seconds
}

//
bool volm_desc_ex_2d_indexer::get_next()
{
  current_leaf_maps_.clear();
  lon_imgs.clear();
  lat_imgs.clear();
  vgl_box_2d<double> extent(leaves_[current_leaf_id_]->extent_.min_x(), leaves_[current_leaf_id_]->extent_.max_x(),
                            leaves_[current_leaf_id_]->extent_.min_y(), leaves_[current_leaf_id_]->extent_.max_y());

  // obtain the hemisphere and direction fro current leaf (assuming no leaf will cross Equator or Prime meridian)
  std::string hemisphere, direction;
  if (extent.centroid_x() <= 0)  direction = "W";
  else                           direction = "E";
  if (extent.centroid_y() <= 0)  hemisphere = "S";
  else                           hemisphere = "N";

  // enlarge this box with largest_rad_ to make sure we get all possible images that would be within that distance from any hypo in this leaf
  extent.expand_about_centroid(largest_rad_seconds_);

  for (unsigned i = 0; i < classification_maps_.size(); i++) {
    if (vgl_area(vgl_intersection(classification_maps_[i].bbox, extent)) > 0) {
      current_leaf_maps_.push_back(i);
      // load the image resource (if not already loaded)
      if (!classification_maps_[i].img_r) {
        classification_maps_[i].img_r = vil_load(classification_maps_[i].img_name.c_str());
        classification_maps_[i].ni = classification_maps_[i].img_r->ni(); classification_maps_[i].nj = classification_maps_[i].img_r->nj();
      }
      // compute the lon and lat images (if not already computed!)
      auto lon_it = lon_imgs.find(i);
      if (lon_it == lon_imgs.end()) {
        vil_image_view<double> lon_img(classification_maps_[i].ni, classification_maps_[i].nj);
        vil_image_view<double> lat_img(classification_maps_[i].ni, classification_maps_[i].nj);
        for (unsigned ii = 0; ii < lon_img.ni(); ii++)
          for (unsigned jj = 0; jj < lon_img.nj(); jj++) {
            double llon, llat;
            classification_maps_[i].cam->img_to_global(ii, jj, llon, llat);  // WARNING: W is hard coded in vpgl_geo_camera so use -lon in the following lvcs method!!!!!
            lon_img(ii, jj) = llon;
            lat_img(ii, jj) = llat;
            //if (direction == "W") lon_img(ii,jj) = -llon;
            //else                  lon_img(ii,jj) = llon;
            //if (hemisphere == "S") lat_img(ii,jj) = -llat;
            //else                   lat_img(ii,jj) = llat;
          }
        vil_image_view_base_sptr lon_img_sptr = new vil_image_view<double>(lon_img);
        vil_image_view_base_sptr lat_img_sptr = new vil_image_view<double>(lat_img);
        lon_imgs[i] = lon_img_sptr;
        lat_imgs[i] = lat_img_sptr;
      }
    }
  }
  std::cout << "!!!!!!! there are: " << current_leaf_maps_.size() << " classification maps that intersect the current leaf!\n";
  std::cout.flush();
  return true;
}


bool volm_desc_ex_2d_indexer::extract(double lat, double lon, double elev, std::vector<unsigned char>& values)
{
  auto* desc = new volm_desc_ex_land_only(ndists_, nlands_, radius_);

  // use location to create a local vertical coordinate system, to get distances in meters
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(lat, lon, elev, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);

  // find the four corners
  std::vector<std::pair<double, double> > corners;
  corners.emplace_back(lon-largest_rad_seconds_, lat-largest_rad_seconds_);
  corners.emplace_back(lon+largest_rad_seconds_, lat-largest_rad_seconds_);
  corners.emplace_back(lon+largest_rad_seconds_, lat+largest_rad_seconds_);
  corners.emplace_back(lon-largest_rad_seconds_, lat+largest_rad_seconds_);

  // find out which images need to be considered and the corners in pixels in each image
  std::map<unsigned, std::vector<std::pair<int, int> > > imgs_and_corners;
  double u,v;
  for (unsigned int current_leaf_map : current_leaf_maps_) {
    vpgl_geo_camera* cam = classification_maps_[current_leaf_map].cam;
    int ni = (int)classification_maps_[current_leaf_map].ni;
    int nj = (int)classification_maps_[current_leaf_map].nj;
    bool at_least_one = false;
    int min_i = ni-1, min_j = nj-1, max_i = 0, max_j = 0;
    for (auto & corner : corners) {
      double lon_abs, lat_abs;
      lon_abs = corner.first;  lat_abs = corner.second;
      //if (lon_abs < 0) lon_abs = -lon_abs;
      //if (lat_abs < 0) lat_abs = -lat_abs;
      cam->global_to_img(lon_abs, lat_abs, 0.0, u, v);
      int i = (int)std::floor(u + 0.5);
      int j = (int)std::floor(v + 0.5);
      if (i >= 0 && j >= 0 && i < ni && j < nj)
        at_least_one = true;
      if (i < 0) i = 0; if (j < 0) j = 0;
      if (i >= ni) i = ni-1; if (j >= nj) j = nj-1;
      min_i = min_i > i ? i : min_i;
      max_i = max_i < i ? i : max_i;
      min_j = min_j > j ? j : min_j;
      max_j = max_j < j ? j : max_j;
    }
    if (at_least_one) {
      std::vector<std::pair<int, int> > tmp;
      tmp.emplace_back(min_i, min_j);
      tmp.emplace_back(max_i, max_j);
      imgs_and_corners[current_leaf_map] = tmp;
    }
  }

  for (auto & imgs_and_corner : imgs_and_corners) {
    vil_image_view<unsigned char> map(classification_maps_[imgs_and_corner.first].img_r);
    int min_i = imgs_and_corner.second[0].first;
    int min_j = imgs_and_corner.second[0].second;
    int max_i = imgs_and_corner.second[1].first;
    int max_j = imgs_and_corner.second[1].second;
    vil_image_view<double> lon_img(lon_imgs[imgs_and_corner.first]);
    vil_image_view<double> lat_img(lat_imgs[imgs_and_corner.first]);

    for (int i = min_i; i <= max_i; i++)
      for (int j = min_j; j <= max_j; j++) {
        //double llon, llat;
        //cam->img_to_global(i, j, llon, llat);  // WARNING: W is hard coded in vpgl_geo_camera so use -lon in the following lvcs method!!!!!
        double lx, ly, lz;
        //lvcs->global_to_local(-llon, llat, elev, vpgl_lvcs::wgs84, lx, ly, lz);
        lvcs->global_to_local(lon_img(i,j), lat_img(i,j), elev, vpgl_lvcs::wgs84, lx, ly, lz);
        double dist = std::sqrt(lx*lx + ly*ly); // local coords are in meters so get the dist in meters directly

        if (dist > largest_rad_)  // ignore everything outside
          continue;
        unsigned char land_id = map(i,j);
        desc->set_count(dist, land_id, 1);
      }
  }

  desc->get_char_array(values);
  std::cout << '.';
  std::cout.flush();
  return true;
}

bool volm_desc_ex_2d_indexer::write_params_file()
{
  volm_buffered_index_params params;
  params.radius = radius_;
  params.layer_size = layer_size_;
  params.nlands = nlands_;
  params.norients = 0;
  if (!params.write_ex_param_file(out_file_name_pre_.str()))
    return false;
  return true;
}
