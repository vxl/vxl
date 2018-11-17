// This is brl/bpro/core/vpgl_pro/processes/vpgl_nitf_camera_coverage_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bpgl/algo/bpgl_nitf_camera_coverage.h>

//: initialization
bool vpgl_nitf_camera_coverage_process_cons(bprb_func_process& pro)
{
  //this process takes 3 inputs:
  // 1: the filename containing a list of images to evaluate
  // 2: Filename for region-points input file
  //    This file must contain the points in the following format
  //    x-coord1 y-coord1
  //    x-coord2 y-coord2
  //    Caution: Don't forget that in geo coordinates. x-coord = longitude, y-coord =latitude
  // 3: the filename for output coverage list
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");
  input_types.emplace_back("vcl_string");
  input_types.emplace_back("vcl_string");
  return pro.set_input_types(input_types);
}

// Get regions from a file.
void  get_regions(const std::string& file, std::vector< vgl_point_2d<double> > &region)
{
  region.clear();
  std::ifstream ifs( file.c_str() );
  while (!ifs.eof())
  {
    double x, y;
    ifs >> x; ifs >> y;
    region.emplace_back( x, y );
  }
}

//: Execute the process
bool vpgl_nitf_camera_coverage_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 3) {
    std::cout << "vpgl_nitf_camera_coverage_process: The number of inputs should be 3" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  std::string in_img_list = pro.get_input<std::string>(i++);
  std::string region_file = pro.get_input<std::string>(i++);
  std::string out_img_list = pro.get_input<std::string>(i++);

  std::vector<vgl_point_2d<double> > regions;

  get_regions(region_file, regions);

  if (!bpgl_nitf_camera_coverage::coverage_list(regions,in_img_list, out_img_list))
  {
    std::cerr << "Error vpgl_nitf_camera_coverage_process: Failed to get coverage list\n";
    return false;
  };

  return true;
}
