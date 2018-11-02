// This is brl/bbas/volm/processes/vlm_project_dem_to_sat_img_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Process to find a set of highly overlapping resource footprints
//
// \author Scott Richardson
// \date May 11, 2015
// \verbatim
// \endverbatim
//

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include <vul/vul_file.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <volm/volm_satellite_resources.h>
#include <volm/volm_satellite_resources_sptr.h>
#include <bkml/bkml_write.h>


//: global variables and functions
namespace volm_find_overlapping_sat_resources_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;
}

//: constructor
bool volm_find_overlapping_sat_resources_process_cons(bprb_func_process& pro)
{
  using namespace volm_find_overlapping_sat_resources_process_globals;
  // process takes 4 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "volm_satellite_resources_sptr"; // satellite resource
  input_types_[1] = "vcl_string";                    // kml polygon filename
  input_types_[2] = "float";                         // factor by which to downsample resource footprints when
                                                     // computing the raster (smaller factor takes more time & memory)
  input_types_[3] = "vcl_string";                    // output file to print the list

  // process takes 0 outputs
  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: execute the process
bool volm_find_overlapping_sat_resources_process(bprb_func_process& pro)
{
  using namespace volm_find_overlapping_sat_resources_process_globals;
  // sanity check
  if (pro.n_inputs() != n_inputs_) {
    std::cout << pro.name() << ": there should be " << n_inputs_ << " inputs" << std::endl;
    return false;
  }
  // get the input
  unsigned in_i = 0;
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  std::string kml_file = pro.get_input<std::string>(1);
  auto downsample_factor = pro.get_input<float>(2);
  std::string out_file = pro.get_input<std::string>(3);

  std::vector<std::string> overlapping_res;
  res->highly_overlapping_resources(overlapping_res, res, kml_file, downsample_factor);


  // save txt file of nitf filenames
  unsigned cnt = overlapping_res.size();
  if (out_file.compare("") == 0)
    return true;
  std::ofstream ofs(out_file.c_str());
  if (!ofs) {
    std::cerr << pro.name() << " ERROR: cannot open file: " << out_file << std::endl;
    return false;
  }
  for (const auto & overlapping_re : overlapping_res)
    ofs << overlapping_re << '\n';
  ofs.close();

  return true;
}
