#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdio>
#include <utility>
#include "volm_satellite_resources.h"

#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_threshold.h>
#include <vil/algo/vil_binary_erode.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_region_finder.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <volm/volm_tile.h>
#include <volm/volm_geo_index2.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_clip.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vpgl/vpgl_lvcs.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bkml/bkml_parser.h>

void add_directories(const std::string& root, std::vector<std::string>& directories) {
  if (vul_file::is_directory(root))
    directories.push_back(root);

  std::string glob = root + "/*"; // get everything directory or not
  vul_file_iterator file_it(glob.c_str());
  ++file_it;  // skip .
  ++file_it;  // skip ..
  while (file_it) {
    std::string name(file_it());
    if (vul_file::is_directory(name))
      add_directories(name, directories);
    ++file_it;
  }

}

void volm_satellite_resources::add_resource(const std::string& name)
{
  volm_satellite_resource res;
  res.full_path_ = name;
  res.name_ = vul_file::strip_directory(name);
  res.name_ = vul_file::strip_extension(res.name_);
  res.meta_ = new brad_image_metadata(name, "");
  if (eliminate_same_) {
    // first check if we already have the exact same image, (unfortunately there are resources with different name but taken at exactly same time, so same image)
    // we define images are same if they have time less than 2 minutes, close enough extent, same name and same band number
    for (auto & resource : resources_) {
      if (resource.meta_->satellite_name_.compare(res.meta_->satellite_name_) == 0 &&
          resource.meta_->band_.compare(res.meta_->band_) == 0 &&
          this->same_time(resource, res) &&
          this->same_view(resource, res) &&
          this->same_extent(resource, res))
      {
          std::cout << "!!!!!!!!!!!!! cannot add: " << res.name_ << " with time: "; res.meta_->print_time();
          std::cout << " view: " << res.meta_->view_azimuth_ << ", " << res.meta_->view_elevation_;
          std::cout << "already exists: \n"   << resource.name_ << " with time: "; resource.meta_->print_time();
          std::cout << " view: " << resource.meta_->view_azimuth_ << ", " << resource.meta_->view_elevation_;
          std::cout << " band of resources: " <<  resource.meta_->band_ << " band trying to add: " << res.meta_->band_ << std::flush << std::endl;
          return;
      }
    }
  }
  if (res.meta_->gsd_ > 0)  // if there are parsing problems, gsd is negative
    resources_.push_back(res);
}

//: x is lon and y is lat in the bbox, construct bbox with min point to be lower left and max to be upper right and as axis aligned with North-East
volm_satellite_resources::volm_satellite_resources(vgl_box_2d<double>& bbox, double min_size, bool eliminate_same) : min_size_(min_size), bbox_(bbox), eliminate_same_(eliminate_same)
{
  this->construct_tree();
}

//: traverse the given path recursively and add each satellite resource
void volm_satellite_resources::add_path(std::string path)
{
  std::vector<std::string> directories;
  add_directories(std::move(path), directories);
  if (!directories.size())
    return;
  std::cout << "found " << directories.size() << " directories!\n";
  unsigned start = resources_.size();
  for (const auto & directorie : directories) {
    std::string glob = directorie + "/*.NTF";
    vul_file_iterator file_it(glob.c_str());
    while (file_it) {
      std::string name(file_it());
      //std::cout << name << "\n";
      this->add_resource(name);
      ++file_it;
    }
    glob = directorie + "/*.ntf";
    vul_file_iterator file_it2(glob.c_str());
    while (file_it2) {
      std::string name(file_it2());
      //std::cout << name << "\n";
      this->add_resource(name);
      ++file_it2;
    }
  }
  unsigned end = resources_.size();
  this->add_resources(start, end);
}

void volm_satellite_resources::construct_tree()
{
  // construct volm_geo_index2 quad tree with 1.0 degree leaves - satellite images are pretty large, so the leaves need to be large
  root_ = volm_geo_index2::construct_tree<std::vector<unsigned> >(bbox_, min_size_);
  std::vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root_, leaves);
  std::cout << " the number of leaves in the quad tree of satellite resources: " << leaves.size() << '\n';
}

void volm_satellite_resources::add_resources(unsigned start, unsigned end) {
  // insert the ids of the resources
  for (unsigned i = start; i < end; i++) {
    std::vector<volm_geo_index2_node_sptr> leaves;
    // CAUTION: x is lat and y is lon in nitf_camera but we want x to be lon and y to be lat, use all the corners of satellite image by inverting x-y to create the bounding box
    vgl_box_2d<double> satellite_footprint;
    satellite_footprint.add(vgl_point_2d<double>(resources_[i].meta_->lower_left_.x(), resources_[i].meta_->lower_left_.y()));
    satellite_footprint.add(vgl_point_2d<double>(resources_[i].meta_->upper_right_.x(), resources_[i].meta_->upper_right_.y()));
    volm_geo_index2::get_leaves(root_, leaves, satellite_footprint);
    for (auto & leave : leaves) {
      auto* leaf_ptr = dynamic_cast<volm_geo_index2_node<std::vector<unsigned> >* >(leave.ptr());
      leaf_ptr->contents_.push_back(i);  // push this satellite image to this leave that intersects its footprint
    }
  }
}

//: get a list of ids in the resources_ list that overlap the given rectangular region
void volm_satellite_resources::query(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, const std::string& band_str, std::vector<unsigned>& ids, double gsd_thres)
{
  vgl_box_2d<double> area(lower_left_lon, upper_right_lon, lower_left_lat, upper_right_lat);
  std::vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root_, leaves, area);
  std::vector<unsigned> temp_ids_init;
  for (auto & leave : leaves) {
    auto* leaf_ptr = dynamic_cast<volm_geo_index2_node<std::vector<unsigned> >* >(leave.ptr());
    // check which images overlap with the given bbox
    for (unsigned int res_id : leaf_ptr->contents_) {
      // CAUTION: x is lat and y is lon in nitf_camera but we want x to be lon and y to be lat, use all the corners of satellite image by inverting x-y to create the bounding box
      vgl_box_2d<double> sat_box;
      sat_box.add(vgl_point_2d<double>(resources_[res_id].meta_->lower_left_.x(), resources_[res_id].meta_->lower_left_.y()));
      sat_box.add(vgl_point_2d<double>(resources_[res_id].meta_->upper_right_.x(), resources_[res_id].meta_->upper_right_.y()));
      if (resources_[res_id].meta_->band_.compare(band_str) == 0 && vgl_area(vgl_intersection(sat_box, area)) > 0)
        temp_ids_init.push_back(res_id);
    }
  }

  // eliminate the ones which does not satisfy the GSD (ground sampling distance) threshold
  std::vector<unsigned> temp_ids;
  for (unsigned int i : temp_ids_init) {
    if (resources_[i].meta_->gsd_ <= gsd_thres)
      temp_ids.push_back(i);
  }

  // order the resources in the order of GeoEye1, WV2, WV1, QB/others
  for (unsigned int temp_id : temp_ids) {
    if (resources_[temp_id].meta_->satellite_name_.compare("GeoEye-1") == 0)
      ids.push_back(temp_id);
  }
  for (unsigned int temp_id : temp_ids) {
    if (resources_[temp_id].meta_->satellite_name_.compare("WorldView") == 0)
      ids.push_back(temp_id);
  }
  for (unsigned int temp_id : temp_ids) {
    if (resources_[temp_id].meta_->satellite_name_.compare("WorldView2") == 0)
      ids.push_back(temp_id);
  }

  std::vector<unsigned> temp_ids2;
  // find the ones that are not already in ids
  for (unsigned int temp_id : temp_ids) {
    bool contains = false;
    for (unsigned int id : ids) {
      if (temp_id == id) {
        contains = true;
        break;
      }
    }
    if (!contains)
      temp_ids2.push_back(temp_id);
  }
  for (unsigned int i : temp_ids2)
    ids.push_back(i);


}
//: query the resources in the given box and output the full paths to the given file
bool volm_satellite_resources::query_print_to_file(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, unsigned& cnt, std::string& out_file, const std::string& band_str, double gsd_thres)
{
  std::vector<unsigned> ids, ids_all;
  query(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, band_str, ids_all, gsd_thres);

  // eliminate the repeating ids, more than one leaf may contain the same resource
  for (unsigned i = 0; i < ids_all.size(); i++) {
    bool contains = false;
    for (unsigned j = i+1; j < ids_all.size(); j++) {
      if (ids_all[i] == ids_all[j]) {
        contains = true;
        break;
      }
    }
    if (!contains)
      ids.push_back(ids_all[i]);
  }

  cnt = ids.size();
  if (out_file.compare("") == 0)
    return true;
  std::ofstream ofs(out_file.c_str());
  if (!ofs) {
    std::cerr << "In volm_satellite_resources::query_print_to_file() -- cannot open file: " << out_file << std::endl;
    return false;
  }
  for (unsigned int id : ids)
    ofs << resources_[id].full_path_ << '\n';
  ofs.close();
  return true;
}

//: query the resources in the given box and output the full paths of randomly selected seeds to the given file,
//  the order of satellites to select seeds from: GeoEye1, WorldView2, WorldView1 and then any others
bool volm_satellite_resources::query_seeds_print_to_file(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, int n_seeds, unsigned& cnt, std::string& out_file, std::string& band_str, double gsd_thres)
{
  std::vector<unsigned> ids;
  double mid_lon = (lower_left_lon + upper_right_lon) / 2;
  double mid_lat = (lower_left_lat + upper_right_lat) / 2;
  double lower_lon = (lower_left_lon + mid_lon) / 2;
  double lower_lat = (lower_left_lat + mid_lat) / 2;
  double upper_lon = (upper_right_lon + mid_lon) / 2;
  double upper_lat = (upper_right_lat + mid_lat) / 2;
  std::cout << "using bbox for scene: " << lower_lon << " " << lower_lat << " " << upper_lon << " " << upper_lat << std::endl;
  query(lower_lon, lower_lat, upper_lon, upper_lat, band_str, ids, gsd_thres);

  // now select n_seeds among these ones
  std::map<std::string, std::vector<unsigned> > possible_seeds;
  std::vector<unsigned> tmp;
  possible_seeds["GeoEye-1"] = tmp;
  possible_seeds["WV01"] = tmp;
  possible_seeds["WV02"] = tmp;
  possible_seeds["other"] = tmp;
  for (unsigned int id : ids) {
    if (resources_[id].meta_->cloud_coverage_percentage_ < 1) {
      auto iter = possible_seeds.find(resources_[id].meta_->satellite_name_);
      if (iter != possible_seeds.end())
        iter->second.push_back(id);
      else
        possible_seeds["other"].push_back(id);
    }
  }
#if 0
  std::cout << "possible seeds:" << std::endl;
  for (std::map<std::string, std::vector<unsigned> >::iterator iter = possible_seeds.begin(); iter != possible_seeds.end(); iter++) {
    std::cout << iter->first << "\n";
    for (unsigned k = 0; k < iter->second.size(); k++) {
      std::cout << "\t\t" << resources_[iter->second[k]].name_ << "\n";
    }
  }
#endif

  std::vector<brad_image_metadata_sptr> selected_names;
#if 0
  std::vector<std::string> names;
  names.push_back("GeoEye-1"); names.push_back("WV02"); names.push_back("WV01"); names.push_back("other");
  for (unsigned ii = 0; ii < names.size(); ii++) {
    std::string name = names[ii];
    for (unsigned i = 0; i < possible_seeds[name].size(); i++) {
      std::cout << resources_[possible_seeds[name][i]].name_;
      std::cout << " time " << name << ": " << resources_[possible_seeds[name][i]].meta_->t_.year;
      std::cout << " " << resources_[possible_seeds[name][i]].meta_->t_.month;
      std::cout << " " << resources_[possible_seeds[name][i]].meta_->t_.day;
      std::cout << " " << resources_[possible_seeds[name][i]].meta_->t_.hour;
      std::cout << " " << resources_[possible_seeds[name][i]].meta_->t_.min;
      std::cout << "\n";
    }
  }
#endif

  std::vector<std::string> seed_paths;

  cnt = 0;
  bool done = false;
  for (unsigned i = 0; i < possible_seeds["GeoEye-1"].size(); i++) {
    // first check if there is a satellite with the same time
    bool exists = false;
    for (auto & selected_name : selected_names) {
      if (resources_[possible_seeds["GeoEye-1"][i]].meta_->same_time(*selected_name.ptr())) {
        exists = true;
        break;
      }
    }
    if (exists) continue;
    selected_names.push_back(resources_[possible_seeds["GeoEye-1"][i]].meta_);

    //ofs << resources_[possible_seeds["GeoEye-1"][i]].name_ << '\n';
    seed_paths.push_back(resources_[possible_seeds["GeoEye-1"][i]].name_);
    //std::cout << resources_[possible_seeds["GeoEye-1"][i]].name_ << '\n';
    cnt++;

    if (cnt == n_seeds) {
     done = true;
     break;
    }
  }
  if (!done) {
    for (unsigned i = 0; i < possible_seeds["WV02"].size(); i++) {

     // first check if there is a satellite with the same time
     bool exists = false;
     for (auto & selected_name : selected_names) {
       if (resources_[possible_seeds["WV02"][i]].meta_->same_time(*selected_name.ptr())) {
         exists = true;
         break;
       }
     }
     if (exists) continue;
     selected_names.push_back(resources_[possible_seeds["WV02"][i]].meta_);

     //ofs << resources_[possible_seeds["WV02"][i]].name_ << '\n';
     seed_paths.push_back(resources_[possible_seeds["WV02"][i]].name_);
     //std::cout << resources_[possible_seeds["WV02"][i]].name_ << '\n';
     cnt++;
     if (cnt == n_seeds) {
       done = true;
       break;
     }
    }
  }
  if (!done) {
    for (unsigned i = 0; i < possible_seeds["WV01"].size(); i++) {

     // first check if there is a satellite with the same time
     bool exists = false;
     for (auto & selected_name : selected_names) {
       if (resources_[possible_seeds["WV01"][i]].meta_->same_time(*selected_name.ptr())) {
         exists = true;
         break;
       }
     }
     if (exists) continue;
     selected_names.push_back(resources_[possible_seeds["WV01"][i]].meta_);

     //ofs << resources_[possible_seeds["WV01"][i]].name_ << '\n';
     seed_paths.push_back(resources_[possible_seeds["WV01"][i]].name_);
     //std::cout << resources_[possible_seeds["WV01"][i]].name_ << '\n';
     cnt++;
     if (cnt == n_seeds) {
       done = true;
       break;
     }
    }
  }
  if (!done) {
    for (unsigned i = 0; i < possible_seeds["other"].size(); i++) {

     // first check if there is a satellite with the same time
     bool exists = false;
     for (auto & selected_name : selected_names) {
       if (resources_[possible_seeds["other"][i]].meta_->same_time(*selected_name.ptr())) {
         exists = true;
         break;
       }
     }
     if (exists) continue;
     selected_names.push_back(resources_[possible_seeds["other"][i]].meta_);

     //ofs << resources_[possible_seeds["other"][i]].name_ << '\n';
     seed_paths.push_back(resources_[possible_seeds["other"][i]].name_);
     //std::cout << resources_[possible_seeds["other"][i]].name_ << '\n';
     cnt++;
     if (cnt == n_seeds) {
       break;
     }
    }
  }

  if (out_file.compare("") == 0)
    return true;

  std::ofstream ofs(out_file.c_str());
  if (!ofs) {
    std::cerr << "In volm_satellite_resources::query_print_to_file() -- cannot open file: " << out_file << std::endl;
    return false;
  }

  for (const auto & seed_path : seed_paths)
    ofs << seed_path << '\n';

  ofs.close();
  return true;
}

//: get a list of ids in the resources_ list that overlap the given rectangular region
unsigned volm_satellite_resources::query_pairs(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, std::string& sat_name, float GSD_thres, std::vector<std::pair<unsigned, unsigned> >& ids)
{
  // first get all the images that intersect the area
  std::vector<unsigned> temp_ids;
  std::string band_str = "PAN";
  this->query(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, band_str, temp_ids,GSD_thres); // we're only interested in which images intersect the box, so pass gsd_thres very high
  std::cout << "there are " << temp_ids.size() << " images that intersect the scene!\n";

  // prune out the ones from the wrong satellite
  std::vector<unsigned> ids2;
  for (unsigned int temp_id : temp_ids) {
    if (resources_[temp_id].meta_->satellite_name_.compare(sat_name) == 0)
      ids2.push_back(temp_id);
  }

  std::cout << "there are " << ids2.size() << " images that intersect the scene from sat: " << sat_name << "!\n";
  // check the time of collection to find pairs
  for (unsigned i = 0; i < ids2.size(); i++) {
    for (unsigned j = i+1; j < ids2.size(); j++) {
      if (!resources_[ids2[i]].meta_->same_day(*(resources_[ids2[j]].meta_)))
        continue;

      std::cout << resources_[ids2[i]].name_ << ": ";
      resources_[ids2[i]].meta_->print_time();
      std::cout << " azi: " << resources_[ids2[i]].meta_->sun_azimuth_ << " elev: " << resources_[ids2[i]].meta_->sun_elevation_ << " res: " << resources_[ids2[i]].meta_->gsd_ << std::endl;
      std::cout << resources_[ids2[j]].name_ << ": ";
      resources_[ids2[j]].meta_->print_time();
      std::cout << " azi: " << resources_[ids2[j]].meta_->sun_azimuth_ << " elev: " << resources_[ids2[j]].meta_->sun_elevation_ << " res: " << resources_[ids2[j]].meta_->gsd_ << std::endl;

      unsigned time_dif = resources_[ids2[i]].meta_->time_minute_dif(*(resources_[ids2[j]].meta_));
      if (time_dif > 0 && time_dif < 5) { // if taken less than 5 minute apart
        //ids.push_back(std::pair<unsigned, unsigned>(ids2[i], ids2[j]));
        // check if one of the images have already been pushed
        bool already_added = false;
        for (auto & id : ids) {
          if (resources_[ids2[i]].meta_->same_time(*(resources_[id.first].meta_)) ||
              resources_[ids2[i]].meta_->same_time(*(resources_[id.second].meta_))) {
                already_added = true;
                break;
          }
        }
        if (!already_added) {
          ids.emplace_back(ids2[i], ids2[j]);
          std::cout << "\t\t !!!!!! PUSHED! time dif: " << time_dif << " mins within threshold [1, 5] mins!\n";
        }
      }
      std::cout << "\n";
      }
    }
  return ids.size();
}

//: query the resources in the given box and output the full paths of pairs to the given file
bool volm_satellite_resources::query_pairs_print_to_file(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, float GSD_thres, unsigned& cnt, std::string& out_file, std::string& sat_name)
{
  std::vector<std::pair<unsigned, unsigned> > ids;
  cnt = query_pairs(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, sat_name, GSD_thres, ids);

  std::ofstream ofs(out_file.c_str());
  if (!ofs) {
    std::cerr << "In volm_satellite_resources::query_pairs_print_to_file() -- cannot open file: " << out_file << std::endl;
    return false;
  }

  for (auto & id : ids) {
    ofs << resources_[id.first].full_path_ << '\n';
    ofs << resources_[id.second].full_path_ << "\n\n";
  }

  ofs.close();
  return true;
}

//: return the full path of a satellite image given its name, if not found returns empty string
std::pair<std::string, std::string> volm_satellite_resources::full_path(const std::string& name)
{
  for (auto & resource : resources_) {
    if (name.compare(resource.name_) == 0) {
      std::pair<std::string, std::string> p(resource.full_path_, resource.meta_->satellite_name_);
      return p;
    }
  }
  return std::pair<std::string, std::string>("", "");
}

std::string volm_satellite_resources::find_pair(std::string const& name, double const& tol)
{
  for (auto & resource : resources_) {
    if (name.compare(resource.name_) == 0) {
      if (resource.pair_.compare("") == 0) {  // not yet found
        // find all the resources that overlaps with this one
        std::string band_str = resource.meta_->band_;
        std::string other_band;
        if (band_str.compare("PAN") == 0)
          other_band = "MULTI";
        else
          other_band = "PAN";
        std::vector<unsigned> ids;
        this->query(resource.meta_->lower_left_.x(),
                    resource.meta_->lower_left_.y(),
                    resource.meta_->upper_right_.x(),
                    resource.meta_->upper_right_.y(), other_band, ids,10.0);  // pass gsd_thres very high, only interested in finding all the images
        std::cout << " there are " << ids.size() << " resources that cover the image!\n";
        for (unsigned int ii : ids) {
          if (resource.meta_->satellite_name_.compare(resources_[ii].meta_->satellite_name_) == 0 &&  // same satellite good!
              this->same_time(resource, resources_[ii]) &&
              this->same_view(resource, resources_[ii]) &&
              this->same_extent(resource, resources_[ii], tol) )
          {
                  resource.pair_ = resources_[ii].name_;
                  resources_[ii].pair_ = resource.name_;
                return resources_[ii].name_;
          }
        }
      } else
        return resource.pair_;
     break;
    }
  }
  return "";
}

void
volm_satellite_resources::convert_to_global_footprints(std::vector<vgl_polygon<double> >& footprints, const vpgl_lvcs_sptr& lvcs,
  const std::vector<vgl_polygon<double> >& lvcs_footprints, float downsample_factor)
{
  footprints = lvcs_footprints;

  // rescale by GSD
  for(auto & footprint : footprints) {
    assert(footprint.num_sheets() == 1);
    for (auto & p : footprint[0]) {
      p.x() *= downsample_factor;
      p.y() *= downsample_factor;
    }
  }

  // convert to global lon/lat coordinate system
  for(auto & footprint : footprints) {
    assert(footprint.num_sheets() == 1);
    for (auto & p : footprint[0]) {
      double x = p.x();
      double y = p.y();
      double lon,lat,gz;
      lvcs->local_to_global(x, y, 0,
                            lvcs->get_cs_name(), // this is output global cs
                            lon, lat, gz,
                            lvcs->geo_angle_unit(), lvcs->local_length_unit());
      p = vgl_point_2d<double>(lon,lat);
    }
  }
}

void
volm_satellite_resources::convert_to_local_footprints(vpgl_lvcs_sptr& lvcs, std::vector<vgl_polygon<double> >& lvcs_footprints,
  const std::vector<vgl_polygon<double> >& footprints, float downsample_factor)
{
  lvcs_footprints.clear();

  // if no lvcs was provided, create one
  if(!lvcs) {
    // find the lower-left lat/lon
    double min_lon=181.0, min_lat=91.0;
    for(const auto & footprint : footprints) {
      assert(footprint.num_sheets() == 1);
      for (auto p : footprint[0]) {
        min_lon = std::min(p.x(), min_lon);
        min_lat = std::min(p.y(), min_lat);
      }
    }

    // create an lvcs coordinate system
    lvcs = new vpgl_lvcs(min_lat, min_lon, 0, vpgl_lvcs::str_to_enum("wgs84"), vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  }

  // convert to the lvcs coordinate system
  //std::vector<vgl_polygon<double> > lvcs_footprints;
  for(const auto & footprint : footprints) {
    vgl_polygon<double> lvcs_footprint(1);
    assert(footprint.num_sheets() == 1);
    for (auto p : footprint[0]) {
      double lon = p.x();
      double lat = p.y();
      double x,y,z;
      lvcs->global_to_local(lon, lat, 0, lvcs->get_cs_name(), x, y, z, lvcs->geo_angle_unit(), lvcs->local_length_unit());
      lvcs_footprint.push_back(x,y);
    }
    lvcs_footprints.push_back(lvcs_footprint);
  }

  // rescale by GSD
  for(auto & footprint : lvcs_footprints) {
    assert(footprint.num_sheets() == 1);
    for (auto & p : footprint[0]) {
      p.x() /= downsample_factor;
      p.y() /= downsample_factor;
    }
  }
}

// this is creating a raster, so ensure the union of the footprints does not cover a huge area
void
volm_satellite_resources::compute_footprints_heatmap(vil_image_view<unsigned>& heatmap, vgl_box_2d<double>& image_window,
  const std::vector<vgl_polygon<double> >& footprints)
{
  // find the lower-left & upper-right coordinates to create the image region
  double min_x, min_y, max_x, max_y;
  if(footprints.size() > 0 && footprints[0].num_vertices() > 0) { // not the most ideal way to initilize this...
    const vgl_polygon<double>& footprint = footprints[0];

    unsigned i = 0;
    while(footprint[i].size() == 0) ++i;
    min_x = max_x = footprint[i][0].x();
    min_y = max_y = footprint[i][0].y();
  }

  for(const auto & footprint : footprints) {
    assert(footprint.num_sheets() == 1);
    for (auto p : footprint[0]) {
      min_x = std::min(p.x(), min_x);
      min_y = std::min(p.y(), min_y);
      max_x = std::max(p.x(), max_x);
      max_y = std::max(p.y(), max_y);
    }
  }
  std::cout << "max_x,max_y,max_x,max_y: " << min_x << "," << min_y << "," << max_x << "," << max_y << std::endl;

  unsigned ncols = (int)std::ceil(max_x - min_x);
  unsigned nrows = (int)std::ceil(max_y - min_y);

  // create the heatmap of intersecting polygons
  assert(ncols != 0 && nrows != 0);
  heatmap.set_size(ncols, nrows, 1);
  heatmap.fill(0);
  // define window as image size to prevent out-of-range in case of bad polygons
  image_window = vgl_box_2d<double>(min_x, max_x, min_y, max_y);
  vil_image_view<bool> object_mask;
  for(const auto & footprint : footprints) {
    rasterize(image_window, footprint, object_mask);
    vil_math_image_sum(heatmap, object_mask, heatmap);
  }

#ifdef DEBUG_HIGHLY_OVERLAPPING_SAT_RESOURCES
  vil_image_view<vxl_byte> dest;
  vil_convert_stretch_range(heatmap, dest);
  vil_save(dest, "./heatmap.png");
#endif
}

template<class T> struct index_cmp {
  index_cmp(const T arr) : arr(arr) {}
  bool operator()(const size_t a, const size_t b) const { return arr[a] > arr[b]; }
  const T arr;
};

void
volm_satellite_resources::query_resources(std::vector<vgl_polygon<double> >& footprints, std::vector<unsigned>& footprint_ids,
  const volm_satellite_resources_sptr& res, const std::string& kml_file, const std::string& band, double gsd_thres)
{
  // parse the polygon and construct its bounding box
  if (!vul_file::exists(kml_file)) {
    std::cout << "can not find input kml file: " << kml_file << std::endl;
    return;
  }
  vgl_polygon<double> poly = bkml_parser::parse_polygon(kml_file);
  vgl_box_2d<double> bbox;
  for (auto i : poly[0])
    bbox.add(i);
  double lower_left_lon  = bbox.min_x();
  double lower_left_lat  = bbox.min_y();
  double upper_right_lon = bbox.max_x();
  double upper_right_lat = bbox.max_y();
  std::vector<unsigned> ids;
  res->query(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, band, ids, gsd_thres);

  // eliminate the repeating ids (maintaining original order), more than one leaf may contain the same resource
  // this mangles the expected order...
  //std::set<unsigned> s(ids.begin(),ids.end());
  //ids.assign(s.begin(), s.end());
  std::vector<unsigned>& deduped_ids = footprint_ids;
  for (unsigned i = 0; i < ids.size(); i++) {
    bool contains = false;
    for (unsigned j = i+1; j < ids.size(); j++) {
      if (ids[i] == ids[j]) {
        contains = true;
        break;
      }
    }
    if (!contains) {
      deduped_ids.push_back(ids[i]);
    }
  }

  // filter footprints
  for(unsigned int deduped_id : deduped_ids) {
    footprints.push_back(resources_[deduped_id].meta_->footprint_);
    //std::cout << resources_[deduped_ids[res_id]].name_ << std::endl;
  }
  std::cout << "footprints: " << footprints.size() << std::endl;
}

void
volm_satellite_resources::highly_overlapping_resources(std::vector<std::string>& overlapping_res, const volm_satellite_resources_sptr& res,
  const std::string& kml_file, float downsample_factor, const std::string& band, double gsd_thres)
{
  // query_resources
  std::vector<vgl_polygon<double> > footprints;
  std::vector<unsigned> resource_ids;
  query_resources(footprints, resource_ids, res, kml_file, band, gsd_thres);

  std::vector<unsigned> filtered_ids;
  res->highly_overlapping_resources(filtered_ids, footprints, downsample_factor);
  //for(int i=0; i < filtered_ids.size(); ++i) {
  //  std::cout << filtered_ids[i] << ",";
  //}
  //std::cout << std::endl;

  for(unsigned int filtered_id : filtered_ids) {
    overlapping_res.push_back(resources_[resource_ids[filtered_id]].full_path_);
  }
}

void
volm_satellite_resources::highly_overlapping_resources(std::vector<unsigned>& overlapping_ids,
  const std::vector<vgl_polygon<double> >& footprints, float downsample_factor)
{
  // be careful with coordinate transforms

  // convert to local coordinates
  vpgl_lvcs_sptr lvcs;
  std::vector<vgl_polygon<double> > lvcs_footprints;
  convert_to_local_footprints(lvcs, lvcs_footprints, footprints, downsample_factor);

  // compute the heatmap of the resource (nitf) footprints
  vil_image_view<unsigned> heatmap;
  vgl_box_2d<double> lvcs_window;
  compute_footprints_heatmap(heatmap, lvcs_window, lvcs_footprints);
  unsigned int nrows = heatmap.nj();
  unsigned int ncols = heatmap.ni();


  // compute the set of highly overlapping regions
  float best_score = 0;
  unsigned best_nimages = 0;
  vgl_polygon<double> best_region_hull;

  vil_image_view<bool> mask;
  for(unsigned nimages=1; nimages < lvcs_footprints.size(); ++nimages) {
    // threshold image
    vil_threshold_above(heatmap, mask, nimages);
//#define DEBUG_HIGHLY_OVERLAPPING_SAT_RESOURCES
#ifdef DEBUG_HIGHLY_OVERLAPPING_SAT_RESOURCES
    vil_image_view<vxl_byte> dest;
    vil_convert_stretch_range(mask, dest);
    char path[256];
    std::sprintf(path, "./masks/mask_%u.png", nimages);
    vil_save(dest, path);
#endif
    // find regions in the mask
    vil_image_view<bool> to_process;
    to_process.deep_copy(mask);
    // traverse image, looking for pixels in regions yet to process
    for (unsigned int j=0; j<nrows; ++j) {
      for (unsigned int i=0; i<ncols; ++i) {
        if (!to_process(i,j)) {
          continue;
        }
        // found a new region - extract all connected pixels
        // (i'm using vil_region_finder a little differently than it seems it was
        // designed; i'll keep track of what to process externally and use the
        // boolean_region_image as the binary mask).
        vil_region_finder<bool> region_finder(mask, vil_region_finder_4_conn);
        std::vector<unsigned> ri,rj;
        region_finder.same_int_region(i,j,ri,rj);
        // get the binary mask
        vil_image_view<bool> region_mask;
        region_mask = region_finder.boolean_region_image();
        // mark all pixels in region as processed and compute mean score
        for (std::vector<unsigned>::const_iterator rj_it=rj.begin(), ri_it=ri.begin();
            rj_it!=rj.end(); ++rj_it, ++ri_it) {
          to_process(*ri_it, *rj_it) = false;
        }
        //const unsigned region_mask_area = ri.size();

        // compute the convex hull of each region
        vgl_polygon<double> region_hull = calculate_convex_hull(region_mask);

        // compute a score for each region based on its density and the number of contributing images
        double compactness_score = compactness(region_hull, region_mask) * nimages*nimages;
        //std::cout << compactness_score << std::endl;

        // find the best score
        if(compactness_score > best_score) {
          best_score = compactness_score;
          best_region_hull = region_hull;
          //std::cout << best_region_hull << std::endl;
        }
      }
    }
  }


  // find the image ids associated with the best scoring region
  //std::cout << "at least " << best_nimages << " footprints intersecting" << std::endl;
  //std::cout << best_region_hull << std::endl;

  //vgl_point_2d<double> centroid = vgl_centroid(best_region_hull);
  // map image coordinates to lvcs coordinates
  //vgl_point_2d<double> lvcs_centroid(centroid.x()-lvcs_window.min_x(), centroid.y()-lvcs_window.min_y());
  //std::cout << lvcs_centroid.x() << "," << lvcs_centroid.y() << std::endl;
  // map lvcs coordinates to global coordinates
  //double lon, lat, gz;
  //lvcs->local_to_global(lvcs_centroid.x()*downsample_factor, lvcs_centroid.y()*downsample_factor, 0,
  //                      lvcs->get_cs_name(), // this is output global cs
  //                      lon, lat, gz,
  //                      lvcs->geo_angle_unit(), lvcs->local_length_unit());
  //
  //for(unsigned i=0; i < lvcs_footprints.size(); ++i) {
  //  vgl_polygon<double>& footprint = lvcs_footprints[i];
  //
  //  if(footprint.contains(lvcs_centroid)) {
  //    //std::cout << footprint << std::endl;
  //    overlapping_ids.push_back(i);
  //  }
  //}

  // unfortunately, there is no function to compute the intersection of two polygons, or even the
  // intersection of a polygon and a box (there is a test for this, but it returns true/false, not
  // a polygon); so instead, i rasterize...
  // RE: not true anymore
  vil_image_view<bool> best_region_mask;
  rasterize(lvcs_window, best_region_hull, best_region_mask);
  unsigned int best_region_mask_area;
  vil_math_sum(best_region_mask_area, best_region_mask, 0);

  std::vector<double> covered_areas;
  for(auto & footprint : lvcs_footprints) {
    vil_image_view<bool> mask;
    rasterize(lvcs_window, footprint, mask);

    vil_image_view<bool> intersection_mask;
    vil_math_image_product(best_region_mask, mask, intersection_mask);
    unsigned int intersection_mask_area;
    vil_math_sum(intersection_mask_area, intersection_mask, 0);
    covered_areas.push_back(intersection_mask_area/(double)best_region_mask_area);
  }
  overlapping_ids.resize(covered_areas.size());
  for(size_t i=0; i < overlapping_ids.size(); ++i) { overlapping_ids[i] = i; }
  std::sort(overlapping_ids.begin(), overlapping_ids.end(), index_cmp<std::vector<double>&>(covered_areas));

  double best_area = covered_areas[overlapping_ids[0]];
  unsigned i=3;
  for( ; i<covered_areas.size(); ++i) {
    if(covered_areas[overlapping_ids[i]] < best_area*.95) {
      break;
    }
  }
  overlapping_ids.erase(overlapping_ids.begin()+i,overlapping_ids.end());


  // map lvcs coordinates to global coordinates
  //std::vector<vgl_polygon<double> > footprints;
  //convert_to_global_footprints(footprints, lvcs, lvcs_footprints, downsample_factor);
}

void
volm_satellite_resources::rasterize(
    const vgl_polygon<double> &bounds, vil_image_view<bool> &mask)
{
  double min_x, max_x, min_y, max_y;
  for (unsigned int s=0; s < bounds.num_sheets(); ++s) {
    for (unsigned int p=0; p < bounds[s].size(); ++p) {
      if(s==0 && p==0) { // not the most ideal way to initilize this...
        min_x = max_x = bounds[0][0].x();
        min_y = max_y = bounds[0][0].y();
      }

      min_x = std::min(bounds[s][p].x(), min_x);
      min_y = std::min(bounds[s][p].y(), min_y);
      max_x = std::max(bounds[s][p].x(), max_x);
      max_y = std::max(bounds[s][p].y(), max_y);
    }
  }

  vgl_box_2d<double> window(min_x, max_x, min_y, max_y);

  rasterize(window, bounds, mask);
}

void
volm_satellite_resources::rasterize(
    const vgl_box_2d<double> &bbox_clipped, const vgl_polygon<double> &bounds, vil_image_view<bool> &mask)
{
  if(mask.ni() != bbox_clipped.width() && mask.nj() != bbox_clipped.height() && mask.nplanes() != 1) {
    assert(!"mask not formated properly");
  } else {
    mask.set_size((int)std::ceil(bbox_clipped.width()), (int)std::ceil(bbox_clipped.height()), 1);
    mask.fill(false);
  }
  // NOTE sgr - there is a bug/instability in vgl_polygon_scan_iterator that sometimes
  // causes a scanline of pixels outside the convex hull to be included in the mask.
  // setting this to false avoids the bug
  const bool include_boundary = false;
  vgl_polygon_scan_iterator<double> polyIter(bounds, include_boundary, bbox_clipped);
  for (polyIter.reset(); polyIter.next(); ) {
    // Y position in the mask needs to be relativized
    int y_shifted = polyIter.scany() - bbox_clipped.min_y();
    for (int x = polyIter.startx(); x <= polyIter.endx(); x++) {
      int x_shifted = x - bbox_clipped.min_x();
      mask(x_shifted, y_shifted) = true;
    }
  }
}

double
volm_satellite_resources::compactness(vgl_polygon<double> const&poly, vil_image_view<bool> const& mask)
{
  double region_hull_area = vgl_area(poly);
  if(region_hull_area == 0) return 0.0;

  unsigned int region_mask_area;
  vil_math_sum(region_mask_area, mask, 0);

  // region_mask_area <= region_hull_area
  double compactness_score = region_hull_area * (region_mask_area / region_hull_area);

  return compactness_score;
}

vgl_polygon<double>
volm_satellite_resources::calculate_convex_hull(vil_image_view<bool> const& mask, unsigned off_x, unsigned off_y)
{
  const unsigned ni = mask.ni();
  const unsigned nj = mask.nj();

  // determine the boundary points (can't remember why I do this...)
  vil_structuring_element strel;
  strel.set_to_disk(1.1); // set radius to 1.1 to make sure pixels dist 1 away are included
  vil_image_view<bool> mask_eroded(ni,nj);
  // NOTE the erode operation could remove (very) thin elements
  vil_binary_erode(mask, mask_eroded, strel, vil_border_create_constant(mask, false));

  // find non-zero points
  std::vector<vgl_point_2d<double> > boundary_pts;
  for (unsigned bj=0; bj<nj; ++bj) {
    for (unsigned bi=0; bi<ni; ++bi) {
      if (mask(bi,bj) && !mask_eroded(bi,bj)) {
        boundary_pts.emplace_back((double)bi + off_x,(double)bj + off_y);
      }
    }
  }
  if (boundary_pts.size() < 3) {
    //std::cerr << "boundary points < 3" << std::endl;
    assert(boundary_pts.size() > 0);
    // if not debugging, just return empty polygon
    return vgl_polygon<double>();
  }
  // compute convex hull
  vgl_convex_hull_2d<double> compute_hull(boundary_pts);
  vgl_polygon<double> poly = compute_hull.hull();

  return poly;
}

// from http://www.drdobbs.com/the-standard-librarian-defining-iterato/184401331?pgno=3 and
// http://www.cs.helsinki.fi/u/tpkarkka/alglib/k06/lectures/iterators.html#example-simple-list-continued
// with modifications
class CombinatorGenerator
{
public:
  CombinatorGenerator(unsigned N, unsigned K) : N(N), K(K) { }

private:
  struct Combinator
  {
    Combinator(unsigned N=0, unsigned K=0) : N(N), K(K) {
      bitmask = std::string(K,1); // K leading 1's
      bitmask.resize(N,0);       // N-K trailing 0's
      for (unsigned i = 0; i < N; ++i) { // [0..N-1] integers
        if (bitmask[i]) { inds.push_back(i); }
      }
    }

    bool operator==(const Combinator& other) const {
      return bitmask == other.bitmask; // inds is derived from bitmask
    }
    bool operator!=(const Combinator& other) const {
      return !(this == &other);
    }
    Combinator& operator++() {
      if(!std::prev_permutation(bitmask.begin(), bitmask.end())) {
        *this = Combinator(); // null-object pattern
        return *this;
      }

      inds.clear();
      for (unsigned i = 0; i < N; ++i) {
        if (bitmask[i]) { inds.push_back(i); }
      }
      return *this;
    }

    std::vector<unsigned> inds;
  private:
    unsigned N, K;
    std::string bitmask;
  };

  struct Iterator : std::iterator<std::forward_iterator_tag, std::vector<unsigned> >
  {
    Combinator asg_;

    typedef Iterator self_type;

    explicit Iterator(Combinator asg) : asg_(std::move(asg)) { }
    // implicit copy constructor, copy assignment and destructor
    reference operator*() { return asg_.inds; }
    pointer operator->() { return &*(*this); }
    self_type& operator++() { ++asg_; return *this; }
    self_type operator++(int  /*junk*/) { self_type i = *this; ++asg_; return i; }
    bool operator==(const self_type& rhs) const { return asg_ == rhs.asg_; }
    bool operator!=(const self_type& rhs) const { return !(*this == rhs); }
  };

  struct ConstIterator : std::iterator<std::forward_iterator_tag, std::vector<unsigned>,
      std::ptrdiff_t, const std::vector<unsigned>*, const std::vector<unsigned>& >
  {
    Combinator asg_;

    typedef ConstIterator self_type;

    explicit ConstIterator(Combinator asg) : asg_(std::move(asg)) { }
    // implicit copy constructor, copy assignment and destructor
    // a non-const object calling begin() will return an iterator. implicitly
    // convert it if a const_iterator is desired
    ConstIterator(const Iterator& i) : asg_(i.asg_) { }
    reference operator*() { return asg_.inds; }
    pointer operator->() { return &*(*this); }
    self_type& operator++() { ++asg_; return *this; }
    self_type operator++(int  /*junk*/) { self_type i = *this; ++asg_; return i; }
    bool operator==(const self_type& rhs) const { return asg_ == rhs.asg_; }
    bool operator!=(const self_type& rhs) const { return !(*this == rhs); }
  };

public:
  Iterator begin() {
    return Iterator(Combinator(N,K));
  }

  Iterator end() {
    return Iterator(Combinator());
  }

  ConstIterator begin() const {
    return ConstIterator(Combinator(N,K));
  }

  ConstIterator end() const {
    return ConstIterator(Combinator());
  }

public:
  typedef Iterator iterator;
  typedef ConstIterator const_iterator;

private:
  unsigned N, K;
};

void
volm_satellite_resources::highly_intersecting_resources(std::vector<std::string>& overlapping_res, const volm_satellite_resources_sptr& res,
  const std::string& kml_file, int k, int l, const std::string& band, double gsd_thres)
{
  // query_resources
  std::vector<vgl_polygon<double> > footprints;
  std::vector<unsigned> resource_ids;
  query_resources(footprints, resource_ids, res, kml_file, band, gsd_thres);

  // convert to local coordinates
  vpgl_lvcs_sptr lvcs;
  std::vector<vgl_polygon<double> > lvcs_footprints;
  convert_to_local_footprints(lvcs, lvcs_footprints, footprints);

  std::vector<unsigned> filtered_ids;
  res->highly_intersecting_resources(filtered_ids, lvcs_footprints, k, l);
  //for(int i=0; i < filtered_ids.size(); ++i) {
  //  std::cout << filtered_ids[i] << ",";
  //}
  //std::cout << std::endl;

  for(unsigned int filtered_id : filtered_ids) {
    overlapping_res.push_back(resources_[resource_ids[filtered_id]].full_path_);
  }
}

void
volm_satellite_resources::highly_intersecting_resources(std::vector<unsigned>& overlapping_ids,
  const std::vector<vgl_polygon<double> >& footprints, unsigned k, unsigned l)
{
  double max_area = 0.0;
  std::list<std::pair<std::vector<unsigned>, double> > areas;
  // could be memory inefficient; TODO prune
  // RE only need to keep the previous level of indices (len(inds)-1)
  std::map<std::vector<unsigned>, vgl_polygon<double> > cache;

  unsigned n = footprints.size();

  // compute the rising powerset from k to l
  for(unsigned kk=k; kk < l; ++kk) {
    CombinatorGenerator combs(n, kk);

    CombinatorGenerator::const_iterator it=combs.begin();
    CombinatorGenerator::const_iterator end=combs.end();
    for( ; it != end; ++it) {
      const std::vector<unsigned>& inds = *it;
      //for(int i=0; i < inds.size(); ++i)
      //  std::cout << inds[i] << " ";
      //std::cout << std::endl;

      if(inds.size() < 2) {
        continue;
      }
      unsigned i = 0;
      vgl_polygon<double> intersection = footprints[inds[0]];
      if(inds.size() == 2) {
        i = 1;
      }
      else if(inds.size() <= kk) {
        for(i=inds.size()-2; i>0; --i) { // full sub-index cannot already be processed
          std::vector<unsigned> sub_inds(&inds[0], &inds[i+1]);
          if(cache.find(sub_inds) != cache.end()) {
            intersection = cache[sub_inds];
            i++;
            break;
          }
        }
      }
      else { // must have k+1 footprints to intersect
        i = inds.size()-2;
        std::vector<unsigned> sub_inds(&inds[0], &inds[i+1]);
        if(cache.find(sub_inds) != cache.end()) {
          intersection = cache[sub_inds];
          i++;
        }
        // if the sub-index is not in the cache, it is because the intersection area was
        // smaller than already observed, so just continue
        else {
          continue;
        }
      }

      double area = 0;
      for(unsigned j=i; j < inds.size(); ++j) {
        unsigned ind = inds[j];
        // compute the intersection of all images in the set
        intersection = vgl_clip( intersection, footprints[ind], vgl_clip_type_intersect );
        unsigned nimages = inds.size();
        area = vgl_area(intersection)*nimages*nimages;
        // if this has a smaller area than we've already seen, just abort
        if(area <= max_area) {
          break;
        }
        else {
          std::vector<unsigned> sub_inds(&inds[0], &inds[j+1]);
          cache[sub_inds] = intersection;
        }
      }

      // is this a larger area than what we've alread seen
      if(area > max_area) {
        max_area = area;
        areas.emplace_back(inds,area);
      }
    }
  }


  overlapping_ids = areas.back().first;
  //for(std::list<std::pair<std::vector<unsigned>, double> >::const_iterator it=areas.begin();
  //    it != areas.end(); ++it) {
  //  const std::vector<unsigned>& inds = it->first;
  //  double area = it->second;
  //  std::cout << "(";
  //  for(int i=0; i<inds.size(); ++i) {
  //    std::cout << inds[i] << ",";
  //  }
  //  std::cout << "), " << area << std::endl;
  //}
}

bool volm_satellite_resources::same_time(volm_satellite_resource const& res_a, volm_satellite_resource const& res_b, float const& diff_in_sec)
{
  if (!res_a.meta_->same_day(*res_b.meta_))
    return false;
  // calculate the time difference in seconds
  unsigned time_min_diff = res_a.meta_->time_minute_dif(*res_b.meta_);
  auto second_diff = (unsigned)std::abs(res_a.meta_->t_.sec - res_b.meta_->t_.sec);
  float time_second_diff = second_diff + 60.0 * time_min_diff;
  if (time_second_diff < diff_in_sec) {
    return true;
  }
  return false;
}

// compare the viewing elevation and azimuth angle of two satellite image
bool volm_satellite_resources::same_view(volm_satellite_resource const& res_a, volm_satellite_resource const& res_b, double const & tol)
{
  if (std::abs(res_a.meta_->view_elevation_ - res_b.meta_->view_elevation_) > tol)
    return false;
  if (std::abs(res_a.meta_->view_azimuth_ - res_b.meta_->view_azimuth_) > tol)
    return false;
  return true;
}

// compare the lat, lon bounding boxes to check how close of the footprint of two satellite resources
bool volm_satellite_resources::same_extent(volm_satellite_resource const& res_a, volm_satellite_resource const& res_b, double const& tol)
{
  // check by compare lower left corner and upper right corner with a local lvcs
  vgl_point_2d<double> ll_a(res_a.meta_->lower_left_.x(),  res_a.meta_->lower_left_.y());
  vgl_point_2d<double> ur_a(res_a.meta_->upper_right_.x(), res_a.meta_->upper_right_.y());
  vgl_point_2d<double> ll_b(res_b.meta_->lower_left_.x(),  res_b.meta_->lower_left_.y());
  vgl_point_2d<double> ur_b(res_b.meta_->upper_right_.x(), res_b.meta_->upper_right_.y());
  vpgl_lvcs ll_lvcs(ll_a.y(), ll_a.x(), 0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  double ll_x, ll_y, ll_z;
  ll_lvcs.global_to_local(ll_b.x(), ll_b.y(), 0.0, vpgl_lvcs::wgs84, ll_x, ll_y, ll_z);
  double ll_dist = std::sqrt(ll_x*ll_x + ll_y*ll_y);
  if (ll_dist > tol) {
    return false;
  }

  vpgl_lvcs ur_lvcs(ur_a.y(), ur_a.x(), 0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  double ur_x, ur_y, ur_z;
  ur_lvcs.global_to_local(ur_b.x(), ur_b.y(), 0.0, vpgl_lvcs::wgs84, ur_x, ur_y, ur_z);
  double ur_dist = std::sqrt(ur_x*ur_x + ur_y*ur_y);
  // the footprints are allowed to have 30 meter shift (e.g. PAN and MULTI band)
  if (ur_dist > tol) {
    return false;
  }
  return true;
}

//: binary save self to stream
void volm_satellite_resources::b_write(vsl_b_ostream& os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, min_size_);
  vsl_b_write(os, bbox_.min_x());
  vsl_b_write(os, bbox_.min_y());
  vsl_b_write(os, bbox_.max_x());
  vsl_b_write(os, bbox_.max_y());
  vsl_b_write(os, resources_.size());
  for (const auto & resource : resources_) {
    resource.b_write(os);
  }
}

//: binary load self from stream
void volm_satellite_resources::b_read(vsl_b_istream& is)
{
  if (!is) return;
  short ver;
  vsl_b_read(is, ver);
  if (ver == 0) {
    vsl_b_read(is, min_size_);
    double min_x, min_y, max_x, max_y;
    vsl_b_read(is, min_x);
    vsl_b_read(is, min_y);
    vsl_b_read(is, max_x);
    vsl_b_read(is, max_y);
    bbox_ = vgl_box_2d<double>(min_x, max_x, min_y, max_y);
    unsigned size;
    vsl_b_read(is, size);
    for (unsigned i = 0; i < size; i++) {
      volm_satellite_resource r;
      r.b_read(is);
      resources_.push_back(r);
    }
    this->construct_tree();
    this->add_resources(0, resources_.size());
  }
  else {
    std::cout << "volm_satellite_resources -- unknown binary io version " << ver << '\n';
    return;
  }
}

//: binary save self to stream
void volm_satellite_resource::b_write(vsl_b_ostream& os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, full_path_);
  vsl_b_write(os, name_);
  vsl_b_write(os, pair_);
  meta_->b_write(os);
}

//: binary load self from stream
void volm_satellite_resource::b_read(vsl_b_istream& is)
{
  if (!is) return;
  short ver;
  vsl_b_read(is, ver);
  if (ver == 0) {
    vsl_b_read(is, full_path_);
    vsl_b_read(is, name_);
    vsl_b_read(is, pair_);
    brad_image_metadata meta;
    meta.b_read(is);
    meta_ = new brad_image_metadata(meta);
  }
  else {
    std::cout << "volm_satellite_resources -- unknown binary io version " << ver << '\n';
    return;
  }
}


//dummy vsl io functions to allow volm_satellite_resources to be inserted into
//brdb as a dbvalue
void vsl_b_write(vsl_b_ostream &  /*os*/, volm_satellite_resources const & /*tc*/)
{ /* do nothing */ }
void vsl_b_read(vsl_b_istream &  /*is*/, volm_satellite_resources & /*tc*/)
{ /* do nothing */ }
void vsl_print_summary(std::ostream & /*os*/, const volm_satellite_resources & /*tc*/)
{ /* do nothing */ }
void vsl_b_read(vsl_b_istream&  /*is*/, volm_satellite_resources*  /*tc*/)
{ /* do nothing */ }
void vsl_b_write(vsl_b_ostream&  /*os*/, const volm_satellite_resources* & /*tc*/)
{ /* do nothing */ }
void vsl_print_summary(std::ostream&  /*os*/, const volm_satellite_resources* & /*tc*/)
{ /* do nothing */ }
void vsl_b_read(vsl_b_istream&  /*is*/, volm_satellite_resources_sptr&  /*tc*/)
{ /* do nothing */ }
void vsl_b_write(vsl_b_ostream&  /*os*/, const volm_satellite_resources_sptr & /*tc*/)
{ /* do nothing */ }
void vsl_print_summary(std::ostream&  /*os*/, const volm_satellite_resources_sptr & /*tc*/)
{ /* do nothing */ }


// create table with the increments to use during hypotheses generation according to each land type, the unit is in meters
std::map<std::string, float> create_satellite_reliability()
{
  std::map<std::string, float> m;
  m["GeoEye-1"]    = 0.7f;  // CE90 is 2 meter
  m["WV01"]    = 0.125f;  // CE90 is up to 12 meter
  m["WV02"]    = 0.125f;  // CE90 is up to 12 meter
  m["QB1"]    = 0.05f;  // CE90 is up to 23 meter
  /*m["GeoEye-1"]    = 0.9f;  // CE90 is 2 meter
  m["WV01"]    = 0.04f;  // CE90 is up to 12 meter
  m["WV02"]    = 0.05f;  // CE90 is up to 12 meter
  m["QB1"]    = 0.01f;  // CE90 is up to 23 meter*/
  return m;
}

//: use the corresponding global reliability for each satellite when setting weights for camera correction
std::map<std::string, float> volm_satellite_resources::satellite_geo_reliability = create_satellite_reliability();
