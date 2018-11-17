//This is brl/bseg/bvxm/pro/processes/bvxm_illum_index_process.cxx
#include <iostream>
#include <cmath>
#include <vector>
#include "bvxm_illum_index_process.h"
//:
// \file

#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: set input and output types
bool bvxm_illum_index_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_illum_index_process_globals;

  //inputs
  // 0: Map type :
  //        - 10bins_1d_radial
  //        - eq_area
  // 1: NITF image path
  // 2: Number of latitude regions
  // 3: Number of "longitudinal" regions
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";
  input_types_[1] = "vcl_string";
  input_types_[2] = "unsigned";
  input_types_[3] = "unsigned";
  if (!pro.set_input_types(input_types_))
    return false;

  //output
   //0: bin index
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0]= "unsigned";
  return pro.set_output_types(output_types_);
}

bool bvxm_illum_index_process(bprb_func_process& pro)
{
  using namespace bvxm_illum_index_process_globals;

  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  std::string map_type = pro.get_input<std::string>(i++);
  std::string nitf_image_path = pro.get_input<std::string>(i++);
  auto num_lat = pro.get_input<unsigned>(i++);
  auto num_long = pro.get_input<unsigned>(i++);

  //read and parse NITF image

  vil_image_resource_sptr image =
      vil_load_image_resource(nitf_image_path.c_str());
  if (!image)
  {
    std::cout << "NITF image load failed in bvxm_illum_index_process\n";
    return false;
  }

  std::string format = image->file_format();
  std::string prefix = format.substr(0,4);

  if (prefix != "nitf")
  {
    std::cout << "source image is not NITF in bvxm_illum_index_process\n";
    return false;
  }

    //cast to an nitf2_image
  auto *nitf_image = static_cast<vil_nitf2_image*>(image.ptr());

    //get NITF information
  std::vector< vil_nitf2_image_subheader* > headers = nitf_image->get_image_headers();
  vil_nitf2_image_subheader* hdr = headers[0];

  double sun_el;
  double sun_az;

  bool success = hdr->get_sun_params(sun_el, sun_az);

  if (!success) {
    std::cerr << "error bvxm_illum_index_process: failed to obtain illumination angles from nitf image\n";
    return false;
  }

  unsigned bin_idx =bin_index(map_type, sun_el, sun_az, num_lat, num_long);

  //store bin index
  pro.set_output_val<unsigned>(0, bin_idx);

  return true;
}


unsigned
bvxm_illum_index_process_globals::bin_index(const std::string& map_type,
                                            double sun_el, double sun_az,
                                            unsigned num_lat, unsigned num_long)
{
  unsigned bin_idx = 0;
  //read and parse XML file
  if (map_type == "10bins_1d_radial")
  {
    unsigned num_light_bins = num_lat;
    bin_idx = (unsigned)( num_light_bins*( sun_el + 90 )/180 );
    if ( bin_idx >= num_light_bins) bin_idx = num_light_bins-1;
  }
  else if (map_type == "eq_area")
  {
    unsigned lat_idx = 1;

    //if the user specifies zero regions it is consider equivalent to one region
    if (num_lat == 0)
      num_lat = 1;
    if (num_long ==0)
      num_long = 1;

    //area of latitude regions
    double lat_area = 1;
    //zero regions case is equivalent to one region case
    if (num_lat != 0)
      lat_area = 1/double(num_lat);

    //match sun elevation angle to latitudinal region
    double sin_el = std::sin(sun_el);
    for (unsigned i = 1; i <= num_lat; i++)
    {
      //general case
      if (((i -1) * lat_area <= sin_el ) && (sin_el < i * lat_area))
      {
        lat_idx = i;
        break;
      }
      //special case p/2

      if (sin_el == 1)
        lat_idx = num_lat;
    }

    //area of "half-longitudinal" regions
    double long_area =1;
    if (num_long != 0)
      long_area =double(360)/double(num_long);

    //match sun azimuthal angle to "half-longitudinal" region
    unsigned long_idx= 1;

    for (unsigned i = 1; i <= num_long; i++)
    {
      //zero case
      if (sun_az == 360.0)
        sun_az = 0;

      //general case
      if (( (i-1) * long_area <= sun_az) && ( sun_az < i * long_area))
      {
        long_idx = i;
        break;
      }
    }

    //get the bin number (-1 offset because the first region should be region 0)
    bin_idx = ((lat_idx-1) * num_long) + long_idx - 1;
  }
  else
    bin_idx = 0;

  return bin_idx;
}
