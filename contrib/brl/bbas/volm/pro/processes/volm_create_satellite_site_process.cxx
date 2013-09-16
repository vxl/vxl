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

//: sets input and output types
bool volm_create_satellite_resources_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(2);
  input_types_[0] = "vcl_string";      // polygon file (kml) of ROI
  input_types_[1] = "float";      // polygon file (kml) of ROI
  
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

  // find the bbox of the polygon
  vgl_polygon<double> poly = bkml_parser::parse_polygon(poly_file);
  vcl_cout << "outer poly  has: " << poly[0].size() << " points " << vcl_endl;

  // find the bbox of ROI from its polygon
  vgl_box_2d<double> bbox;
  for (unsigned i = 0; i < poly[0].size(); i++) {
    bbox.add(poly[0][i]);
  }
  vcl_cout << "bbox of ROI: " << bbox << vcl_endl;

  volm_satellite_resources_sptr res = new volm_satellite_resources(bbox, leaf_size);
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
  vcl_vector<vcl_string> input_types_(9);
  input_types_[0] = "volm_satellite_resources_sptr"; 
  input_types_[1] = "double";      // lower left lon
  input_types_[2] = "double";      // lower left lat
  input_types_[3] = "double";      // upper right lon
  input_types_[4] = "double";      // upper right lat
  input_types_[5] = "vcl_string";      // output file to print the list
  input_types_[6] = "vcl_string";      // the band: PAN or MULTI 
  input_types_[7] = "bool";      // if TRUE, pick seed images randomly with a certain order of satellites
  input_types_[8] = "int";       // number of seed images to pick, if not enough then creates from all available
  
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
  
  unsigned cnt; bool out = false;
  if (!pick_seed) {
    out = res->query_print_to_file(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, cnt, out_file, band);
    pro.set_output_val<unsigned>(0, cnt);
  } else {
    out = res->query_seeds_print_to_file(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, n_seeds, cnt, out_file, band);
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
  vcl_vector<vcl_string> input_types_(7);
  input_types_[0] = "volm_satellite_resources_sptr"; 
  input_types_[1] = "double";      // lower left lon
  input_types_[2] = "double";      // lower left lat
  input_types_[3] = "double";      // upper right lon
  input_types_[4] = "double";      // upper right lat
  input_types_[5] = "vcl_string";      // the band: PAN or MULTI 
  input_types_[6] = "vcl_string";      // satelllite name
  
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
  
  vcl_vector<unsigned> ids;
  res->query(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, band, ids);
  double largest_view_angle = 0.0;
  unsigned id = 0;  
  for (unsigned i = 0; i < ids.size(); i++) {
    if (res->resources_[ids[i]].meta_->satellite_name_.compare(sat_name) == 0) {
      vcl_cout << "res: " << res->resources_[ids[i]].name_ << " view azimuth: " << res->resources_[ids[i]].meta_->view_azimuth_ << " view elev: " << res->resources_[ids[i]].meta_->view_elevation_ << '\n';
      if (largest_view_angle < res->resources_[ids[i]].meta_->view_elevation_) {
        largest_view_angle = res->resources_[ids[i]].meta_->view_elevation_;
        id = ids[i];
      }
    }
  }
  vcl_cout << "picked: " << res->resources_[id].full_path_ << vcl_endl;
  pro.set_output_val<vcl_string>(0, res->resources_[id].full_path_);
  return true;
}

