#include "volm_satellite_resources.h"

#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <volm/volm_tile.h>
#include <volm/volm_geo_index2.h>
#include <vgl/vgl_intersection.h>

void add_directories(vcl_string root, vcl_vector<vcl_string>& directories) {
  if (vul_file::is_directory(root))
    directories.push_back(root);
  
  vcl_string glob = root + "/*"; // get everything directory or not
  vul_file_iterator file_it(glob.c_str());
  ++file_it;  // skip .
  ++file_it;  // skip ..
  while (file_it) {
    vcl_string name(file_it());
    if (vul_file::is_directory(name))
      add_directories(name, directories);
    ++file_it;
  }

}

void volm_satellite_resources::add_resource(vcl_string name)
{
  volm_satellite_resource res;
  res.full_path_ = name;
  res.name_ = vul_file::strip_directory(name);
  res.name_ = vul_file::strip_extension(res.name_);
  res.meta_ = new brad_image_metadata(name, "");
  if (res.meta_->gsd_ > 0)  // if there are parsing problems, gsd is negative
    resources_.push_back(res);
}

//: x is lon and y is lat in the bbox, construct bbox with min point to be lower left and max to be upper right and as axis aligned with North-East
volm_satellite_resources::volm_satellite_resources(vgl_box_2d<double>& bbox, double min_size) : bbox_(bbox), min_size_(min_size)
{ 
  this->construct_tree();
}

//: traverse the given path recursively and add each satellite resource
void volm_satellite_resources::add_path(vcl_string path)
{
  vcl_vector<vcl_string> directories;
  add_directories(path, directories);
  if (!directories.size())
    return;
  vcl_cout << "found " << directories.size() << " directories!\n"; 
  
  unsigned start = resources_.size();
  for (unsigned i = 0; i < directories.size(); i++) {
    vcl_string glob = directories[i] + "/*.NTF";
    vul_file_iterator file_it(glob.c_str());
    while (file_it) {
      vcl_string name(file_it());
      //vcl_cout << name << "\n";
      this->add_resource(name);    
      ++file_it;
    }  
    glob = directories[i] + "/*.ntf";
    vul_file_iterator file_it2(glob.c_str());
    while (file_it2) {
      vcl_string name(file_it2());
      //vcl_cout << name << "\n";
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
  root_ = volm_geo_index2::construct_tree<vcl_vector<unsigned> >(bbox_, min_size_);
  vcl_vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root_, leaves);
  vcl_cout << " the number of leaves in the quad tree of satellite resources: " << leaves.size() << '\n';
}

void volm_satellite_resources::add_resources(unsigned start, unsigned end) {
  // insert the ids of the resources
  for (unsigned i = start; i < end; i++) {
    vcl_vector<volm_geo_index2_node_sptr> leaves;
    // CAUTION: x is lat and y is lon in nitf_camera but we want x to be lon and y to be lat, use all the corners of satellite image by inverting x-y to create the bounding box
    vgl_box_2d<double> satellite_footprint;
    satellite_footprint.add(vgl_point_2d<double>(resources_[i].meta_->lower_left_.x(), resources_[i].meta_->lower_left_.y()));
    satellite_footprint.add(vgl_point_2d<double>(resources_[i].meta_->upper_right_.x(), resources_[i].meta_->upper_right_.y()));
    volm_geo_index2::get_leaves(root_, leaves, satellite_footprint);
    for (unsigned j = 0; j < leaves.size(); j++) {
      volm_geo_index2_node<vcl_vector<unsigned> >* leaf_ptr = dynamic_cast<volm_geo_index2_node<vcl_vector<unsigned> >* >(leaves[j].ptr());
      leaf_ptr->contents_.push_back(i);  // push this satellite image to this leave that intersects its footprint
    }
  }
}

//: get a list of ids in the resources_ list that overlap the given rectangular region
void volm_satellite_resources::query(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, vcl_string& band_str, vcl_vector<unsigned>& ids, double gsd_thres)
{
  vgl_box_2d<double> area(lower_left_lon, upper_right_lon, lower_left_lat, upper_right_lat);
  vcl_vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root_, leaves, area);
  vcl_vector<unsigned> temp_ids_init;
  for (unsigned i = 0; i < leaves.size(); i++) {
    volm_geo_index2_node<vcl_vector<unsigned> >* leaf_ptr = dynamic_cast<volm_geo_index2_node<vcl_vector<unsigned> >* >(leaves[i].ptr());
    // check which images overlap with the given bbox
    for (unsigned k = 0; k < leaf_ptr->contents_.size(); k++) {
      unsigned res_id = leaf_ptr->contents_[k];
      // CAUTION: x is lat and y is lon in nitf_camera but we want x to be lon and y to be lat, use all the corners of satellite image by inverting x-y to create the bounding box
      vgl_box_2d<double> sat_box;
      sat_box.add(vgl_point_2d<double>(resources_[res_id].meta_->lower_left_.x(), resources_[res_id].meta_->lower_left_.y()));
      sat_box.add(vgl_point_2d<double>(resources_[res_id].meta_->upper_right_.x(), resources_[res_id].meta_->upper_right_.y()));
      if (resources_[res_id].meta_->band_.compare(band_str) == 0 && vgl_intersection(sat_box, area).area() > 0)
        temp_ids_init.push_back(res_id);
    }
  }

  // eliminate the ones which does not satisfy the GSD (ground sampling distance) threshold
  vcl_vector<unsigned> temp_ids;
  for (unsigned i = 0; i < temp_ids_init.size(); i++) {
    if (resources_[temp_ids_init[i]].meta_->gsd_ <= gsd_thres)
      temp_ids.push_back(temp_ids_init[i]);
  }

  // order the resources in the order of GeoEye1, WV2, WV1, QB/others
  for (unsigned i = 0; i < temp_ids.size(); i++) {
    if (resources_[temp_ids[i]].meta_->satellite_name_.compare("GeoEye-1") == 0)
      ids.push_back(temp_ids[i]);
  }
  for (unsigned i = 0; i < temp_ids.size(); i++) {
    if (resources_[temp_ids[i]].meta_->satellite_name_.compare("WV01") == 0)
      ids.push_back(temp_ids[i]);
  }
  for (unsigned i = 0; i < temp_ids.size(); i++) {
    if (resources_[temp_ids[i]].meta_->satellite_name_.compare("WV02") == 0)
      ids.push_back(temp_ids[i]);
  }
  
  vcl_vector<unsigned> temp_ids2;
  // find the ones that are not already in ids
  for (unsigned i = 0; i < temp_ids.size(); i++) {
    bool contains = false;
    for (unsigned j = 0; j < ids.size(); j++) {
      if (temp_ids[i] == ids[j]) {
        contains = true;
        break;
      }
    }
    if (!contains)
      temp_ids2.push_back(temp_ids[i]);
  }
  for (unsigned i = 0; i < temp_ids2.size(); i++)
    ids.push_back(temp_ids2[i]);
    

}
//: query the resources in the given box and output the full paths to the given file
bool volm_satellite_resources::query_print_to_file(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, unsigned& cnt, vcl_string& out_file, vcl_string& band_str, double gsd_thres)
{
  vcl_vector<unsigned> ids, ids_all;
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
  vcl_ofstream ofs(out_file.c_str());
  if (!ofs) {
    vcl_cerr << "In volm_satellite_resources::query_print_to_file() -- cannot open file: " << out_file << vcl_endl;
    return false;
  }
  for (unsigned i = 0; i < ids.size(); i++)
    ofs << resources_[ids[i]].full_path_ << '\n';
  ofs.close();
  return true;
}

//: query the resources in the given box and output the full paths of randomly selected seeds to the given file, 
//  the order of satellites to select seeds from: GeoEye1, WorldView2, WorldView1 and then any others
bool volm_satellite_resources::query_seeds_print_to_file(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, int n_seeds, unsigned& cnt, vcl_string& out_file, vcl_string& band_str, double gsd_thres)
{
  vcl_vector<unsigned> ids;
  double mid_lon = (lower_left_lon + upper_right_lon) / 2;
  double mid_lat = (lower_left_lat + upper_right_lat) / 2;
  double lower_lon = (lower_left_lon + mid_lon) / 2;
  double lower_lat = (lower_left_lat + mid_lat) / 2;
  double upper_lon = (upper_right_lon + mid_lon) / 2;
  double upper_lat = (upper_right_lat + mid_lat) / 2;
  vcl_cout << "using bbox for scene: " << lower_lon << " " << lower_lat << " " << upper_lon << " " << upper_lat << vcl_endl;
  query(lower_lon, lower_lat, upper_lon, upper_lat, band_str, ids, gsd_thres);

  // now select n_seeds among these ones
  vcl_map<vcl_string, vcl_vector<unsigned> > possible_seeds;
  vcl_vector<unsigned> tmp;
  possible_seeds["GeoEye-1"] = tmp;
  possible_seeds["WV01"] = tmp;
  possible_seeds["WV02"] = tmp;
  possible_seeds["other"] = tmp;  
  for (unsigned i = 0; i < ids.size(); i++) {
    if (resources_[ids[i]].meta_->cloud_coverage_percentage_ < 1) {
      vcl_map<vcl_string, vcl_vector<unsigned> >::iterator iter = possible_seeds.find(resources_[ids[i]].meta_->satellite_name_);
      if (iter != possible_seeds.end()) 
        iter->second.push_back(ids[i]);
      else
        possible_seeds["other"].push_back(ids[i]);
    }
  }
#if 0
  vcl_cout << "possible seeds:" << vcl_endl;
  for (vcl_map<vcl_string, vcl_vector<unsigned> >::iterator iter = possible_seeds.begin(); iter != possible_seeds.end(); iter++) {
    vcl_cout << iter->first << "\n";
    for (unsigned k = 0; k < iter->second.size(); k++) {
      vcl_cout << "\t\t" << resources_[iter->second[k]].name_ << "\n";
    }
  }
#endif
  
  vcl_vector<brad_image_metadata_sptr> selected_names;
#if 0
  vcl_vector<vcl_string> names;
  names.push_back("GeoEye-1"); names.push_back("WV02"); names.push_back("WV01"); names.push_back("other");
  for (unsigned ii = 0; ii < names.size(); ii++) {
    vcl_string name = names[ii];
    for (unsigned i = 0; i < possible_seeds[name].size(); i++) {
      vcl_cout << resources_[possible_seeds[name][i]].name_;
      vcl_cout << " time " << name << ": " << resources_[possible_seeds[name][i]].meta_->t_.year;
      vcl_cout << " " << resources_[possible_seeds[name][i]].meta_->t_.month;
      vcl_cout << " " << resources_[possible_seeds[name][i]].meta_->t_.day;
      vcl_cout << " " << resources_[possible_seeds[name][i]].meta_->t_.hour;
      vcl_cout << " " << resources_[possible_seeds[name][i]].meta_->t_.min;
      vcl_cout << "\n";
    }
  }
#endif

  vcl_vector<vcl_string> seed_paths;

  cnt = 0;
  bool done = false;
  for (unsigned i = 0; i < possible_seeds["GeoEye-1"].size(); i++) {
    // first check if there is a satellite with the same time
    bool exists = false;
    for (unsigned k = 0; k < selected_names.size(); k++) {
      if (resources_[possible_seeds["GeoEye-1"][i]].meta_->same_time(*selected_names[k].ptr())) {
        exists = true;
        break;
      }
    }
    if (exists) continue;
    selected_names.push_back(resources_[possible_seeds["GeoEye-1"][i]].meta_);

    //ofs << resources_[possible_seeds["GeoEye-1"][i]].name_ << '\n';
    seed_paths.push_back(resources_[possible_seeds["GeoEye-1"][i]].name_);
    //vcl_cout << resources_[possible_seeds["GeoEye-1"][i]].name_ << '\n';
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
     for (unsigned k = 0; k < selected_names.size(); k++) {
       if (resources_[possible_seeds["WV02"][i]].meta_->same_time(*selected_names[k].ptr())) {
         exists = true;
         break;
       }
     }
     if (exists) continue;
     selected_names.push_back(resources_[possible_seeds["WV02"][i]].meta_);
      
     //ofs << resources_[possible_seeds["WV02"][i]].name_ << '\n';
     seed_paths.push_back(resources_[possible_seeds["WV02"][i]].name_);
     //vcl_cout << resources_[possible_seeds["WV02"][i]].name_ << '\n';
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
     for (unsigned k = 0; k < selected_names.size(); k++) {
       if (resources_[possible_seeds["WV01"][i]].meta_->same_time(*selected_names[k].ptr())) {
         exists = true;
         break;
       }
     }
     if (exists) continue;
     selected_names.push_back(resources_[possible_seeds["WV01"][i]].meta_);
      
     //ofs << resources_[possible_seeds["WV01"][i]].name_ << '\n';
     seed_paths.push_back(resources_[possible_seeds["WV01"][i]].name_);
     //vcl_cout << resources_[possible_seeds["WV01"][i]].name_ << '\n';
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
     for (unsigned k = 0; k < selected_names.size(); k++) {
       if (resources_[possible_seeds["other"][i]].meta_->same_time(*selected_names[k].ptr())) {
         exists = true;
         break;
       }
     }
     if (exists) continue;
     selected_names.push_back(resources_[possible_seeds["other"][i]].meta_);
      
     //ofs << resources_[possible_seeds["other"][i]].name_ << '\n';
     seed_paths.push_back(resources_[possible_seeds["other"][i]].name_);
     //vcl_cout << resources_[possible_seeds["other"][i]].name_ << '\n';
     cnt++;
     if (cnt == n_seeds) {
       done = true;
       break;
     }
    }
  }

  if (out_file.compare("") == 0)
    return true;

  vcl_ofstream ofs(out_file.c_str());
  if (!ofs) {
    vcl_cerr << "In volm_satellite_resources::query_print_to_file() -- cannot open file: " << out_file << vcl_endl;
    return false;
  }

  for (unsigned i = 0;i < seed_paths.size(); i++)
    ofs << seed_paths[i] << '\n';

  ofs.close();
  return true;
}

//: get a list of ids in the resources_ list that overlap the given rectangular region
void volm_satellite_resources::query_pairs(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, vcl_string& sat_name, vcl_vector<vcl_pair<unsigned, unsigned> >& ids)
{
  // first get all the images that intersect the area
  vcl_vector<unsigned> temp_ids;
  vcl_string band_str = "PAN";
  this->query(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, band_str, temp_ids,10.0); // we're only interested in which images intersect the box, so pass gsd_thres very high
  vcl_cout << "there are " << temp_ids.size() << " images that intersect the scene!\n";

  // prune out the ones from the wrong satellite
  vcl_vector<unsigned> ids2;
  for (unsigned i = 0; i < temp_ids.size(); i++) {
    if (resources_[temp_ids[i]].meta_->satellite_name_.compare(sat_name) == 0)
      ids2.push_back(temp_ids[i]);
  }

  vcl_cout << "there are " << ids2.size() << " images that intersect the scene from sat: " << sat_name << "!\n";
  // check the time of collection to find pairs
  for (unsigned i = 0; i < ids2.size(); i++) {
    for (unsigned j = i+1; j < ids2.size(); j++) {
      if (resources_[ids2[i]].meta_->same_day_time_dif(*(resources_[ids2[j]].meta_)) < 5) { // if taken less than 5 minute apart
        ids.push_back(vcl_pair<unsigned, unsigned>(ids2[i], ids2[j]));
      }
    }
  }
  
}

//: query the resources in the given box and output the full paths of pairs to the given file
bool volm_satellite_resources::query_pairs_print_to_file(double lower_left_lon, double lower_left_lat, double upper_right_lon, double upper_right_lat, unsigned& cnt, vcl_string& out_file, vcl_string& sat_name)
{
  vcl_vector<vcl_pair<unsigned, unsigned> > ids;
  query_pairs(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, sat_name, ids);

  vcl_ofstream ofs(out_file.c_str());
  if (!ofs) {
    vcl_cerr << "In volm_satellite_resources::query_pairs_print_to_file() -- cannot open file: " << out_file << vcl_endl;
    return false;
  }
  
  for (unsigned i = 0; i < ids.size(); i++) {
    ofs << resources_[ids[i].first].full_path_ << '\n';
    ofs << resources_[ids[i].second].full_path_ << "\n\n";
  }

  ofs.close();
  return true;
}

//: return the full path of a satellite image given its name, if not found returns empty string
vcl_pair<vcl_string, vcl_string> volm_satellite_resources::full_path(vcl_string name)
{
  for (unsigned i = 0; i < resources_.size(); i++) {
    if (name.compare(resources_[i].name_) == 0) {
      vcl_pair<vcl_string, vcl_string> p(resources_[i].full_path_, resources_[i].meta_->satellite_name_);
      return p;
    }
  }
  return vcl_pair<vcl_string, vcl_string>("", "");
}

vcl_string volm_satellite_resources::find_pair(vcl_string const& name)
{
  for (unsigned i = 0; i < resources_.size(); i++) {
    if (name.compare(resources_[i].name_) == 0) {
      if (resources_[i].pair_.compare("") == 0) {  // not yet found
        // find all the resources that overlaps with this one
        vcl_string band_str = resources_[i].meta_->band_;
        vcl_string other_band;
        if (band_str.compare("PAN") == 0)
          other_band = "MULTI";
        else
          other_band = "PAN";
        vcl_vector<unsigned> ids;
        this->query(resources_[i].meta_->lower_left_.x(), 
                    resources_[i].meta_->lower_left_.y(), 
                    resources_[i].meta_->upper_right_.x(), 
                    resources_[i].meta_->upper_right_.y(), other_band, ids,10.0);  // pass gsd_thres very high, only interested in finding all the images
        vcl_cout << " there are " << ids.size() << " resources that cover the image!\n";
        for (unsigned iii = 0; iii < ids.size(); iii++) {
          unsigned ii = ids[iii];
          if (resources_[i].meta_->satellite_name_.compare(resources_[ii].meta_->satellite_name_) == 0 &&  // same satellite good!
                resources_[i].meta_->same_time(*resources_[ii].meta_) &&
                resources_[i].meta_->same_extent(*resources_[ii].meta_) ) {
                  resources_[i].pair_ = resources_[ii].name_;
                  resources_[ii].pair_ = resources_[i].name_;
                return resources_[ii].name_; 
          }
        }
      } else 
        return resources_[i].pair_;
     break;
    }
  }
  return "";
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
  for (unsigned i = 0; i < resources_.size(); i++) {
    resources_[i].b_write(os);
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
    vcl_cout << "volm_satellite_resources -- unknown binary io version " << ver << '\n';
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
    vcl_cout << "volm_satellite_resources -- unknown binary io version " << ver << '\n';
    return;
  }
}


//dummy vsl io functions to allow volm_satellite_resources to be inserted into
//brdb as a dbvalue
void vsl_b_write(vsl_b_ostream & os, volm_satellite_resources const &tc)
{ /* do nothing */ }
void vsl_b_read(vsl_b_istream & is, volm_satellite_resources &tc)
{ /* do nothing */ }
void vsl_print_summary(vcl_ostream &os, const volm_satellite_resources &tc)
{ /* do nothing */ }
void vsl_b_read(vsl_b_istream& is, volm_satellite_resources* tc)
{ /* do nothing */ }
void vsl_b_write(vsl_b_ostream& os, const volm_satellite_resources* &tc)
{ /* do nothing */ }
void vsl_print_summary(vcl_ostream& os, const volm_satellite_resources* &tc)
{ /* do nothing */ }
void vsl_b_read(vsl_b_istream& is, volm_satellite_resources_sptr& tc)
{ /* do nothing */ }
void vsl_b_write(vsl_b_ostream& os, const volm_satellite_resources_sptr &tc)
{ /* do nothing */ }
void vsl_print_summary(vcl_ostream& os, const volm_satellite_resources_sptr &tc)
{ /* do nothing */ }


// create table with the increments to use during hypotheses generation according to each land type, the unit is in meters
vcl_map<vcl_string, float> create_satellite_reliability()
{
  vcl_map<vcl_string, float> m;
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
vcl_map<vcl_string, float> volm_satellite_resources::satellite_geo_reliability = create_satellite_reliability();

