#include "bvxm_rpc_registration_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>

#include <bil/algo/bil_cedt.h>
#include <vnl/algo/vnl_gaussian_kernel_1d.h>
#include <vpgl/vpgl_local_rational_camera.h>

#include <brip/brip_vil_float_ops.h>

bvxm_rpc_registration_process::bvxm_rpc_registration_process()
{
  // process takes 4 inputs: 
  //input[0]: The voxel world
  //input[1]: The current camera
  //input[2]: The current edge image
  //input[3]: The flag indicating whether to correct offsets of input image (offsets are corrected if true)
  input_data_.resize(4,brdb_value_sptr(0));
  input_types_.resize(4);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "vil_image_view_base_sptr";
  input_types_[3] = "bool";

  // process has 2 outputs:
  // output[0]: The optimized camera
  // output[1]: Expected Voxel Image
  output_data_.resize(2,brdb_value_sptr(0));
  output_types_.resize(2);
  output_types_[0] = "vpgl_camera_double_sptr";
  output_types_[1] = "vil_image_view_base_sptr";

  // adding parameters
  parameters()->add("parameter specifying whether to use corrected images in the update part", "use_online_algorithm", false);
  parameters()->add("gaussian sigma for the edge distance map", "cedt_image_gaussian_sigma", 2.0);
  parameters()->add("maximum expected error in the rpc image offset", "offset_search_size", 20);
}

bool bvxm_rpc_registration_process::execute()
{
  // Sanity check
  if(!this->verify_inputs())
    return false;

  brdb_value_t<bvxm_voxel_world_sptr>* input0 = static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[0].ptr());
  brdb_value_t<vpgl_camera_double_sptr>* input1 =  static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[1].ptr());
  brdb_value_t<vil_image_view_base_sptr>* input2 = static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[2].ptr());
  brdb_value_t<bool>* input3 = static_cast<brdb_value_t<bool>* >(input_data_[3].ptr());

  bvxm_voxel_world_sptr vox_world = input0->value();

  vpgl_camera_double_sptr camera_inp = input1->value();
  vpgl_local_rational_camera<double> *cam_inp;
  if (!(cam_inp = dynamic_cast<vpgl_local_rational_camera<double>*>(camera_inp.ptr()))) {
    vcl_cerr << "error: process expects camera to be a vpgl_local_rational_camera." << vcl_endl;
    return false;
  }

  vil_image_view_base_sptr edge_image_sptr = input2->value();
  bool rpc_correction_flag = input3->value();

  vil_image_view<vxl_byte> edge_image(edge_image_sptr);

  // getting parameters
  double cedt_image_gaussian_sigma;
  int offset_search_size;
  bool use_online_algorithm;
  if (!parameters()->get_value("use_online_algorithm", use_online_algorithm) || 
    !parameters()->get_value("cedt_image_gaussian_sigma", cedt_image_gaussian_sigma) || 
    !parameters()->get_value("offset_search_size", offset_search_size)
    ){
      vcl_cout << "problems in retrieving parameters\n";
      return false;
  }

  int ni = edge_image.ni();
  int nj = edge_image.nj();

  int max_u = 0, max_v = 0;

  vil_image_view<vxl_byte> expected_edge_image_output;
  expected_edge_image_output.set_size(ni,nj);
  expected_edge_image_output.fill(0);

  // part 1: correction
  if(rpc_correction_flag){

    //create image metadata object (no image with camera, so just use dummy):
    vil_image_view_base_sptr dummy_img;
    bvxm_image_metadata camera_metadata_inp(dummy_img,camera_inp);

    // render the edge image
    vil_image_view_base_sptr expected_edge_image_sptr = new vil_image_view<float>(ni,nj,1);
    bool result = vox_world->expected_edge_image(camera_metadata_inp, expected_edge_image_sptr);
    vil_image_view<float> expected_edge_image(expected_edge_image_sptr);

    for(int i=0; i<ni; i++){
      for(int j=0; j<nj; j++){
        expected_edge_image_output(i,j) = (vxl_byte)(256.0*expected_edge_image(i,j));
      }
    }

    advanced_offset_estimation(offset_search_size,edge_image,expected_edge_image,max_u,max_v);

    vcl_cout << "Estimated changes in offsets (u,v)=(" << max_u << "," << max_v << ")" << vcl_endl;
  }


  vpgl_local_rational_camera<double> cam_out(*cam_inp);
  double offset_u,offset_v;
  cam_out.image_offset(offset_u,offset_v);
  offset_u += (double)max_u;
  offset_v += (double)max_v;
  cam_out.set_image_offset(offset_u,offset_v);

  // part 2: update

  vpgl_camera_double_sptr camera_out = new vpgl_local_rational_camera<double>(cam_out);

  if(use_online_algorithm || !rpc_correction_flag){
    vil_image_view<vxl_byte> edge_image_negated(edge_image);
    vil_math_scale_and_offset_values(edge_image_negated,-1.0,255);

    bil_cedt bil_cedt_operator(edge_image_negated);
    bil_cedt_operator.compute_cedt();
    vil_image_view<float> cedt_image = bil_cedt_operator.cedtimg();

    vnl_gaussian_kernel_1d gaussian(cedt_image_gaussian_sigma);
    for(int i=0; i<ni; i++){
      for(int j=0; j<nj; j++){
        cedt_image(i,j) = (float)gaussian.G((double)cedt_image(i,j));
      }
    }

    vil_image_view_base_sptr cedt_image_sptr = new vil_image_view<float>(cedt_image);
    bvxm_image_metadata camera_metadata_out(cedt_image_sptr,camera_inp);

    vox_world->update_edges(camera_metadata_out);
  }

  // update the camera and store
  brdb_value_sptr output0 = new brdb_value_t<vpgl_camera_double_sptr>(camera_out);
  output_data_[0] = output0;

  brdb_value_sptr output1 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(expected_edge_image_output));
  output_data_[1] = output1;

  return true;
}

void bvxm_rpc_registration_process::advanced_offset_estimation(const int offset_search_size,const vil_image_view<vxl_byte>& edge_image,const vil_image_view<float>& expected_edge_image,int& max_u,int& max_v){
  vcl_cout << "Estimating image offsets:" << vcl_endl;

  //bool brip_vil_float_ops::homography(vil_image_view<float> const& input,
  //  vgl_h_matrix_2d<double> const& H,
  //  vil_image_view<float>& output,
  //  bool output_size_fixed,
  //  float output_fill_value)

  double max_prob = 0.0;
  int ni = edge_image.ni();
  int nj = edge_image.nj();

  //brip_vil_float_ops::convert_to_float(
  //  vil_math_scale_and_offset_values(

  vil_image_view<float> expected_edge_image_translated(ni,nj);

  for(int u=-offset_search_size; u<=offset_search_size; u++){
    vcl_cout << ".";
    for(int v=-offset_search_size; v<=offset_search_size; v++){
      translate_image(expected_edge_image,expected_edge_image_translated,(double)u,(double)v);
      double prob = 0.0;
      for(int m=0; m<ni; m++){
        for(int n=0; n<nj; n++){
          if(edge_image(m,n)==255){
            prob += expected_edge_image_translated(m,n);
          }
        }
      }

      if(prob > max_prob){
        max_prob = prob;
        max_u = u;
        max_v = v;
      }
    }
  }
  vcl_cout << vcl_endl;
}

void bvxm_rpc_registration_process::translate_image(const vil_image_view<float>& inp,vil_image_view<float>& out, double ti,double tj){
  int ti_int = vnl_math_rnd(ti);
  int tj_int = vnl_math_rnd(tj);

  int ti_abs = vnl_math_abs(ti_int);
  int tj_abs = vnl_math_abs(tj_int);

  int ni = inp.ni();
  int nj = inp.nj();

  out.set_size(ni,nj);
  out.fill(0.0);

  bool sub_pixel_mode = true;
  if(vnl_math_abs(ti-(double)ti_int) < 0.049 && vnl_math_abs(tj-(double)tj_int) < 0.049 ){
    sub_pixel_mode = false;
  }

  if(sub_pixel_mode){
  }
  else{
    for(int i=ti_abs; i<ni-ti_abs; i++){
      for(int j=tj_abs; j<nj-tj_abs; j++){
        int ci = i-ti_int;
        int cj = j-tj_int;
        out(i,j) = inp(ci,cj);
      }
    }
  }
}
