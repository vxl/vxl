#include "volm_desc_ex_2d_indexer.h"
#include <vcl_algorithm.h>
#include <vil/vil_load.h>

vcl_string volm_desc_ex_2d_indexer::name_ = "ex_2d";

volm_desc_ex_2d_indexer::
volm_desc_ex_2d_indexer(vcl_string const& input_folder,
                        vcl_string const& out_index_folder, 
                        vcl_vector<double> const& radius,
                        unsigned const& nlands) : volm_desc_indexer(out_index_folder), nlands_(nlands)
{
  volm_io_tools::load_imgs(input_folder, classification_maps_, false);  // load them just like NLCD but do not load img resources

  if (radius.empty())
    radius_.push_back(1.0);
  else
    radius_ = radius;
  // sort the radius to ensure the bin order
  vcl_sort(radius_.begin(), radius_.end());
  ndists_ = (unsigned)radius.size() + 1;
  layer_size_ = ndists_ * nlands_;
  largest_rad_ = radius_[radius_.size()-1];
  largest_rad_seconds_ = ((largest_rad_ / 30.0) + 1.0) * (1.0 / 3600.0);  // 1 arcseconds is ~ 30 meter, 1 arcseconds is 1/3600 seconds
}

// 
bool volm_desc_ex_2d_indexer::get_next()
{
  current_leaf_maps_.clear();

  vgl_box_2d<double> extent(leaves_[current_leaf_id_]->extent_.min_x(), leaves_[current_leaf_id_]->extent_.max_x(),
                            leaves_[current_leaf_id_]->extent_.min_y(), leaves_[current_leaf_id_]->extent_.max_y());

  // enlarge this box with largest_rad_ to make sure we get all possible images that would be within that distance from any hypo in this leaf
  extent.expand_about_centroid(largest_rad_seconds_);

  for (unsigned i = 0; i < classification_maps_.size(); i++) {
    if (vgl_intersection(classification_maps_[i].bbox, extent).area() > 0) {
      current_leaf_maps_.push_back(i);
      // load the image resource (if not already loaded)
      if (!classification_maps_[i].img_r) {
        classification_maps_[i].img_r = vil_load(classification_maps_[i].img_name.c_str());
        classification_maps_[i].ni = classification_maps_[i].img_r->ni(); classification_maps_[i].nj = classification_maps_[i].img_r->nj(); 
      }
    }
  }
  vcl_cout << "!!!!!!! there are: " << current_leaf_maps_.size() << " classification maps that intersect the current leaf!\n";
  return true;
}


bool volm_desc_ex_2d_indexer::extract(double lat, double lon, double elev, vcl_vector<unsigned char>& values)
{
  volm_desc_ex_land_only* desc = new volm_desc_ex_land_only(ndists_, nlands_, radius_);
  
  // use location to create a local vertical coordinate system, to get distances in meters
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(lat, lon, elev, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);

  // find the four corners
  vcl_vector<vcl_pair<double, double> > corners;
  corners.push_back(vcl_pair<double, double>(lon-largest_rad_seconds_, lat-largest_rad_seconds_));
  corners.push_back(vcl_pair<double, double>(lon+largest_rad_seconds_, lat-largest_rad_seconds_));
  corners.push_back(vcl_pair<double, double>(lon+largest_rad_seconds_, lat+largest_rad_seconds_));
  corners.push_back(vcl_pair<double, double>(lon-largest_rad_seconds_, lat+largest_rad_seconds_));

  // find out which images need to be considered and the corners in pixels in each image
  vcl_map<unsigned, vcl_vector<vcl_pair<int, int> > > imgs_and_corners;
  double u,v;
  for (unsigned k = 0; k < current_leaf_maps_.size(); k++) {
    vpgl_geo_camera* cam = classification_maps_[current_leaf_maps_[k]].cam;
    int ni = (int)classification_maps_[current_leaf_maps_[k]].ni;
    int nj = (int)classification_maps_[current_leaf_maps_[k]].nj;
    vcl_vector<vcl_pair<int, int> > img_corners;
    bool at_least_one = false;
    int min_i = ni-1, min_j = nj-1, max_i = 0, max_j = 0;
    for (unsigned m = 0; m < corners.size(); m++) {
      cam->global_to_img(-corners[m].first, corners[m].second, 0.0, u, v); // WARNING: W is hard coded in vpgl_geo_camera so use -lon!!!!
      int i = (int)vcl_floor(u + 0.5);
      int j = (int)vcl_floor(v + 0.5);
      if (i >= 0 && j >= 0 && i < ni && j < nj)
        at_least_one = true;
      min_i = min_i > i ? i : min_i; 
      max_i = max_i < i ? i : max_i;
      min_j = min_j > j ? j : min_j;
      max_j = max_j < j ? j : max_j;
    }
    if (at_least_one) {
      vcl_vector<vcl_pair<int, int> > tmp;
      tmp.push_back(vcl_pair<int, int>(min_i, min_j));
      tmp.push_back(vcl_pair<int, int>(max_i, max_j));
      imgs_and_corners[current_leaf_maps_[k]] = tmp;
    }
  }

  for (vcl_map<unsigned, vcl_vector<vcl_pair<int, int> > >::iterator it = imgs_and_corners.begin(); it != imgs_and_corners.end(); it++) {
    vil_image_view<unsigned char> map(classification_maps_[it->first].img_r);
    vpgl_geo_camera* cam = classification_maps_[it->first].cam;
    int min_i = it->second[0].first;
    int min_j = it->second[0].second;
    int max_i = it->second[1].first;
    int max_j = it->second[1].second;
    
    // this is stupid to go over all the image, but for now it makes sure we update the histogram with every pixel within largest_radius to lat, lon
    for (unsigned i = min_i; i <= max_i; i++)
      for (unsigned j = min_j; j <= max_j; j++) {
        double llon, llat;
        cam->img_to_global(i, j, llon, llat);  // WARNING: W is hard coded in vpgl_geo_camera so use -lon in the following lvcs method!!!!! 
        double lx, ly, lz;
        lvcs->global_to_local(-llon, llat, elev, vpgl_lvcs::wgs84, lx, ly, lz);
        double dist = vcl_sqrt(lx*lx + ly*ly); // local coords are in meters so get the dist in meters directly
        
        if (dist > largest_rad_)  // ignore everything outside
          continue;
        unsigned char land_id = map(i,j);
        desc->set_count(dist, land_id, 1);
      }
  }
  
  desc->get_char_array(values);
  return true;
}

