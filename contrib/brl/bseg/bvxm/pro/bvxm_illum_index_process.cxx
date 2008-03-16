#include "bvxm_illum_index_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vpgl/bgeo/bgeo_lvcs_sptr.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>

#include <vcl_cmath.h>

bvxm_illum_index_process::bvxm_illum_index_process()
{
    //inputs
    // 0: Map type :
    //        - 10bins_1d_radial
    //        - eq_area
    // 1: NITF image path
    // 2: Number of latitude regions
    // 3: Number of "longitudinal" regions
    // 4: local vertical coordinates system : To be added

    input_data_.resize(4,brdb_value_sptr(0));
    input_types_.resize(4);
    input_types_[0] = "vcl_string";
    input_types_[1] = "vcl_string";
    input_types_[2] = "unsigned";
    input_types_[3] = "unsigned";
    //input_types_[3] = "bgeo_lvcs_sptr";

    //output
    //0: bin index
    output_data_.resize(1,brdb_value_sptr(0));
    output_types_.resize(1);
    output_types_[0]= "unsigned";
}


bool bvxm_illum_index_process::execute()
{
    // Sanity check
    if (!this->verify_inputs())
        return false;

     //get the inputs
    brdb_value_t<vcl_string>* input0 =
        static_cast<brdb_value_t<vcl_string>* >(input_data_[0].ptr());

    vcl_string map_type = input0->value();

    //get the inputs
    brdb_value_t<vcl_string>* input1 =
        static_cast<brdb_value_t<vcl_string>* >(input_data_[1].ptr());

    vcl_string nitf_image_path = input1->value();

    brdb_value_t<unsigned>* input2 =
        static_cast<brdb_value_t<unsigned>* >(input_data_[2].ptr());

    unsigned num_lat = input2->value();

   brdb_value_t<unsigned>* input3 =
        static_cast<brdb_value_t<unsigned>* >(input_data_[3].ptr());

    unsigned num_long = input3->value();

    //read and parse NITF image

    //open the nitf image
    vil_image_resource_sptr image =
        vil_load_image_resource(nitf_image_path.c_str());
    if (!image)
    {
        vcl_cout << "NITF image load failed in bvxm_illum_index_process\n";
        return 0;
    }

    vcl_string format = image->file_format();
    vcl_string prefix = format.substr(0,4);

    if (prefix != "nitf")
    {
        vcl_cout << "source image is not NITF in bvxm_illum_index_process\n";
        return 0;
    }

    //cast to an nitf2_image
    vil_nitf2_image *nitf_image = static_cast<vil_nitf2_image*>(image.ptr());

    //get NITF information
     vcl_vector< vil_nitf2_image_subheader* > headers = nitf_image->get_image_headers();
     vil_nitf2_image_subheader* hdr = headers[0];

     double sun_el;
     double sun_az;

     bool success = hdr->get_sun_params(sun_el, sun_az);

     if (!success){
       vcl_cerr << "error bvxm_illum_index_process: failed to obtain illumination angles from nitf image\n";
       return false;
     }

     unsigned bin_idx = this->bin_index(map_type, sun_el, sun_az, num_lat, num_long);

    //store output
    brdb_value_sptr output0 =
        new brdb_value_t<unsigned>(bin_idx);
    output_data_[0] = output0;

    return true;
}


unsigned bvxm_illum_index_process::bin_index(vcl_string map_type, double sun_el, double sun_az,
                       unsigned num_lat, unsigned num_long)
{
  unsigned bin_idx = 0;
  //read and parse XML file
  if (map_type == "10bins_1d_radial")
  {
    unsigned num_light_bins = num_lat;
    bin_idx = (unsigned)( num_light_bins*( sun_el + 90 )/180 );
    if ( bin_idx < 0 ) bin_idx = 0;
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
    double sin_el = vcl_sin(sun_el);
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
