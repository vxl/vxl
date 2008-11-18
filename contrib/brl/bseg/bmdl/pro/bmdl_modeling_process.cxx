#include "bmdl_modeling_process.h"
//:
// \file
#include "bmdl_lidar_roi_process.h"
#include "bmdl_classify_process.h"
#include "bmdl_trace_boundaries_process.h"
#include "bmdl_generate_mesh_process.h"

#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view_base.h>

#include <vpl/vpl.h>
#include <bprb/bprb_parameters.h>

//#include <vpgl/bgeo/bgeo_utm.h>
//#include <vpgl/bgeo/bgeo_lvcs.h>
//#include <brip/brip_roi.h>

bmdl_modeling_process::bmdl_modeling_process()
{
  //this process takes 2 input:
  //the file paths of the first return and second return
  input_data_.resize(8, brdb_value_sptr(0));
  input_types_.resize(8);

  int i=0;
  input_types_[i++] = "vcl_string";      // first ret. image path (geotiff)
  input_types_[i++] = "vcl_string";      // last ret. image path (geotiff)
  input_types_[i++] = "vcl_string";      // ground image (tiff)
  input_types_[i++] = "vcl_string";      // output directory
  input_types_[i++] = "unsigned";           // tile x dimension
  input_types_[i++] = "unsigned";           // tile y dimension
  input_types_[i++] = "unsigned";           // tile x overlapping
  input_types_[i++] = "unsigned";           // tile y overlapping

  //output
  output_data_.resize(0,brdb_value_sptr(0));
  output_types_.resize(0);

  if (!parameters()->add( "Ground Threshold" , "gthresh" , (float) 2.0 ))
    vcl_cerr << "ERROR: Adding parameters in bmdl_modeling_process\n";
}

bool bmdl_modeling_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  // get the inputs:
  // images
  brdb_value_t<vcl_string>* input0 = static_cast<brdb_value_t< vcl_string>* >(input_data_[0].ptr());
  vcl_string first = input0->value();

  brdb_value_t<vcl_string>* input1 = static_cast<brdb_value_t< vcl_string>* >(input_data_[1].ptr());
  vcl_string last = input1->value();

  brdb_value_t<vcl_string>* input2 = static_cast<brdb_value_t< vcl_string>* >(input_data_[2].ptr());
  vcl_string ground = input2->value();

  brdb_value_t<vcl_string>* input3 = static_cast<brdb_value_t< vcl_string>* >(input_data_[3].ptr());
  vcl_string output_path = input3->value();

  brdb_value_t<unsigned>* input4 = static_cast<brdb_value_t<unsigned>* >(input_data_[4].ptr());
  unsigned x_dim = input4->value();

  brdb_value_t<unsigned>* input5 = static_cast<brdb_value_t<unsigned>* >(input_data_[5].ptr());
  unsigned y_dim = input5->value();

  brdb_value_t<unsigned>* input6 = static_cast<brdb_value_t<unsigned>* >(input_data_[6].ptr());
  unsigned x_over = input6->value();

  brdb_value_t<unsigned>* input7 = static_cast<brdb_value_t<unsigned>* >(input_data_[7].ptr());
  unsigned y_over = input7->value();

  float gthresh=0.0;
  if (!parameters()->get_value("gthresh", gthresh)) {
    vcl_cout << "bmdl_modeling_process -- has problem getting the parameter!\n";
    return false;
  }

  // check first return's validity
  vil_image_resource_sptr first_ret = vil_load_image_resource(first.c_str());
  if (!first_ret) {
    vcl_cout << "bmdl_modeling_process -- First return image path is not valid!\n";
    return false;
  }

  // check last return's validity
  vil_image_resource_sptr last_ret = vil_load_image_resource(last.c_str());
  if (!last_ret) {
    vcl_cout << "bmdl_modeling_process -- Last return image path is not valid!\n";
    return false;
  }

  // Ground image path can be invalid or empty, in that case an estimated ground will be computed
  vil_image_resource_sptr ground_img =0;
  if (ground.size() > 0) {
    ground_img = vil_load_image_resource(ground.c_str());
  }

  vil_image_view_base_sptr first_roi=0, last_roi=0, ground_roi;
  vpgl_geo_camera* lidar_cam =0;
  if (!modeling(first_ret, last_ret, ground_img, output_path,
    x_dim, y_dim, x_over, y_over, gthresh)) {
    vcl_cout << "bmdl_modeling_process -- The process has failed!\n";
    return false;
  }

  return true;
}

bool bmdl_modeling_process::modeling(vil_image_resource_sptr lidar_first,
                                     vil_image_resource_sptr lidar_last,
                                     vil_image_resource_sptr ground,
                                     vcl_string output_path,
                                     unsigned x_dim, unsigned y_dim,
                                     unsigned x_overlap, unsigned y_overlap, 
                                     float gthresh)
{
  vil_image_view_base_sptr first_roi;
  vil_image_view_base_sptr last_roi;
  vil_image_view_base_sptr ground_roi;
  vil_image_view_base_sptr label_img;
  vil_image_view_base_sptr height_img;
  vpgl_geo_camera* geo_cam=0;
  unsigned num_of_buildings=0;

  // process each tile
  unsigned j_orig=0;
  while (j_orig < lidar_first->nj()) {
    unsigned i_orig=0;
    while (i_orig < lidar_first->ni()) {
      vcl_cout << "-------->Processing (" << i_orig << ',' << j_orig << ')' << vcl_endl;
      bmdl_lidar_roi_process lidar_roi_process;
      lidar_roi_process.lidar_roi(1, lidar_first, lidar_last, ground,
                                  i_orig, j_orig, i_orig+x_dim, j_orig+y_dim,
                                  first_roi, last_roi, ground_roi, geo_cam);

      bmdl_classify_process classify_process;
      classify_process.classify(first_roi, last_roi, ground_roi, label_img, height_img, gthresh);

      vcl_string poly_path = output_path + "//polygons.bin";
      bmdl_trace_boundaries_process trace_boundaries_process;
      trace_boundaries_process.trace_boundaries(label_img, poly_path);

      bmdl_generate_mesh_process mesh_process;
      mesh_process.generate_mesh(poly_path, label_img, height_img,
                                 ground_roi, output_path, geo_cam, num_of_buildings);
      vpl_unlink(poly_path.c_str());
      i_orig += x_dim - x_overlap;
    }
    j_orig += y_dim - y_overlap;
  }

  return true;
}
