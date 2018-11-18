//This is brl/bbas/volm/pro/processes/volm_create_satellite_site_process.cxx
//:
// \file
#include <string>
#include <iostream>
#include <cstdio>
#include <functional>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <brdb/brdb_value.h>

#include <volm/volm_satellite_resources.h>
#include <volm/volm_satellite_resources_sptr.h>

#include <vgl/vgl_polygon.h>
#include <bkml/bkml_parser.h>

#include <vul/vul_file.h>
#include <vgl/vgl_area.h>

//: sets input and output types
bool volm_create_satellite_resources_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types_(3);
  input_types_[0] = "vcl_string";      // polygon file (kml) of ROI
  input_types_[1] = "float";           // leaf size
  input_types_[2] = "bool";    // if true: eliminate the images which have different names but 'same' extent and 'same' collection time up to seconds

  if (!pro.set_input_types(input_types_))
    return false;
  //output
  std::vector<std::string> output_types_(1);
  output_types_[0] = "volm_satellite_resources_sptr";
  return pro.set_output_types(output_types_);
}

bool volm_create_satellite_resources_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  std::string poly_file = pro.get_input<std::string>(0);
  auto leaf_size = pro.get_input<float>(1);
  bool eliminate_same = pro.get_input<bool>(2);

  // find the bbox of the polygon
  vgl_polygon<double> poly = bkml_parser::parse_polygon(poly_file);
  std::cout << "outer poly  has: " << poly[0].size() << " points " << std::endl;

  // find the bbox of ROI from its polygon
  vgl_box_2d<double> bbox;
  for (auto i : poly[0]) {
    bbox.add(i);
  }
  std::cout << "bbox of ROI: " << bbox << std::endl;

  volm_satellite_resources_sptr res = new volm_satellite_resources(bbox, leaf_size,eliminate_same);
  pro.set_output_val<volm_satellite_resources_sptr>(0, res);

  return true;
}


bool volm_save_satellite_resources_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types_(2);
  input_types_[0] = "volm_satellite_resources_sptr";
  input_types_[1] = "vcl_string";      // output file to save as binar

  if (!pro.set_input_types(input_types_))
    return false;
  //output
  std::vector<std::string> output_types_(0);
  return pro.set_output_types(output_types_);
}

bool volm_save_satellite_resources_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  std::string out_file = pro.get_input<std::string>(1);

  vsl_b_ofstream ofs(out_file);
  res->b_write(ofs);
  ofs.close();

  pro.set_output_val<volm_satellite_resources_sptr>(0, res);

  return true;
}

bool volm_load_satellite_resources_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types_(1);
  input_types_[0] = "vcl_string";

  if (!pro.set_input_types(input_types_))
    return false;
  //output
  std::vector<std::string> output_types_(1);
  output_types_[0] = "volm_satellite_resources_sptr";
  return pro.set_output_types(output_types_);
}

bool volm_load_satellite_resources_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  std::string res_file = pro.get_input<std::string>(0);

  volm_satellite_resources_sptr res = new volm_satellite_resources();
  vsl_b_ifstream is(res_file);
  res->b_read(is);
  is.close();

  std::cout << "there are " << res->resources_size() << " resources in the file!\n";

  pro.set_output_val<volm_satellite_resources_sptr>(0, res);
  return true;
}


bool volm_query_satellite_resources_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types_(10);
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
  std::vector<std::string> output_types_(1);
  output_types_[0] = "unsigned";  // return number of resources that intersect this region
  return pro.set_output_types(output_types_);
}

bool volm_query_satellite_resources_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  auto lower_left_lon = pro.get_input<double>(1);
  auto lower_left_lat = pro.get_input<double>(2);
  auto upper_right_lon = pro.get_input<double>(3);
  auto upper_right_lat = pro.get_input<double>(4);
  std::string out_file = pro.get_input<std::string>(5);
  std::string band = pro.get_input<std::string>(6);
  bool pick_seed = pro.get_input<bool>(7);
  int n_seeds = pro.get_input<int>(8);
  auto gsd_thres = pro.get_input<double>(9);

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
  std::vector<std::string> input_types_(7);
  input_types_[0] = "volm_satellite_resources_sptr";  // satellite resource
  input_types_[1] = "vcl_string";                     // kml polygon
  input_types_[2] = "vcl_string";                     // output file to print the list
  input_types_[3] = "vcl_string";                     // the band: PAN or MULTI
  input_types_[4] = "bool";                           // if TRUE, pick seed images randomly with a certain order of satellites
  input_types_[5] = "int";                            // number of seed images to pick, if not enough then creates from all available
  input_types_[6] = "double";                         // GSD threshold in meters, any image with GSD more than this GSD will not be returned, e.g. pass 1 to eliminate images with pixel resolution more than 1 meter

  // output
  std::vector<std::string> output_types_(1);
  output_types_[0] = "unsigned";  // return number of resources that intersect this region
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

}

bool volm_query_satellite_resources_kml_process(bprb_func_process& pro)
{
  // check inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " invalid inputs" << std::endl;
    return false;
  }

  // get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  std::string kml_file = pro.get_input<std::string>(1);
  std::string out_file = pro.get_input<std::string>(2);
  std::string band = pro.get_input<std::string>(3);
  bool pick_seed = pro.get_input<bool>(4);
  int n_seeds = pro.get_input<int>(5);
  auto gsd_thres = pro.get_input<double>(6);

  // parse the polygon and construct its bounding box
  if (!vul_file::exists(kml_file)) {
    std::cout << pro.name() << ": can not find input kml file: " << kml_file << std::endl;
    return false;
  }
  vgl_polygon<double> poly = bkml_parser::parse_polygon(kml_file);
  vgl_box_2d<double> bbox;
  for (auto i : poly[0])
    bbox.add(i);
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
  std::vector<std::string> input_types_(2);
  input_types_[0] = "volm_satellite_resources_sptr";
  input_types_[1] = "vcl_string"; // folder -- will be traversed recursively to find NITF files and add to the resources

  if (!pro.set_input_types(input_types_))
    return false;
  //output
  std::vector<std::string> output_types_(0);
  return pro.set_output_types(output_types_);
}

bool volm_add_satellite_resources_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  std::string folder = pro.get_input<std::string>(1);
  res->add_path(folder);

  std::cout << " AFTER addition, there are " << res->resources_size() << " resources in the file!\n";
  return true;
}

bool volm_pick_nadir_resource_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types_(8);
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
  std::vector<std::string> output_types_(1);
  output_types_[0] = "vcl_string";  // full path of the satellite image
  return pro.set_output_types(output_types_);
}

bool volm_pick_nadir_resource_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  auto lower_left_lon = pro.get_input<double>(1);
  auto lower_left_lat = pro.get_input<double>(2);
  auto upper_right_lon = pro.get_input<double>(3);
  auto upper_right_lat = pro.get_input<double>(4);
  std::string band = pro.get_input<std::string>(5);
  std::string sat_name = pro.get_input<std::string>(6);
  std::string non_cloud_folder = pro.get_input<std::string>(7);

  std::vector<unsigned> ids;
  res->query(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, band, ids,10.0); // pass gsd_thres very high, only interested in finding all the images that intersect the box
  double largest_view_angle = -100.0;
  unsigned id = 0;
  for (unsigned int i : ids) {
    if (res->resources_[i].meta_->satellite_name_.compare(sat_name) == 0) {
      std::cout << "res: " << res->resources_[i].name_
               << " view azimuth: " << res->resources_[i].meta_->view_azimuth_
               << " view elev: " << res->resources_[i].meta_->view_elevation_ << '\n';
      // pick the image with largest view angle and least could coverage if possible
      if (largest_view_angle < res->resources_[i].meta_->view_elevation_) {
        // check whether this image is non-cloud one or not
        if (non_cloud_folder.compare("") != 0) {
          std::string non_cloud_img = non_cloud_folder + "/" + res->resources_[i].name_ + "_cropped.tif";
          if (vul_file::exists(non_cloud_img)) {
            largest_view_angle = res->resources_[i].meta_->view_elevation_;
            id = i;
          }
        }
        else {  // TO DO -- add edge detection here if non-cloud images folder is not given
          largest_view_angle = res->resources_[i].meta_->view_elevation_;
          id = i;
        }
      }
    }
  }
  std::cout << "picked: " << res->resources_[id].full_path_ << std::endl;
  pro.set_output_val<std::string>(0, res->resources_[id].full_path_);
  return true;
}

// find the non-cloud PAN/MULTI pair from satellite resource
// Note it will output all PAN/MULTI pairs that intersect with current scene, sorted by the view angles
bool volm_pick_nadir_resource_pair_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types_(9);
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
  std::vector<std::string> output_types_(2);
  output_types_[0] = "vcl_string";  // full path of the PAN image
  output_types_[1] = "vcl_string";  // full path of the MULTI image
  return pro.set_output_types(output_types_) && pro.set_input_types(input_types_);
}

bool volm_pick_nadir_resource_pair_process(bprb_func_process& pro)
{
  // input check
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " invalid inputs" << std::endl;
    return false;
  }
  // get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  auto lower_left_lon = pro.get_input<double>(1);
  auto lower_left_lat = pro.get_input<double>(2);
  auto upper_right_lon = pro.get_input<double>(3);
  auto upper_right_lat = pro.get_input<double>(4);
  std::string band = pro.get_input<std::string>(5);
  std::string sat_name = pro.get_input<std::string>(6);
  std::string non_cloud_folder = pro.get_input<std::string>(7);
  std::string out_folder = pro.get_input<std::string>(8);
  // obtain resources having given band that overlap with current region
  std::vector<unsigned> ids;
  res->query(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, band, ids,10.0);
  // map of resources that have given band and sat_name, sorted by the view angle
  std::map<double, unsigned, std::greater<double> > band_res;
  for (unsigned int & id : ids)
    if (res->resources_[id].meta_->satellite_name_.compare(sat_name) == 0)
      band_res.insert(std::pair<double, unsigned>(res->resources_[id].meta_->view_elevation_, id));

  // text file where the sorted PAN/MULTI pair will be stored
  std::string out_txt = out_folder + "/pan_multi_pair_list.txt";
  std::ofstream ofs(out_txt.c_str());
  ofs << "view_angle(deg) \t pan_img \t multi_img \t" << std::endl;

  // find the PAN/MULTI pair
  std::map<double, std::pair<std::string, std::string>, std::greater<double> > pairs;
  for (auto & band_re : band_res) {
    std::string img_name = res->resources_[band_re.second].name_;
    std::string pair_name = res->find_pair(img_name);
    if (pair_name.compare("") == 0)
      continue;
    if (non_cloud_folder.compare("") != 0)
    {
      std::string non_cloud_img_band = non_cloud_folder + "/" + img_name  + "_cropped.tif";
      std::cout << " view_angle = " << band_re.first << " img_name = " << res->resources_[band_re.second].full_path_
             << " pair_name = " << res->full_path(pair_name).first
             << " non_cloud_img = " << non_cloud_img_band
             << std::endl;
      if (vul_file::exists(non_cloud_img_band)) {
        std::pair<std::string, std::string> name_pair;
        if (band == "PAN") {
          name_pair.first  = res->resources_[band_re.second].full_path_;
          name_pair.second = res->full_path(pair_name).first;
        }
        else if (band == "MULTI") {
          name_pair.first  = res->full_path(pair_name).first;
          name_pair.second = res->resources_[band_re.second].full_path_;
        }
        else {
          std::cout << pro.name() << ": unknown input band " << band << std::endl;
          return false;
        }
        pairs.insert(std::pair<double, std::pair<std::string, std::string> >(band_re.first, name_pair));
      }
    }
    else {
      std::pair<std::string, std::string> name_pair;
      if (band == "PAN") {
        name_pair.first = res->resources_[band_re.second].full_path_;
        name_pair.second = res->full_path(pair_name).first;
      } else if (band == "MULTI") {
        name_pair.first = res->full_path(pair_name).first;
        name_pair.second = res->resources_[band_re.second].full_path_;
      } else {
        std::cout << pro.name() << ": unknown input band " << band << std::endl;
        return false;
      }
      pairs.insert(std::pair<double, std::pair<std::string, std::string> >(band_re.first, name_pair));
    }
  }
  // output
  if (pairs.size() == 0) {
    std::cout << pro.name() << ": can not find any PAN/MULTI pair for current scene" << std::endl;
    return false;
  }

  for (auto & mit : pairs)
    ofs << mit.first << " \t " << mit.second.first << " \t " << mit.second.second << std::endl;
  ofs.close();

  pro.set_output_val<std::string>(0, pairs.begin()->second.first);
  pro.set_output_val<std::string>(1, pairs.begin()->second.second);

#if 0
  for (std::map<double, unsigned, std::greater<double> >::iterator mit = band_res.begin(); mit != band_res.end(); ++mit) {
    std::string img_name = res->resources_[mit->second].name_;
    std::string pair_name = res->find_pair(img_name);
    if (pair_name.compare("") == 0)
      continue;
    if (non_cloud_folder.compare("") != 0)
    {
      std::string non_cloud_img_band = non_cloud_folder + "/" + img_name  + "_cropped.tif";
      if (vul_file::exists(non_cloud_img_band)) {
        std::string pan_path, multi_path;
        if (band == "PAN") {
          pan_path = res->resources_[mit->second].full_path_;
          std::pair<std::string, std::string> pp = res->full_path(pair_name);
          multi_path = pp.first;
        }
        else if (band == "MULTI") {
          multi_path = res->resources_[mit->second].full_path_;
          std::pair<std::string, std::string> pp = res->full_path(pair_name);
          pan_path = pp.first;
        }
        else {
          std::cout << pro.name() << ": unknown input band " << band << std::endl;
          return false;
        }
        pro.set_output_val<std::string>(0, pan_path);
        pro.set_output_val<std::string>(1, multi_path);
        return true;
      }
    }
    else // TO DO -- add edge detection here if non-cloud images folder is not given
    {
      std::string pan_path, multi_path;
      if (band == "PAN") {
        pan_path = res->resources_[mit->second].full_path_;
        std::pair<std::string, std::string> pp = res->full_path(pair_name);
        multi_path = pp.first;
      }
      else if (band == "MULTI") {
        multi_path = res->resources_[mit->second].full_path_;
        std::pair<std::string, std::string> pp = res->full_path(pair_name);
        pan_path = pp.first;
      }
      else {
        std::cout << pro.name() << ": unknown input band " << band << std::endl;
        return false;
      }
      pro.set_output_val<std::string>(0, pan_path);
      pro.set_output_val<std::string>(1, multi_path);
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
  std::vector<std::string> input_types_(2);
  input_types_[0] = "volm_satellite_resources_sptr";
  input_types_[1] = "vcl_string";      // satellite img name

  if (!pro.set_input_types(input_types_))
    return false;
  //output
  std::vector<std::string> output_types_(1);
  output_types_[0] = "vcl_string";  // full path of the satellite whose name is passed
  return pro.set_output_types(output_types_);
}

bool volm_get_full_path_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  std::string name = pro.get_input<std::string>(1);
  std::pair<std::string, std::string> full = res->full_path(name);
  pro.set_output_val<std::string>(0, full.first);
  return true;
}

// find the PAN counterpart if given a multi band image, and vice versa
bool volm_find_res_pair_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types_(3);
  input_types_[0] = "volm_satellite_resources_sptr";
  input_types_[1] = "vcl_string";      // satellite img name  // only the name, don't need the full path
  input_types_[2] = "double";          // tolerance of footprint difference ( in meters)
  if (!pro.set_input_types(input_types_))
    return false;
  //output
  std::vector<std::string> output_types_(3);
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
    std::cout << pro.name() << " invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  std::string name = pro.get_input<std::string>(1);
  auto tol = pro.get_input<double>(2);
  std::pair<std::string, std::string> full = res->full_path(name);
  std::string pair_name = res->find_pair(name, tol);
  std::cout << "pair_name = " << pair_name << std::endl;
  if (pair_name.compare("") == 0) {
    pro.set_output_val<std::string>(0, "");
    pro.set_output_val<std::string>(1, "");
    pro.set_output_val<std::string>(2, "");
    return false;
  }
  std::pair<std::string, std::string> pp = res->full_path(pair_name);
  pro.set_output_val<std::string>(0, full.first);
  pro.set_output_val<std::string>(1, pair_name);
  pro.set_output_val<std::string>(2, pp.first);
  return true;
}

// a process to return a text file with names of 'pairs' of satellite images that are taken a few minutes apart from each other
bool volm_find_satellite_pairs_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types_(8);
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
  std::vector<std::string> output_types_(1);
  output_types_[0] = "unsigned";  // return number of pairs that intersect this region
  return pro.set_output_types(output_types_);
}

bool volm_find_satellite_pairs_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  auto lower_left_lon = pro.get_input<double>(1);
  auto lower_left_lat = pro.get_input<double>(2);
  auto upper_right_lon = pro.get_input<double>(3);
  auto upper_right_lat = pro.get_input<double>(4);
  std::string out_file = pro.get_input<std::string>(5);
  std::string sat_name = pro.get_input<std::string>(6);
  auto GSD_thres = pro.get_input<float>(7);

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
  std::vector<std::string> input_types_(5);
  input_types_[0] = "volm_satellite_resources_sptr";
  input_types_[1] = "vcl_string";      // kml file that gives the region of interest polygon
  input_types_[2] = "vcl_string";      // output file to print the list
  input_types_[3] = "vcl_string";      // satellite name
  input_types_[4] = "float";       // GSD_threshold
  if (!pro.set_input_types(input_types_))
    return false;
  //output
  std::vector<std::string> output_types_(1);
  output_types_[0] = "unsigned";  // return number of pairs that intersect this region
  return pro.set_output_types(output_types_);
}

bool volm_find_satellite_pairs_poly_process(bprb_func_process& pro)
{
  // check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " invalid inputs" << std::endl;
    return false;
  }
  //get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  std::string poly_file = pro.get_input<std::string>(1);
  std::string out_file = pro.get_input<std::string>(2);
  std::string sat_name = pro.get_input<std::string>(3);
  auto GSD_thres = pro.get_input<float>(4);

  // find the bbox of the polygon
  vgl_polygon<double> poly = bkml_parser::parse_polygon(poly_file);
  std::cout << "outer poly  has: " << poly[0].size() << " points " << std::endl;

  // find the bbox of ROI from its polygon
  vgl_box_2d<double> bbox;
  for (auto i : poly[0]) {
    bbox.add(i);
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
  std::vector<std::string> input_types_(2);
  input_types_[0] = "vcl_string";      // satellite image 1
  input_types_[1] = "vcl_string";      // satellite image 2
  if (!pro.set_input_types(input_types_))
    return false;
  //output
  std::vector<std::string> output_types_(5);
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
    std::cout << pro.name() << " invalid inputs" << std::endl;
    return false;
  }
  //get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  std::string img_file1 = pro.get_input<std::string>(1);
  std::string img_file2 = pro.get_input<std::string>(2);

  if (!vul_file::exists(img_file1)) {
    std::cout << pro.name() << ": " << img_file1 << " is missing" << std::endl;
    return false;
  }
  if (!vul_file::exists(img_file2)) {
    std::cout << pro.name() << ": " << img_file2 << " is missing" << std::endl;
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
  double area = vgl_area(intersection_bbox);

  // generate output
  unsigned i = 0;
  pro.set_output_val<double>(i++, intersection_bbox.min_x());
  pro.set_output_val<double>(i++, intersection_bbox.min_y());
  pro.set_output_val<double>(i++, intersection_bbox.max_x());
  pro.set_output_val<double>(i++, intersection_bbox.max_y());
  pro.set_output_val<double>(i++, area);
  return true;



}
