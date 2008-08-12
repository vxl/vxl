// This is brl/bpro/core/vpgl_pro/vpgl_nitf_camera_coverage_process.cxx
#include "vpgl_nitf_camera_coverage_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vpgl/algo/vpgl_nitf_camera_coverage.h>

//: Constructor
vpgl_nitf_camera_coverage_process::vpgl_nitf_camera_coverage_process()
{
  //this process takes 2 inputs: 
  // 1: the filename containg list of images to evaluate
  // 2: Filename for region-points input file
  //    This file must contain the points in the following format
  //    x-coord1 y-coord1
  //    x-coord2 y-coord2
  //    Caution: Don't forget that in geo coordinates. x-coord = longitude, y-coord =latitude
  // 3: the filename for output coverage list
  input_data_.resize(3,brdb_value_sptr(0));
  input_types_.resize(3);
  input_types_[0]= "vcl_string";
  input_types_[1]= "vcl_string";
  input_types_[2]= "vcl_string";

  //this process has no outputs
  output_data_.resize(0,brdb_value_sptr(0));
  output_types_.resize(0);
 
}


//: Destructor
vpgl_nitf_camera_coverage_process::~vpgl_nitf_camera_coverage_process()
{
}


// Get regions from a file.
void 
vpgl_nitf_camera_coverage_process::get_regions(vcl_string file,
                 vcl_vector< vgl_point_2d<double> > &region)
{
  region.clear();
  vcl_ifstream ifs( file.c_str() );
  while(!ifs.eof())
  {
      double x, y;
      ifs >> x; ifs >> y;
      region.push_back( vgl_point_2d<double>( x, y ) );
  }
};

//: Execute the process
bool
vpgl_nitf_camera_coverage_process::execute()
{
  // Sanity check
    if (!this->verify_inputs())
    return false;

  // Retrieve filename from input
  brdb_value_t<vcl_string>* input0 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[0].ptr());

  vcl_string in_img_list = input0->value();

    brdb_value_t<vcl_string>* input1 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[1].ptr());

  vcl_string region_file = input1->value();

      brdb_value_t<vcl_string>* input2 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[2].ptr());

  vcl_string out_img_list = input2->value();

 
  vcl_vector<vgl_point_2d<double>> regions;

  this->get_regions(region_file, regions);
 

  if (!vpgl_nitf_camera_coverage::coverage_list(regions,in_img_list, out_img_list))
  {
    vcl_cerr << "Error vpgl_nitf_camera_coverage_process: Failed to get coverage list" << vcl_endl;
    return false;
  };

  return true;
}

