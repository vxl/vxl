//This is brl/bbas/bwm/pro/processes/bwm_create_satellite_site_process.cxx
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h> // for std::FILE and std::fopen()

#include <brdb/brdb_value.h>

#include <bwm/algo/bwm_satellite_resources.h>
#include <bwm/algo/bwm_satellite_resources_sptr.h>

#include <bwm/io/bwm_site_sptr.h>
#include <bwm/io/bwm_site.h>
#include <bwm/io/bwm_io_config_parser.h>

//: sets input and output types
bool bwm_create_satellite_resources_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(5);
  input_types_[0] = "vcl_string"; // folder -- will be traversed recursively to find NITF files and create the resources
  input_types_[1] = "double";      // lower left corner latitude
  input_types_[2] = "double";      // lower left corner longitude 
  input_types_[3] = "double";      // upper right corner latitude
  input_types_[4] = "double";      // upper right corner longitude 

  if (!pro.set_input_types(input_types_))
    return false;
  //output
  vcl_vector<vcl_string> output_types_(1);
  output_types_[0] = "bwm_satellite_resources_sptr";
  return pro.set_output_types(output_types_);
}

bool bwm_create_satellite_resources_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << " invalid inputs" << vcl_endl;
    return false;
  }

  //get the inputs
  vcl_string folder = pro.get_input<vcl_string>(0);
  double lower_left_lat = pro.get_input<double>(1);
  double lower_left_lon = pro.get_input<double>(2);
  double upper_right_lat = pro.get_input<double>(3);
  double upper_right_lon = pro.get_input<double>(4);
  
  bwm_satellite_resources_sptr res = new bwm_satellite_resources(folder, lower_left_lat, lower_left_lon, 
                                                                         upper_right_lat, upper_right_lon);

  pro.set_output_val<bwm_satellite_resources_sptr>(0, res);

  return true;
}


/*

//: sets input and output types
bool bwm_create_satellite_site_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(4);
  
  if (!pro.set_input_types(input_types_))
    return false;
  //output
  vcl_vector<vcl_string> output_types_(0);
  return pro.set_output_types(output_types_);
}

bool bwm_create_satellite_site_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << " invalid inputs" << vcl_endl;
    return false;
  }

  //get the inputs
  vcl_string file_name = pro.get_input<vcl_string>(0);

  
  bwm_site_sptr site = parser->site();

  vcl_vector<vcl_vector<vcl_pair<vcl_string, vsol_point_2d> > > site_correspondences = site->corresp_;
  if (!site_correspondences.size() || !site_correspondences[0].size()) {
    vcl_cout << "The file: " << site_file << " does not contain any correspondences, returning!\n";
    return true;
  }

  corr_cnt = corr_cnt > (int)site_correspondences.size() ? (int)site_correspondences.size() : corr_cnt;

  // first open corr_file in read mode, see if it already contains any corrs
  vcl_ifstream corr_file_r(file_name.c_str(), vcl_ios::in);
  if (!corr_file_r) {
    vcl_ofstream corr_file(file_name.c_str(), vcl_ios::out);
    corr_file << corr_cnt << '\n';
    corr_file.close();
  }
  else
    corr_file_r.close();

  // now open corr_file in append mode
  vcl_ofstream corr_file(file_name.c_str(), vcl_ios::app);

  if (!corr_file) {
    vcl_cout << "error in opening: " << file_name << vcl_endl;
    return false;
  }

  // now turn the corrs into the format: <cam name> corr1 x corr1 y corr2x corr2y ...
  int cam_size = site_correspondences[0].size();
  for (int j = 0; j < cam_size; j++) {
    corr_file << site->path_ << "\\cameras\\" << site_correspondences[0][j].first << ".RPB ";
    for (int i = 0; i < corr_cnt; i++) {
      corr_file << site_correspondences[i][j].second.x() << ' ' << site_correspondences[i][j].second.y() << ' ';
    }
    corr_file << '\n';
  }

  corr_file.close();
 
  
  return true;
}
*/

