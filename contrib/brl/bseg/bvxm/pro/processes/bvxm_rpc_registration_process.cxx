// This is brl/bseg/bvxm/pro/processes/bvxm_rpc_registration_process.cxx
//:
// \file
// \brief A process that optimizes rpc camera parameters based on edges in images and the voxel world
//
// \author Ibrahim Eden
// \date 03/02/2008
// \verbatim
//  Modifications
//   Brandon Mayer - 1/28/09 - converted process-class to function to conform with bvxm_process architecture.
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <brdb/brdb_value.h>

#include <bprb/bprb_parameters.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>

#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

//Define parameters here
#define PARAM_NOISE_MULTIPLIER "noise_multiplier"
#define PARAM_SMOOTH "smooth"
#define PARAM_AUTO_THRESH "automatic_threshold"
#define PARAM_JUNCTIONP "junctionp"
#define PARAM_AGRESSIVE_JUNCTION_CLOSURE "aggressive_junction_closure"
#define PARAM_EDT_GAUSS_SIG "edt_gaussian_sigma"

namespace bvxm_rpc_registration_process_globals
{
  unsigned const n_inputs_ = 6;
  unsigned const n_outputs_ = 3;
}

bool bvxm_rpc_registration_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_rpc_registration_process_globals;
  // process takes 6 inputs:
  //input[0]: The voxel world
  //input[1]: The current camera
  //input[2]: The current edge image
  //input[3]: The flag indicating whether to correct offsets of input image (offsets are corrected if true)
  //input[4]: Uncertainty in meters
  //input[5]: Scale of the image

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "vil_image_view_base_sptr";
  input_types_[3] = "bool";
  input_types_[4] = "float";
  input_types_[5] = "unsigned";
  bool ok = pro.set_input_types(input_types_);
  if (!ok) return false;

  // process has 2 outputs:
  // output[0]: The optimized camera
  // output[1]: Edge image
  // output[2]: Expected voxel image
  vcl_vector<vcl_string> output_types_(n_outputs_);
  unsigned j = 0;
  output_types_[j++] = "vpgl_camera_double_sptr";  //optimized camera
  output_types_[j++] = "vil_image_view_base_sptr"; //edge image
  output_types_[j++] = "vil_image_view_base_sptr"; //expected voxel image
  ok = pro.set_output_types(output_types_);
  return ok;
}

bool bvxm_rpc_registration_process(bprb_func_process& pro)
{

  using namespace bvxm_rpc_registration_process_globals;
  
  //check number of inputs
  if ( pro.n_inputs() < n_inputs_){
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  bvxm_voxel_world_sptr vox_world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  vpgl_camera_double_sptr camera_inp = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr image_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  bool rpc_correction_flag = pro.get_input<bool>(i++);
  float uncertainty = pro.get_input<float>(i++);
  unsigned scale = pro.get_input<unsigned>(i++);

  //check input validity
  if ( !vox_world ) {
    vcl_cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }

  if ( !camera_inp ){
    vcl_cout << pro.name() <<" :--  Input 1  is not valid!\n";
    return false;
  }

  if ( !image_sptr ){
    vcl_cout << pro.name() <<" :--  Input 2  is not valid!\n";
    return false;
  }


  // get parameters
    double noise_multiplier=1.5f, smooth=1.5f, edt_gaussian_sigma=2.0f; 
  bool automatic_threshold=false, junctionp=false, aggressive_junction_closure=false;
  if (!pro.parameters()->get_value( PARAM_NOISE_MULTIPLIER, noise_multiplier )||
      !pro.parameters()->get_value( PARAM_SMOOTH, smooth )||
      !pro.parameters()->get_value( PARAM_AUTO_THRESH, automatic_threshold ) ||
      !pro.parameters()->get_value( PARAM_JUNCTIONP, junctionp ) ||
      !pro.parameters()->get_value( PARAM_AGRESSIVE_JUNCTION_CLOSURE, aggressive_junction_closure ) ||
      !pro.parameters()->get_value( PARAM_EDT_GAUSS_SIG, edt_gaussian_sigma ) ){
    vcl_cout << "problem(s) in parameters: please see brl/bseg/bvxm_batch/multiscale/rpc_registration_parameters.xml for correct xml formatting\n";
    return false;
  }

  int offset_search_size = bvxm_util::convert_uncertainty_from_meters_to_pixels(uncertainty, vox_world->get_params()->lvcs(), camera_inp);
  vcl_cout << "Offset search size is: " << offset_search_size << vcl_endl;

  vil_image_view<vxl_byte> image(image_sptr);
  int ni = image.ni();
  int nj = image.nj();

  double max_prob = 0.0;
  int max_u = 0, max_v = 0;

  vil_image_view<vxl_byte> edge_image_output(ni,nj,1);
  vil_image_view<vxl_byte> expected_edge_image_output(ni,nj,1);
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
    vox_world->expected_edge_image(camera_metadata_inp, expected_edge_image_sptr, scale);
    vil_image_view<float> expected_edge_image(expected_edge_image_sptr);

    float eei_min = vcl_numeric_limits<float>::max();
    float eei_max = vcl_numeric_limits<float>::min();

    // setting the output edge image for viewing purposes
    for (int i=0; i<ni; i++) {
      for (int j=0; j<nj; j++) {
        eei_min = vnl_math_min(eei_min,expected_edge_image(i,j));
        eei_max = vnl_math_max(eei_max,expected_edge_image(i,j));
      }
    }
    float division_factor = vnl_math_max((eei_max-eei_min),0.000001f);  // to prevent division by 0
    for (int i=0; i<ni; i++) {
      for (int j=0; j<nj; j++) {
        expected_edge_image_output(i,j) = (int)(255.0*(expected_edge_image(i,j)-eei_min)/division_factor);
      }
    }

    vil_image_view<vxl_byte> edge_image = bvxm_util::detect_edges(image,noise_multiplier,smooth,automatic_threshold,junctionp,aggressive_junction_closure);
    edge_image_output.deep_copy(edge_image);

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
              if (edge_image_output(m,n)==255) {
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

    vcl_cout << "\nEstimated changes in offsets (u,v)=(" << max_u << ',' << max_v << ')' << vcl_endl;
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

  if (!rpc_correction_flag) {

    unsigned max_scale=vox_world->get_params()->max_scale();
    for (unsigned curr_scale = scale;curr_scale < max_scale;curr_scale++)
    {
      bool result;

      vil_image_view<vxl_byte> curr_image;
      vpgl_camera_double_sptr curr_camera;
      if (curr_scale!=scale){
        image_sptr = bvxm_util::downsample_image_by_two(image_sptr);
        vil_image_view<vxl_byte> temp_image(image_sptr);
        curr_image.deep_copy(temp_image);
        curr_camera = bvxm_util::downsample_camera(camera_out,curr_scale);
      }
      else{
        curr_image.deep_copy(image);
        curr_camera = camera_out;
      }
      vil_image_view<vxl_byte> edge_image = bvxm_util::detect_edges(curr_image,noise_multiplier,smooth,automatic_threshold,junctionp,aggressive_junction_closure);
      if (curr_scale==scale){
        edge_image_output.deep_copy(edge_image);
      }
      vil_image_view<float> edt_image;
      bvxm_util::edge_distance_transform(edge_image,edt_image);
      edt_image = bvxm_util::multiply_image_with_gaussian_kernel(edt_image,edt_gaussian_sigma);
      vil_image_view_base_sptr edt_image_sptr = new vil_image_view<float>(edt_image);

      bvxm_image_metadata camera_metadata_out(edt_image_sptr,curr_camera);
      result=vox_world->update_edges(camera_metadata_out,curr_scale);

      if (!result){
        vcl_cerr << "error bvxm_rpc_registration: failed to update edgeimage\n";
        return false;
      }
    }
  }

  vox_world->increment_observations<EDGES>(0,scale);

  //output
  unsigned j = 0;
  // update the camera and store
  pro.set_output_val<vpgl_camera_double_sptr>(j++, camera_out);
  // update the edge image and store
  pro.set_output_val<vil_image_view_base_sptr>(j++, new vil_image_view<vxl_byte>(edge_image_output));
  // update the expected edge image and store
  pro.set_output_val<vil_image_view_base_sptr>(j++, new vil_image_view<vxl_byte>(expected_edge_image_output));

  return true;
}
