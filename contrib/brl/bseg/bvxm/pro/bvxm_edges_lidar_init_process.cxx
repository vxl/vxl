#include "bvxm_edges_lidar_init_process.h"
//:
// \file
#include "bvxm_voxel_world.h"
#include <bvxm/bvxm_util.h>

#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
// unused? #include <vil/file_formats/vil_tiff.h>
// unused? #include <vil/file_formats/vil_geotiff_header.h>

#include <bgeo/bgeo_utm.h>
#include <brip/brip_roi.h>

#include <bprb/bprb_parameters.h>

bvxm_edges_lidar_init_process::bvxm_edges_lidar_init_process()
{
  //this process takes 2 inputs:
  //the first and second return images
  input_data_.resize(2, brdb_value_sptr(0));
  input_types_.resize(2);
  input_types_[0] = "vil_image_view_base_sptr";  // first ret image ROI
  input_types_[1] = "vil_image_view_base_sptr";  // second ret image ROI

  //output
  output_data_.resize(2,brdb_value_sptr(0));
  output_types_.resize(2);
  output_types_[0]= "vil_image_view_base_sptr";  // lidar edge image
  output_types_[1]= "vil_image_view_base_sptr";  // lidar edge probability

  // adding parameters
  parameters()->add("threshold_edge_difference", "threshold_edge_difference", 10.0f);
}

bool bvxm_edges_lidar_init_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  // get the inputs:
  // image
  brdb_value_t<vil_image_view_base_sptr>* input0 = static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr image_first_return_base = input0->value();
  vil_pixel_format input_pixel_format = image_first_return_base->pixel_format();

  brdb_value_t<vil_image_view_base_sptr>* input1 = static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());
  vil_image_view_base_sptr image_second_return_base = input1->value();

  bool is_input_float = true;
  if (input_pixel_format == VIL_PIXEL_FORMAT_BYTE){
    is_input_float = false;
  }
  else if (input_pixel_format == VIL_PIXEL_FORMAT_FLOAT){
    is_input_float = true;
  }
  else {
    vcl_cout << "this process only accepts vxl_float or vxl_byte pixel formats\n";
    return false;
  }


  vil_image_view<float> image_first_return = *(vil_convert_cast(float(), image_first_return_base));
  vil_image_view<float> image_second_return = *(vil_convert_cast(float(), image_second_return_base));

  // todo : check if this is given in meters
  float threshold_edge_difference;
  if (!parameters()->get_value("threshold_edge_difference", threshold_edge_difference)) {
    vcl_cout << "problems in retrieving parameters\n";
    return false;
  }

  unsigned ni = image_first_return.ni();
  unsigned nj = image_first_return.nj();

  vil_image_view<float> edges_lidar(ni,nj);
  edges_lidar.fill(0.0f);
  vil_image_view<float> edges_prob(ni,nj);
  edges_prob.fill(0.0f);

  for (unsigned i=0; i<ni; i++){
    for (unsigned j=0; j<nj; j++){
      float curr_difference = image_first_return(i,j)-image_second_return(i,j);
      if (curr_difference>threshold_edge_difference){
        edges_lidar(i,j) = image_first_return(i,j);
        // todo : fix this probability estimation here
        edges_prob(i,j) = 1.0f - 0.5f*(1.0f/(1.0f+curr_difference-threshold_edge_difference));
      }
    }
  }

  // store image output
  output_data_[0] = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(edges_lidar));

  // store prob output
  output_data_[1] = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(edges_prob));

  return true;
}
