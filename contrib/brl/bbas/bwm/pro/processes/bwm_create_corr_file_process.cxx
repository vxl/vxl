//This is brl/bbas/bwm/pro/processes/bwm_create_corr_file_process.cxx
//:
// \file
#include <string>
#include <iostream>
#include <cstdio>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <brdb/brdb_value.h>
#include <brip/brip_vil_float_ops.h>

#include <bwm/io/bwm_site_sptr.h>
#include <bwm/io/bwm_site.h>
#include <bwm/io/bwm_io_config_parser.h>

//: sets input and output types
bool bwm_create_corr_file_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types_(4);
  input_types_[0] = "vcl_string"; // current file  // writes the current file back as output but with new corrs added
  input_types_[1] = "vcl_string";      // path to the camera folder
  input_types_[2] = "vcl_string"; // the site file to add correspondences from
  input_types_[3] = "int";        // the number of correspondences to add to the output file, e.g. regular refinement only requires one correspondence

  if (!pro.set_input_types(input_types_))
    return false;
  //output
  std::vector<std::string> output_types_(0);
  return pro.set_output_types(output_types_);
}

bool bwm_create_corr_file_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  std::string file_name = pro.get_input<std::string>(0);
  std::string cam_path = pro.get_input<std::string>(1);
  std::string site_file = pro.get_input<std::string>(2);
  int corr_cnt = pro.get_input<int>(3);

  //parse the site file
  bwm_io_config_parser* parser = new bwm_io_config_parser();
  std::FILE* xmlFile = std::fopen(site_file.c_str(), "r");
  if (!xmlFile) {
    std::cerr << site_file.c_str() << " error on opening\n";
    delete parser;
    return false;
  }
  if (!parser->parseFile(xmlFile)) {
    std::cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
             << parser->XML_GetCurrentLineNumber() << '\n';

    delete parser;
    return false;
  }

  bwm_site_sptr site = parser->site();

  std::vector<std::vector<std::pair<std::string, vsol_point_2d> > > site_correspondences = site->corresp_;
  if (!site_correspondences.size() || !site_correspondences[0].size()) {
    std::cout << "The file: " << site_file << " does not contain any correspondences, returning!\n";
    return true;
  }

  corr_cnt = corr_cnt > (int)site_correspondences.size() ? (int)site_correspondences.size() : corr_cnt;

  // first open corr_file in read mode, see if it already contains any corrs
  std::ifstream corr_file_r(file_name.c_str(), std::ios::in);
  if (!corr_file_r) {
    std::ofstream corr_file(file_name.c_str(), std::ios::out);
    corr_file << corr_cnt << '\n';
    corr_file.close();
  }
  else
    corr_file_r.close();

  // now open corr_file in append mode
  std::ofstream corr_file(file_name.c_str(), std::ios::app);

  if (!corr_file) {
    std::cout << "error in opening: " << file_name << std::endl;
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
