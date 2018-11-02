#include <volm/conf/volm_conf_2d_indexer.h>
//:
// \file
#include <vgl/vgl_closest_point.h>
#include <vul/vul_file.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <volm/volm_buffered_index.h>             // for parameter class
#include <bkml/bkml_write.h>

std::string volm_conf_2d_indexer::name_ = "conf_2d";

volm_conf_2d_indexer::volm_conf_2d_indexer(double const& radius, std::string const& out_folder, std::string const& land_map_folder, unsigned const& tile_id)
  : radius_(radius), land_map_folder_(land_map_folder)
{
  // calculate radius in degree
  radius_in_degree_ = ( ( (2*radius_) / 30.0) + 1.0) * (1.0 / 3600.0);  // 1 arcseconds is ~ 30 meter, 1 arcseconds is 1/3600 seconds
  square_radius_ = radius_*radius_;
  current_leaf_id_ = 0;
  tile_id_ = tile_id;
  // specify output folder
  out_index_folder_ = out_folder;
  out_file_name_pre_.clear();
  out_file_name_pre_ << out_index_folder_ << "conf_index_tile_" << tile_id_;
  // load land map 2d tree
  std::stringstream file_name_pre;
  file_name_pre << land_map_folder << "/2d_geo_index_tile_" << tile_id_ << ".txt";
  bool file_exist = vul_file::exists(file_name_pre.str());
  assert(file_exist);
  double min_size;
  land_map_root_ = bvgl_2d_geo_index::read_and_construct<volm_conf_land_map_indexer_sptr>(file_name_pre.str(), min_size);
  land_map_leaves_.clear();
  // load the land map data
}

bool volm_conf_2d_indexer::write_params_file()
{
  volm_buffered_index_params params;
  params.conf_radius = radius_;
  if (!params.write_conf_param_file(out_file_name_pre_.str()))
    return false;
  return true;
}

// keep adding contents into land_map_leaves database
bool volm_conf_2d_indexer::get_next()
{
  // set up the region for current location leaf that is large enough to load all land_map leaves
  vgl_box_2d<double> bbox = loc_leaves_[current_leaf_id_]->extent_;
  double min_x, max_x, min_y, max_y;
  min_x = bbox.min_x() - radius_in_degree_;
  max_x = bbox.max_x() + radius_in_degree_;
  min_y = bbox.min_y() - radius_in_degree_;
  max_y = bbox.max_y() + radius_in_degree_;
  bbox.set_min_x(min_x);  bbox.set_max_x(max_x);
  bbox.set_min_y(min_y);  bbox.set_max_y(max_y);
  // obtain the leaves
  std::vector<bvgl_2d_geo_index_node_sptr> leaves;
  bvgl_2d_geo_index::get_leaves(land_map_root_, leaves, bbox);
  // load the content
  for (auto & leave : leaves) {
    auto* leaf_ptr = dynamic_cast<bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>*>(leave.ptr());
    std::stringstream content_bin_file;
    content_bin_file << land_map_folder_ << leave->get_label_name("land_map_index","all");
    if (!vul_file::exists(content_bin_file.str()))
      continue;
    if (!leaf_ptr->contents_)
    {
      leaf_ptr->contents_ = new volm_conf_land_map_indexer(content_bin_file.str());
      //std::cout << "l_idx: " << l_idx << " content_bin_file: " << content_bin_file.str() << std::endl;
      // put leaf into database
      land_map_leaves_.push_back(leave);
    }
  }
#if 0
  std::cout << land_map_leaves_.size() << " are added for location leaf " << current_leaf_id_ << ": " << loc_leaves_[current_leaf_id_]->extent_ << std::endl;
  std::cout << "expand the loc leaf: "
           << bbox.min_x() << "," << bbox.min_y() << ",0\n"
           << bbox.max_x() << "," << bbox.min_y() << ",0\n"
           << bbox.max_x() << "," << bbox.max_y() << ",0\n"
           << bbox.min_x() << "," << bbox.max_y() << ",0\n"
           << bbox.min_x() << "," << bbox.min_y() << ",0\n";
  for (unsigned i = 0; i < land_map_leaves_.size(); i++) {
    std::stringstream content_bin_file;
    content_bin_file << land_map_folder_ << land_map_leaves_[i]->get_label_name("land_map_index","all");
    std::string kml_file = vul_file::strip_extension(content_bin_file.str()) + ".kml";
    std::cout << kml_file << std::flush << std::endl;
  }
#endif
  return true;
}

// function to create index for a location
bool volm_conf_2d_indexer::extract(double const& lon, double const& lat, double const&  /*elev*/, std::vector<volm_conf_object>& values)
{
#if 0
  std::stringstream kml_file;
  kml_file << "index_test_" << lon << "_" << lat << ".kml";
  std::ofstream ofs(kml_file.str().c_str());
  bkml_write::open_document(ofs);
#endif
  // distance calculation requires a transformation from degree to meter
  // a local lvcs is constructed from the loc hypo (lon, lat, elev);
  vpgl_lvcs lvcs(lat, lon, 0.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  values.clear();
  // obtain bounding box (avoid using lvcs to speed up)
  double lon_min = lon - radius_in_degree_, lon_max = lon + radius_in_degree_;
  double lat_min = lat - radius_in_degree_, lat_max = lat + radius_in_degree_;
  vgl_box_2d<double> bbox(lon_min, lon_max, lat_min, lat_max);

  // obtain the leaves that intersect current location ROI
  std::vector<bvgl_2d_geo_index_node_sptr> leaves;
  bvgl_2d_geo_index::get_leaves(land_map_root_, leaves, bbox);
  if ( leaves.empty() )
    return true;
  // further reduce the number of leaves by check the minimum distance and contents
  std::vector<bvgl_2d_geo_index_node_sptr> valid_leaves;
  std::vector<double> min_dist_vec;
  for (auto & leave : leaves) {
    auto* leaf_ptr = dynamic_cast<bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>*>(leave.ptr());
    if (!leaf_ptr->contents_)
      continue;
    // calculate the minimum distance from location point to the leaf box
    double min_dist = min_dist_from_box_to_pt(lvcs, leaf_ptr->extent_, lon, lat);
    if ( min_dist > radius_)
      continue;
    min_dist_vec.push_back(min_dist);
    valid_leaves.push_back(leave);
  }

  // loop over each leaf to construct the index
  // note the index takes hypo location as origin, east as x axis and north as y axis (the angular value ranges from 0 to 2pi)
  auto n_leaves = (unsigned)valid_leaves.size();
  for (unsigned i = 0; i < n_leaves; i++)
  {
    auto* leaf_ptr = dynamic_cast<bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>*>(valid_leaves[i].ptr());
    if (!leaf_ptr->contents_)
      continue;
#if 0
    std::stringstream content_bin_file;
    content_bin_file << land_map_folder_ << valid_leaves[i]->get_label_name("land_map_index","all");
    std::string kml_file = vul_file::strip_extension(content_bin_file.str()) + ".kml";
    std::cout << kml_file << std::flush << std::endl;
#endif
    volm_conf_loc_map loc_map = leaf_ptr->contents_->land_locs();
    for (auto & mit : loc_map)
    {
      unsigned char land_id = mit.first;
      std::vector<vgl_point_3d<double> > loc = mit.second;
      for (auto & vit : loc)
      {
        // calculate distance and angle
        double lx, ly, lz;
        lvcs.global_to_local(vit.x(), vit.y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
        double square_dist = lx*lx+ly*ly;
        if (square_dist < square_radius_) {
          double dist = std::sqrt(lx*lx+ly*ly);
          double phi  = std::atan2(ly, lx);
          double height = vit.z();
          values.emplace_back(phi, dist, height, land_id);
#if 0
          bkml_write::write_location(ofs, vgl_point_2d<double>(vgl_point_2d<double>(vit->x(), vit->y())), "loc", "", 0.3);
#endif
        }
      }
    }
  }
#if 0
  // for debug purpose
  bkml_write::close_document(ofs);
#endif
  return true;
}

double volm_conf_2d_indexer::min_dist_from_box_to_pt(vpgl_lvcs lvcs, vgl_box_2d<double> const& box, double const& lon, double const& lat)
{
  // check whether the location is inside leaf or not
  if (box.contains(lon, lat))
    return -1.0;
  // obtain the closest points from the given (lon,lat) to the bounding box
  vgl_polygon<double> poly;
  poly.new_sheet();
  for (unsigned k = 0; k < 4; k++) {
    double vlon, vlat;
    switch (k) {
      case 0:  { vlon = box.min_x();  vlat = box.min_y();  break; }
      case 1:  { vlon = box.max_x();  vlat = box.min_y();  break; }
      case 2:  { vlon = box.max_x();  vlat = box.max_y();  break; }
      case 3:  { vlon = box.min_x();  vlat = box.max_y();  break; }
      default: { vlon = box.min_x();  vlat = box.min_y(); }
    }
    poly.push_back(vlon, vlat);
  }
  vgl_point_2d<double> closest_pt = vgl_closest_point(poly, vgl_point_2d<double>(lon, lat));
  // calculate the minimum distance
  double plx, ply, plz;
  lvcs.global_to_local(lon, lat, 0.0, vpgl_lvcs::wgs84, plx, ply, plz);
  double clx, cly, clz;
  lvcs.global_to_local(closest_pt.x(), closest_pt.y(), 0.0, vpgl_lvcs::wgs84, clx, cly, clz);
  double min_dist = std::sqrt((plx-clx)*(plx-clx)+(ply-cly)*(ply-cly));
  return min_dist;
}
