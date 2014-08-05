//This is brl/bbas/volm/pro/processes/volm_create_satellite_site_process.cxx
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h> // for std::FILE and std::fopen()

#include <brdb/brdb_value.h>

#include <volm/volm_satellite_resources.h>
#include <volm/volm_satellite_resources_sptr.h>

#include <vgl/vgl_polygon.h>
#include <bkml/bkml_parser.h>

#include <vul/vul_file.h>

//: sets input and output types
bool volm_create_satellite_resources_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(3);
  input_types_[0] = "vcl_string";      // polygon file (kml) of ROI
  input_types_[1] = "float";           // leaf size
  input_types_[2] = "bool";    // if true: eliminate the images which have different names but 'same' extent and 'same' collection time up to seconds
  
  if (!pro.set_input_types(input_types_))
    return false;
  //output
  vcl_vector<vcl_string> output_types_(1);
  output_types_[0] = "volm_satellite_resources_sptr";
  return pro.set_output_types(output_types_);
}

bool volm_create_satellite_resources_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << " invalid inputs" << vcl_endl;
    return false;
  }

  //get the inputs
  vcl_string poly_file = pro.get_input<vcl_string>(0);
  float leaf_size = pro.get_input<float>(1);
  bool eliminate_same = pro.get_input<bool>(2);

  // find the bbox of the polygon
  vgl_polygon<double> poly = bkml_parser::parse_polygon(poly_file);
  vcl_cout << "outer poly  has: " << poly[0].size() << " points " << vcl_endl;

  // find the bbox of ROI from its polygon
  vgl_box_2d<double> bbox;
  for (unsigned i = 0; i < poly[0].size(); i++) {
    bbox.add(poly[0][i]);
  }
  vcl_cout << "bbox of ROI: " << bbox << vcl_endl;

  volm_satellite_resources_sptr res = new volm_satellite_resources(bbox, leaf_size,eliminate_same);
  pro.set_output_val<volm_satellite_resources_sptr>(0, res);

  return true;
}


bool volm_save_satellite_resources_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(2);
  input_types_[0] = "volm_satellite_resources_sptr"; 
  input_types_[1] = "vcl_string";      // output file to save as binar
  
  if (!pro.set_input_types(input_types_))
    return false;
  //output
  vcl_vector<vcl_string> output_types_(0);
  return pro.set_output_types(output_types_);
}

bool volm_save_satellite_resources_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << " invalid inputs" << vcl_endl;
    return false;
  }

  //get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  vcl_string out_file = pro.get_input<vcl_string>(1);

  vsl_b_ofstream ofs(out_file);
  res->b_write(ofs);
  ofs.close();

  pro.set_output_val<volm_satellite_resources_sptr>(0, res);

  return true;
}

bool volm_load_satellite_resources_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(1);
  input_types_[0] = "vcl_string"; 
  
  if (!pro.set_input_types(input_types_))
    return false;
  //output
  vcl_vector<vcl_string> output_types_(1);
  output_types_[0] = "volm_satellite_resources_sptr";
  return pro.set_output_types(output_types_);
}

bool volm_load_satellite_resources_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << " invalid inputs" << vcl_endl;
    return false;
  }

  //get the inputs
  vcl_string res_file = pro.get_input<vcl_string>(0);

  volm_satellite_resources_sptr res = new volm_satellite_resources();
  vsl_b_ifstream is(res_file);
  res->b_read(is);
  is.close();

  vcl_cout << "there are " << res->resources_size() << " resources in the file!\n";

  pro.set_output_val<volm_satellite_resources_sptr>(0, res);
  return true;
}


bool volm_query_satellite_resources_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(10);
  input_types_[0] = "volm_satellite_resources_sptr"; 
  input_types_[1] = "double";      // lower left lon
  input_types_[2] = "double";      // lower left lat
  input_types_[3] = "double";      // upper right lon
  input_types_[4] = "double";      // upper right lat
  input_types_[5] = "vcl_string";      // output file to print the list
  input_types_[6] = "vcl_string";      // the band: PAN or MULTI 
  input_types_[7] = "bool";      // if TRUE, pick seed images randomly with a certain order of satellites
  input_types_[8] = "int";       // number of seed images to pick, if not enough then creates from all available
  input_types_[9] = "double";       // GSD threshold in meters, any image with GSD more than this GSD will not be returned, e.g. pass 1 to eliminate images with pixel resolution more than 1 meter
  
  if (!pro.set_input_types(input_types_))
    return false;
  //output
  vcl_vector<vcl_string> output_types_(1);
  output_types_[0] = "unsigned";  // return number of resources that intersect this region
  return pro.set_output_types(output_types_);
}

bool volm_query_satellite_resources_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << " invalid inputs" << vcl_endl;
    return false;
  }

  //get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  double lower_left_lon = pro.get_input<double>(1);
  double lower_left_lat = pro.get_input<double>(2);
  double upper_right_lon = pro.get_input<double>(3);
  double upper_right_lat = pro.get_input<double>(4);
  vcl_string out_file = pro.get_input<vcl_string>(5);
  vcl_string band = pro.get_input<vcl_string>(6);
  bool pick_seed = pro.get_input<bool>(7);
  int n_seeds = pro.get_input<int>(8);
  double gsd_thres = pro.get_input<double>(9);
  
  unsigned cnt; bool out = false;
  if (!pick_seed) {
    out = res->query_print_to_file(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, cnt, out_file, band, gsd_thres);
    pro.set_output_val<unsigned>(0, cnt);
  } else {
    out = res->query_seeds_print_to_file(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, n_seeds, cnt, out_file, band, gsd_thres);
    pro.set_output_val<unsigned>(0, cnt);
  }
  return out;
}


// obtain the satellite resources that intersects with given polygon
bool volm_query_satellite_resources_kml_process_cons(bprb_func_process& pro)
{
  // inputs
  vcl_vector<vcl_string> input_types_(7);
  input_types_[0] = "volm_satellite_resources_sptr";  // satellite resource
  input_types_[1] = "vcl_string";                     // kml polygon
  input_types_[2] = "vcl_string";                     // output file to print the list
  input_types_[3] = "vcl_string";                     // the band: PAN or MULTI
  input_types_[4] = "bool";                           // if TRUE, pick seed images randomly with a certain order of satellites
  input_types_[5] = "int";                            // number of seed images to pick, if not enough then creates from all available
  input_types_[6] = "double";                         // GSD threshold in meters, any image with GSD more than this GSD will not be returned, e.g. pass 1 to eliminate images with pixel resolution more than 1 meter

  // output 
  vcl_vector<vcl_string> output_types_(1);
  output_types_[0] = "unsigned";  // return number of resources that intersect this region
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

}

bool volm_query_satellite_resources_kml_process(bprb_func_process& pro)
{
  // check inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << " invalid inputs" << vcl_endl;
    return false;
  }

  // get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  vcl_string kml_file = pro.get_input<vcl_string>(1);
  vcl_string out_file = pro.get_input<vcl_string>(2);
  vcl_string band = pro.get_input<vcl_string>(3);
  bool pick_seed = pro.get_input<bool>(4);
  int n_seeds = pro.get_input<int>(5);
  double gsd_thres = pro.get_input<double>(6);

  // parse the polygon and construct its bounding box
  if (!vul_file::exists(kml_file)) {
    vcl_cout << pro.name() << ": can not find input kml file: " << kml_file << vcl_endl;
    return false;
  }
  vgl_polygon<double> poly = bkml_parser::parse_polygon(kml_file);
  vgl_box_2d<double> bbox;
  for (unsigned i = 0; i < poly[0].size(); i++)
    bbox.add(poly[0][i]);
  double lower_left_lon  = bbox.min_x();
  double lower_left_lat  = bbox.min_y();
  double upper_right_lon = bbox.max_x();
  double upper_right_lat = bbox.max_y();

  unsigned cnt;  bool out = false;
  if (!pick_seed) {
    out = res->query_print_to_file(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, cnt, out_file, band, gsd_thres);
    pro.set_output_val<unsigned>(0, cnt);
  } else {
    out = res->query_seeds_print_to_file(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, n_seeds, cnt, out_file, band, gsd_thres);
    pro.set_output_val<unsigned>(0, cnt);
  }
  return out;

}

//: sets input and output types
bool volm_add_satellite_resources_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(2);
  input_types_[0] = "volm_satellite_resources_sptr";
  input_types_[1] = "vcl_string"; // folder -- will be traversed recursively to find NITF files and add to the resources
  
  if (!pro.set_input_types(input_types_))
    return false;
  //output
  vcl_vector<vcl_string> output_types_(0);
  return pro.set_output_types(output_types_);
}

bool volm_add_satellite_resources_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << " invalid inputs" << vcl_endl;
    return false;
  }

  //get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  vcl_string folder = pro.get_input<vcl_string>(1);
  res->add_path(folder);

  vcl_cout << " AFTER addition, there are " << res->resources_size() << " resources in the file!\n";
  return true;
}

bool volm_pick_nadir_resource_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(8);
  input_types_[0] = "volm_satellite_resources_sptr"; 
  input_types_[1] = "double";      // lower left lon
  input_types_[2] = "double";      // lower left lat
  input_types_[3] = "double";      // upper right lon
  input_types_[4] = "double";      // upper right lat
  input_types_[5] = "vcl_string";      // the band: PAN or MULTI 
  input_types_[6] = "vcl_string";      // satellite name
  input_types_[7] = "vcl_string";      // a folder where all the non-cloud images saved for current rectangular region
  
  if (!pro.set_input_types(input_types_))
    return false;
  //output
  vcl_vector<vcl_string> output_types_(1);
  output_types_[0] = "vcl_string";  // full path of the satellite image
  return pro.set_output_types(output_types_);
}

bool volm_pick_nadir_resource_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << " invalid inputs" << vcl_endl;
    return false;
  }

  //get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  double lower_left_lon = pro.get_input<double>(1);
  double lower_left_lat = pro.get_input<double>(2);
  double upper_right_lon = pro.get_input<double>(3);
  double upper_right_lat = pro.get_input<double>(4);
  vcl_string band = pro.get_input<vcl_string>(5);
  vcl_string sat_name = pro.get_input<vcl_string>(6);
  vcl_string non_cloud_folder = pro.get_input<vcl_string>(7);
  
  vcl_vector<unsigned> ids;
  res->query(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, band, ids,10.0); // pass gsd_thres very high, only interested in finding all the images that intersect the box
  double largest_view_angle = -100.0;
  unsigned id = 0;  
  for (unsigned i = 0; i < ids.size(); i++) {
    if (res->resources_[ids[i]].meta_->satellite_name_.compare(sat_name) == 0) {
      vcl_cout << "res: " << res->resources_[ids[i]].name_
               << " view azimuth: " << res->resources_[ids[i]].meta_->view_azimuth_
               << " view elev: " << res->resources_[ids[i]].meta_->view_elevation_ << '\n';
      // pick the image with largest view angle and least could coverage if possible
      if (largest_view_angle < res->resources_[ids[i]].meta_->view_elevation_) {
        // check whether this image is non-cloud one or not
        if (non_cloud_folder.compare("") != 0) {
          vcl_string non_cloud_img = non_cloud_folder + "/" + res->resources_[ids[i]].name_ + "_cropped.tif";
          if (vul_file::exists(non_cloud_img)) {
            largest_view_angle = res->resources_[ids[i]].meta_->view_elevation_;
            id = ids[i];
          }
        }
        else {  // TO DO -- add edge detection here if non-cloud images folder is not given
          largest_view_angle = res->resources_[ids[i]].meta_->view_elevation_;
          id = ids[i];
        }
      }
    }
  }
  vcl_cout << "picked: " << res->resources_[id].full_path_ << vcl_endl;
  pro.set_output_val<vcl_string>(0, res->resources_[id].full_path_);
  return true;
}

// find the non-cloud PAN/MULTI pair from satellite resource
// Note it will output all PAN/MULTI pairs that intersect with current scene, sorted by the view angles
bool volm_pick_nadir_resource_pair_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(9);
  input_types_[0] = "volm_satellite_resources_sptr"; 
  input_types_[1] = "double";      // lower left lon
  input_types_[2] = "double";      // lower left lat
  input_types_[3] = "double";      // upper right lon
  input_types_[4] = "double";      // upper right lat
  input_types_[5] = "vcl_string";      // the band: PAN or MULTI 
  input_types_[6] = "vcl_string";      // satellite name
  input_types_[7] = "vcl_string";      // a folder where all the non-cloud images saved for current rectangular region
  input_types_[8] = "vcl_string";      // folder where the sorted PAN/MULTI pair list file will be stored
  // output
  vcl_vector<vcl_string> output_types_(2);
  output_types_[0] = "vcl_string";  // full path of the PAN image
  output_types_[1] = "vcl_string";  // full path of the MULTI image
  return pro.set_output_types(output_types_) && pro.set_input_types(input_types_);
}

bool volm_pick_nadir_resource_pair_process(bprb_func_process& pro)
{
  // input check
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << " invalid inputs" << vcl_endl;
    return false;
  }
  // get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  double lower_left_lon = pro.get_input<double>(1);
  double lower_left_lat = pro.get_input<double>(2);
  double upper_right_lon = pro.get_input<double>(3);
  double upper_right_lat = pro.get_input<double>(4);
  vcl_string band = pro.get_input<vcl_string>(5);
  vcl_string sat_name = pro.get_input<vcl_string>(6);
  vcl_string non_cloud_folder = pro.get_input<vcl_string>(7);
  vcl_string out_folder = pro.get_input<vcl_string>(8);
  // obtain resources having given band that overlap with current region
  vcl_vector<unsigned> ids;
  res->query(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, band, ids,10.0);
  // map of resources that have given band and sat_name, sorted by the view angle
  vcl_map<double, unsigned, std::greater<double> > band_res;
  for (unsigned i = 0; i < ids.size(); i++)
    if (res->resources_[ids[i]].meta_->satellite_name_.compare(sat_name) == 0)
      band_res.insert(vcl_pair<double, unsigned>(res->resources_[ids[i]].meta_->view_elevation_, ids[i]));
  
  // text file where the sorted PAN/MULTI pair will be stored
  vcl_string out_txt = out_folder + "/pan_multi_pair_list.txt";
  vcl_ofstream ofs(out_txt.c_str());
  ofs << "view_angle(deg) \t pan_img \t multi_img \t" << vcl_endl;

  // find the PAN/MULTI pair
  vcl_map<double, vcl_pair<vcl_string, vcl_string>, vcl_greater<double> > pairs;
  for (vcl_map<double, unsigned, std::greater<double> >::iterator mit = band_res.begin(); mit != band_res.end(); ++mit) {
    vcl_string img_name = res->resources_[mit->second].name_;
    vcl_string pair_name = res->find_pair(img_name);
    if (pair_name.compare("") == 0)
      continue;
    if (non_cloud_folder.compare("") != 0)
    {
      vcl_string non_cloud_img_band = non_cloud_folder + "/" + img_name  + "_cropped.tif";
      vcl_cout << " view_angle = " << mit->first << " img_name = " << res->resources_[mit->second].full_path_ 
             << " pair_name = " << res->full_path(pair_name).first
             << " non_cloud_img = " << non_cloud_img_band
             << vcl_endl;
      if (vul_file::exists(non_cloud_img_band)) {
        vcl_pair<vcl_string, vcl_string> name_pair;
        if (band == "PAN") {
          name_pair.first  = res->resources_[mit->second].full_path_;
          name_pair.second = res->full_path(pair_name).first;
        }
        else if (band == "MULTI") {
          name_pair.first  = res->full_path(pair_name).first;
          name_pair.second = res->resources_[mit->second].full_path_;
        }
        else {
          vcl_cout << pro.name() << ": unknown input band " << band << vcl_endl;
          return false;
        }
        pairs.insert(vcl_pair<double, vcl_pair<vcl_string, vcl_string> >(mit->first, name_pair));
      }
    }
    else {
      vcl_pair<vcl_string, vcl_string> name_pair;
      if (band == "PAN") {
        name_pair.first = res->resources_[mit->second].full_path_;
        name_pair.second = res->full_path(pair_name).first;
      } else if (band == "MULTI") {
        name_pair.first = res->full_path(pair_name).first;
        name_pair.second = res->resources_[mit->second].full_path_;
      } else {
        vcl_cout << pro.name() << ": unknown input band " << band << vcl_endl;
        return false;
      }
      pairs.insert(vcl_pair<double, vcl_pair<vcl_string, vcl_string> >(mit->first, name_pair));
    }
  }
  // output
  if (pairs.size() == 0) {
    vcl_cout << pro.name() << ": can not find any PAN/MULTI pair for current scene" << vcl_endl;
    return false;
  }
  
  for (vcl_map<double, vcl_pair<vcl_string, vcl_string>, vcl_greater<double> >::iterator mit = pairs.begin(); mit != pairs.end(); ++mit)
    ofs << mit->first << " \t " << mit->second.first << " \t " << mit->second.second << vcl_endl;
  ofs.close();

  pro.set_output_val<vcl_string>(0, pairs.begin()->second.first);
  pro.set_output_val<vcl_string>(1, pairs.begin()->second.second);

#if 0
  for (vcl_map<double, unsigned, std::greater<double> >::iterator mit = band_res.begin(); mit != band_res.end(); ++mit) {
    vcl_string img_name = res->resources_[mit->second].name_;
    vcl_string pair_name = res->find_pair(img_name);
    if (pair_name.compare("") == 0)
      continue;
    if (non_cloud_folder.compare("") != 0)
    {
      vcl_string non_cloud_img_band = non_cloud_folder + "/" + img_name  + "_cropped.tif";
      if (vul_file::exists(non_cloud_img_band)) {
        vcl_string pan_path, multi_path;
        if (band == "PAN") {
          pan_path = res->resources_[mit->second].full_path_;
          vcl_pair<vcl_string, vcl_string> pp = res->full_path(pair_name);
          multi_path = pp.first;
        }
        else if (band == "MULTI") {
          multi_path = res->resources_[mit->second].full_path_;
          vcl_pair<vcl_string, vcl_string> pp = res->full_path(pair_name);
          pan_path = pp.first;
        }
        else {
          vcl_cout << pro.name() << ": unknown input band " << band << vcl_endl;
          return false;
        }
        pro.set_output_val<vcl_string>(0, pan_path);
        pro.set_output_val<vcl_string>(1, multi_path);
        return true;
      }
    }
    else // TO DO -- add edge detection here if non-cloud images folder is not given
    {
      vcl_string pan_path, multi_path;
      if (band == "PAN") {
        pan_path = res->resources_[mit->second].full_path_;
        vcl_pair<vcl_string, vcl_string> pp = res->full_path(pair_name);
        multi_path = pp.first;
      }
      else if (band == "MULTI") {
        multi_path = res->resources_[mit->second].full_path_;
        vcl_pair<vcl_string, vcl_string> pp = res->full_path(pair_name);
        pan_path = pp.first;
      }
      else {
        vcl_cout << pro.name() << ": unknown input band " << band << vcl_endl;
        return false;
      }
      pro.set_output_val<vcl_string>(0, pan_path);
      pro.set_output_val<vcl_string>(1, multi_path);
      return true;
    }
  }

  return false;
#endif

  return true;
}

// find the PAN counterpart if given a multi band image, and vice versa
bool volm_get_full_path_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(2);
  input_types_[0] = "volm_satellite_resources_sptr"; 
  input_types_[1] = "vcl_string";      // satellite img name  
  
  if (!pro.set_input_types(input_types_))
    return false;
  //output
  vcl_vector<vcl_string> output_types_(1);
  output_types_[0] = "vcl_string";  // full path of the satellite whose name is passed
  return pro.set_output_types(output_types_);
}

bool volm_get_full_path_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << " invalid inputs" << vcl_endl;
    return false;
  }

  //get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  vcl_string name = pro.get_input<vcl_string>(1);
  vcl_pair<vcl_string, vcl_string> full = res->full_path(name);
  pro.set_output_val<vcl_string>(0, full.first);
  return true;
}

// find the PAN counterpart if given a multi band image, and vice versa
bool volm_find_res_pair_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(2);
  input_types_[0] = "volm_satellite_resources_sptr"; 
  input_types_[1] = "vcl_string";      // satellite img name  // only the name, don't need the full path
  
  if (!pro.set_input_types(input_types_))
    return false;
  //output
  vcl_vector<vcl_string> output_types_(3);
  output_types_[0] = "vcl_string";  // full path of the satellite whose name is passed
  output_types_[1] = "vcl_string";  // name of the pair satellite image, process returns false if not found
  output_types_[2] = "vcl_string";  // full path of the pair satellite image, process returns false if not found
  return pro.set_output_types(output_types_);
}

bool volm_find_res_pair_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << " invalid inputs" << vcl_endl;
    return false;
  }

  //get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  vcl_string name = pro.get_input<vcl_string>(1);
  vcl_pair<vcl_string, vcl_string> full = res->full_path(name);
  vcl_string pair_name = res->find_pair(name);
  vcl_cout << "pair_name = " << pair_name << vcl_endl;
  if (pair_name.compare("") == 0) {
    pro.set_output_val<vcl_string>(0, "");
    pro.set_output_val<vcl_string>(1, "");
    pro.set_output_val<vcl_string>(2, "");
    return false;
  } 
  vcl_pair<vcl_string, vcl_string> pp = res->full_path(pair_name);
  pro.set_output_val<vcl_string>(0, full.first);
  pro.set_output_val<vcl_string>(1, pair_name);
  pro.set_output_val<vcl_string>(2, pp.first);
  return true;
}

// a process to return a text file with names of 'pairs' of satellite images that are taken a few minutes apart from each other
bool volm_find_satellite_pairs_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(8);
  input_types_[0] = "volm_satellite_resources_sptr"; 
  input_types_[1] = "double";      // lower left lon
  input_types_[2] = "double";      // lower left lat
  input_types_[3] = "double";      // upper right lon
  input_types_[4] = "double";      // upper right lat
  input_types_[5] = "vcl_string";      // output file to print the list
  input_types_[6] = "vcl_string";      // satellite name
  input_types_[7] = "float";       // GSD_threshold
  
  if (!pro.set_input_types(input_types_))
    return false;
  //output
  vcl_vector<vcl_string> output_types_(1);
  output_types_[0] = "unsigned";  // return number of pairs that intersect this region
  return pro.set_output_types(output_types_);
}

bool volm_find_satellite_pairs_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << " invalid inputs" << vcl_endl;
    return false;
  }

  //get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  double lower_left_lon = pro.get_input<double>(1);
  double lower_left_lat = pro.get_input<double>(2);
  double upper_right_lon = pro.get_input<double>(3);
  double upper_right_lat = pro.get_input<double>(4);
  vcl_string out_file = pro.get_input<vcl_string>(5);
  vcl_string sat_name = pro.get_input<vcl_string>(6);
  float GSD_thres = pro.get_input<float>(7);
  
  unsigned cnt = 0; bool out = false;
  out = res->query_pairs_print_to_file(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, GSD_thres, cnt, out_file, sat_name);
  pro.set_output_val<unsigned>(0, cnt);
  return out;
}

// a process to return a text file with names of 'pairs' of satellite images that are taken a few miutues apart from each other
// (the time difference is within 5 minutes and more than 1 minutes)
bool volm_find_satellite_pairs_poly_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(5);
  input_types_[0] = "volm_satellite_resources_sptr"; 
  input_types_[1] = "vcl_string";      // kml file that gives the region of interest polygon
  input_types_[2] = "vcl_string";      // output file to print the list
  input_types_[3] = "vcl_string";      // satellite name
  input_types_[4] = "float";       // GSD_threshold
  if (!pro.set_input_types(input_types_))
    return false;
  //output
  vcl_vector<vcl_string> output_types_(1);
  output_types_[0] = "unsigned";  // return number of pairs that intersect this region
  return pro.set_output_types(output_types_);
}

bool volm_find_satellite_pairs_poly_process(bprb_func_process& pro)
{
  // check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << " invalid inputs" << vcl_endl;
    return false;
  }
  //get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  vcl_string poly_file = pro.get_input<vcl_string>(1);
  vcl_string out_file = pro.get_input<vcl_string>(2);
  vcl_string sat_name = pro.get_input<vcl_string>(3);
  float GSD_thres = pro.get_input<float>(4);

  // find the bbox of the polygon
  vgl_polygon<double> poly = bkml_parser::parse_polygon(poly_file);
  vcl_cout << "outer poly  has: " << poly[0].size() << " points " << vcl_endl;

  // find the bbox of ROI from its polygon
  vgl_box_2d<double> bbox;
  for (unsigned i = 0; i < poly[0].size(); i++) {
    bbox.add(poly[0][i]);
  }
  double lower_left_lon  = bbox.min_x();
  double lower_left_lat  = bbox.min_y();
  double upper_right_lon = bbox.max_x();
  double upper_right_lat = bbox.max_y();

  unsigned cnt = 0; bool out = false;
  out = res->query_pairs_print_to_file(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, GSD_thres, cnt, out_file, sat_name);
  pro.set_output_val<unsigned>(0, cnt);
  return out;
}

#include <vgl/vgl_intersection.h>
// a simple process to return the area of the region where to satellite images intersect
bool volm_satellite_pair_intersection_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(2);
  input_types_[0] = "vcl_string";      // satellite image 1
  input_types_[1] = "vcl_string";      // satellite image 2
  if (!pro.set_input_types(input_types_))
    return false;
  //output
  vcl_vector<vcl_string> output_types_(5);
  output_types_[0] = "double";  // lower left  lon of intersection region
  output_types_[1] = "double";  // lower left  lat of intersection region
  output_types_[2] = "double";  // upper right lon of intersection region
  output_types_[3] = "double";  // upper right lat of intersection region
  output_types_[4] = "double";  // intersection area
  return pro.set_output_types(output_types_);
}

bool volm_satellite_pair_intersection_process(bprb_func_process& pro)
{
  // check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << " invalid inputs" << vcl_endl;
    return false;
  }
  //get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  vcl_string img_file1 = pro.get_input<vcl_string>(1);
  vcl_string img_file2 = pro.get_input<vcl_string>(2);

  if (!vul_file::exists(img_file1)) {
    vcl_cout << pro.name() << ": " << img_file1 << " is missing" << vcl_endl;
    return false;
  }
  if (!vul_file::exists(img_file2)) {
    vcl_cout << pro.name() << ": " << img_file2 << " is missing" << vcl_endl;
    return false;
  }

  // load two nitf images
  brad_image_metadata meta1(img_file1,"");
  brad_image_metadata meta2(img_file2,"");
  vgl_box_2d<double> bbox1(meta1.lower_left_.x(), meta1.upper_right_.x(),
                           meta1.lower_left_.y(), meta1.upper_right_.y());
  vgl_box_2d<double> bbox2(meta2.lower_left_.x(), meta2.upper_right_.x(),
                           meta2.lower_left_.y(), meta2.upper_right_.y());
  
  vgl_box_2d<double> intersection_bbox = vgl_intersection(bbox1, bbox2);
  double area = intersection_bbox.area();

  // generate output
  unsigned i = 0;
  pro.set_output_val<double>(i++, intersection_bbox.min_x());
  pro.set_output_val<double>(i++, intersection_bbox.min_y());
  pro.set_output_val<double>(i++, intersection_bbox.max_x());
  pro.set_output_val<double>(i++, intersection_bbox.max_y());
  pro.set_output_val<double>(i++, area);
  return true;



}


