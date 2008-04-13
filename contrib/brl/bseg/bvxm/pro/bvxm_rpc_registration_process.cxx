#include "bvxm_rpc_registration_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>

#include <bil/algo/bil_cedt.h>
#include <vnl/algo/vnl_gaussian_kernel_1d.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

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
  if (!this->verify_inputs())
    return false;

  // get the inputs
  // voxel world
  brdb_value_t<bvxm_voxel_world_sptr>* input0 = static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[0].ptr());
  // camera
  brdb_value_t<vpgl_camera_double_sptr>* input1 =  static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[1].ptr());
  // edge image
  brdb_value_t<vil_image_view_base_sptr>* input2 = static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[2].ptr());
  // boolean parameter specifyi
  brdb_value_t<bool>* input3 = static_cast<brdb_value_t<bool>* >(input_data_[3].ptr());

  bvxm_voxel_world_sptr vox_world = input0->value();

  vpgl_camera_double_sptr camera_inp = input1->value();

  vil_image_view_base_sptr edge_image_sptr = input2->value();
  bool rpc_correction_flag = input3->value();
  vil_image_view<vxl_byte> edge_image(edge_image_sptr);

  // get parameters
  double cedt_image_gaussian_sigma=0.0;
  int offset_search_size=0;
  bool use_online_algorithm=false;
  if (!parameters()->get_value("use_online_algorithm", use_online_algorithm) ||
      !parameters()->get_value("cedt_image_gaussian_sigma", cedt_image_gaussian_sigma) ||
      !parameters()->get_value("offset_search_size", offset_search_size)) {
    vcl_cout << "problems in retrieving parameters\n";
    return false;
  }

  int ni = edge_image.ni();
  int nj = edge_image.nj();

  double max_prob = 0.0;
  int max_u = 0, max_v = 0;

  vil_image_view<vxl_byte> expected_edge_image_output;
  expected_edge_image_output.set_size(ni,nj);
  expected_edge_image_output.fill(0);

  // part 1: correction
  // this part contains the correction rpc camera parameters using the expected edge image obtained
  // from the voxel model and edge map of the current image.
  // if the camera parameters are manually corrected by the user, this part should be omitted by setting the
  // "rpc_correction_flag" parameter to 0 (false).
  if (rpc_correction_flag)
  {
    //create image metadata object (no image with camera, so just use dummy):
    vil_image_view_base_sptr dummy_img;
    bvxm_image_metadata camera_metadata_inp(dummy_img,camera_inp);

    // render the edge image
    vil_image_view_base_sptr expected_edge_image_sptr = new vil_image_view<float>(ni,nj,1);
    vox_world->expected_edge_image(camera_metadata_inp, expected_edge_image_sptr);
    vil_image_view<float> expected_edge_image(expected_edge_image_sptr);

    // setting the output edge image for viewing purposes
    for (int i=0; i<ni; i++) {
      for (int j=0; j<nj; j++) {
        expected_edge_image_output(i,j) = (vxl_byte)(256.0*expected_edge_image(i,j));
      }
    }

    vcl_cout << "Estimating image offsets:" << vcl_endl;

    // this a two level search algorithm
    // the first level search for the optimal offset parameters in a large scale depending on the input offset search size
    // the second level search for the optimal offset parameters in a small scale
    int offset_step_size = vnl_math_min(((offset_search_size-1)/20)+1,3);
    int offset_lower_limit_u = -offset_search_size+offset_step_size;
    int offset_lower_limit_v = offset_lower_limit_u;
    int offset_upper_limit_u = offset_search_size-offset_step_size;
    int offset_upper_limit_v = offset_upper_limit_u;

    for (int level=1; level<=2; level++)
    {
      for (int u=offset_lower_limit_u; u<=offset_upper_limit_u; u=u+offset_step_size) {
        vcl_cout << '.';
        for (int v=offset_lower_limit_v; v<=offset_upper_limit_v; v=v+offset_step_size) {
          // for each offset pair (u,v)
          double prob = 0.0;
          // find the total probability of the edge image given the expected edge image
          for (int m=offset_search_size; m<ni-offset_search_size; m++) {
            for (int n=offset_search_size; n<nj-offset_search_size; n++) {
              if (edge_image(m,n)==255) {
                prob += expected_edge_image(m-u,n-v);
              }
            }
          }
          // if maximum is found
          if (prob > max_prob) {
            max_prob = prob;
            max_u = u;
            max_v = v;
          }
        }
      }
      offset_lower_limit_u = max_u-offset_step_size;
      offset_lower_limit_v = max_v-offset_step_size;
      offset_upper_limit_u = max_u+offset_step_size;;
      offset_upper_limit_v = max_v+offset_step_size;;
      offset_step_size = 1;
    }
    vcl_cout << vcl_endl;

    vcl_cout << "Estimated changes in offsets (u,v)=(" << max_u << ',' << max_v << ')' << vcl_endl;
  }

  // correct the output for (local) rational camera using the estimated offset pair (max_u,max_v)
  // note that is the correction part is skipped, (max_u,max_v)=(0,0)
  // the following block of code takes care of different input camera types
  //  e.g., vpgl_local_rational_camera and vpgl_rational_camera
  bool is_local_cam = true;
  bool is_rational_cam = true;
  vpgl_local_rational_camera<double> *cam_inp_local;
  if (!(cam_inp_local = dynamic_cast<vpgl_local_rational_camera<double>*>(camera_inp.ptr()))) {
    is_local_cam = false;
  }
  vpgl_rational_camera<double> *cam_inp_rational;
  if (!(cam_inp_rational = dynamic_cast<vpgl_rational_camera<double>*>(camera_inp.ptr()))) {
    is_rational_cam = false;
  }
  vpgl_camera_double_sptr camera_out;
  if (is_local_cam) {
    vpgl_local_rational_camera<double> cam_out_local(*cam_inp_local);
    double offset_u,offset_v;
    cam_out_local.image_offset(offset_u,offset_v);
    offset_u += (double)max_u;
    offset_v += (double)max_v;
    cam_out_local.set_image_offset(offset_u,offset_v);
    camera_out = new vpgl_local_rational_camera<double>(cam_out_local);
  }
  else if (is_rational_cam) {
    vpgl_rational_camera<double> cam_out_rational(*cam_inp_rational);
    double offset_u,offset_v;
    cam_out_rational.image_offset(offset_u,offset_v);
    offset_u += (double)max_u;
    offset_v += (double)max_v;
    cam_out_rational.set_image_offset(offset_u,offset_v);
    camera_out = new vpgl_rational_camera<double>(cam_out_rational);
  }
  else{
    vcl_cerr << "error: process expects camera to be a vpgl_rational_camera or vpgl_local_rational_camera.\n";
    return false;
  }

  // part 2: update
  // this part contains the correction rpc camera parameters using the expected edge image obtained
  // from the voxel model and edge map of the current image.
  // if the camera parameters are manually corrected by the user, this part should be omitted by setting the
  // "rpc_correction_flag" parameter to 0 (false).

  // update part work if the input camera parameters are not correct or the online algorithm flag
  // "use_online_algorithm" is set to 1 (true) in the input parameter file
  if (use_online_algorithm || !rpc_correction_flag) {
    vil_image_view<vxl_byte> edge_image_negated(edge_image);
    vil_math_scale_and_offset_values(edge_image_negated,-1.0,255);

    // generates the edge distance transform
    bil_cedt bil_cedt_operator(edge_image_negated);
    bil_cedt_operator.compute_cedt();
    vil_image_view<float> cedt_image = bil_cedt_operator.cedtimg();

    // multiplies the edge distance transform with a gaussian kernel
    vnl_gaussian_kernel_1d gaussian(cedt_image_gaussian_sigma);
    for (int i=0; i<ni; i++) {
      for (int j=0; j<nj; j++) {
        cedt_image(i,j) = (float)gaussian.G((double)cedt_image(i,j));
      }
    }

    // updates the edge probabilities in the voxel world
    vil_image_view_base_sptr cedt_image_sptr = new vil_image_view<float>(cedt_image);
    bvxm_image_metadata camera_metadata_out(cedt_image_sptr,camera_inp);
    vox_world->update_edges(camera_metadata_out);
  }

  // update the camera and store
  brdb_value_sptr output0 = new brdb_value_t<vpgl_camera_double_sptr>(camera_out);
  output_data_[0] = output0;

  // update the expected edge image and store
  brdb_value_sptr output1 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(expected_edge_image_output));
  output_data_[1] = output1;

  return true;
}
