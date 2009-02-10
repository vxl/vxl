// This is brl/bseg/bvxm/pro/processes/bvxm_rpc_registration_process.cxx
//:
// \file
// \brief A process that optimizes rpc camera parameters based on edges in images and the voxel world
//
// \author Ibrahim Eden
// \date 03/02/2008
// \verbatim
//  Modifications
//   Brandon Mayer - 1/28/09 - converted process-class to function to conform with new bvxm_process architecture.
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>

#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

#include <brip/brip_vil_float_ops.h>

#include <vpgl/algo/vpgl_backproject.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>

#include <vil/vil_load.h>
#include <vcl_cstdio.h>

//: globals
namespace bvxm_rpc_registration_process_globals
{
  const unsigned n_inputs_ = 9;
  const unsigned n_outputs_ = 4;
  
  //parameter strings
  const vcl_string param_noise_multiplier_ =  "noise_multiplier";
  const vcl_string param_smooth_ =  "smooth";
  const vcl_string param_automatic_threshold_ =  "automatic_threshold";
  const vcl_string param_junctionp_ =  "junctionp";
  const vcl_string param_aggressive_junction_closure_ =  "aggressive_junction_closure";
  const vcl_string param_edt_gaussian_sigma_ =  "edt_gaussian_sigma";
  const vcl_string param_edt_image_mean_scale_ =  "edt_image_mean_scale";
}

//: set input and output types
bool bvxm_rpc_registration_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_rpc_registration_process_globals;

  // process takes 9 inputs:
  //input[0]: The voxel world
  //input[1]: The current camera
  //input[2]: The current image
  //input[3]: The flag indicating whether to correct offsets of input image (offsets are corrected if true)
  //input[4]: The flag indicating whether to update the voxel world with the input image
  //input[5]: The flag indicating whether to align the 3D voxel world along with image
  //input[6]: Uncertainty in meters
  //input[7]: n_normal
  //input[8]: Scale of the image

  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "bvxm_voxel_world_sptr";
  input_types_[i++] = "vpgl_camera_double_sptr";
  input_types_[i++] = "vil_image_view_base_sptr";
  input_types_[i++] = "bool";
  input_types_[i++] = "bool";
  input_types_[i++] = "bool";
  input_types_[i++] = "float";
  input_types_[i++] = "float";
  input_types_[i++] = "unsigned";
  if(!pro.set_input_types(input_types_))
      return false;
  
  // process has 3 outputs:
  // output[0]: The optimized camera
  // output[1]: Edge image
  // output[2]: Expected edge image
  // output[3]: New n_normal

  vcl_vector<vcl_string> output_types_(n_outputs_);
  unsigned j = 0;
  output_types_[j++] = "vpgl_camera_double_sptr";
  output_types_[j++] = "vil_image_view_base_sptr";
  output_types_[j++] = "vil_image_view_base_sptr";
  output_types_[j++] = "float";
  if(!pro.set_output_types(output_types_))
    return false;
  
  return true;
}

//:  optimizes rpc camera parameters based on edges
bool bvxm_rpc_registration_process(bprb_func_process& pro)
{
  using namespace bvxm_rpc_registration_process_globals;

  //check number of inputs
  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false; 
  }

  // get the inputs
  unsigned i = 0;
  // voxel world
  bvxm_voxel_world_sptr vox_world = pro.get_input<bvxm_voxel_world_sptr>(i++);        
  // camera
  vpgl_camera_double_sptr camera_inp = pro.get_input<vpgl_camera_double_sptr>(i++);    
  // image
  vil_image_view_base_sptr image_sptr = pro.get_input<vil_image_view_base_sptr>(i++);  
  vil_image_view<vxl_byte> image(image_sptr);

  // boolean parameter specifying the correction state
  bool rpc_correction_flag = pro.get_input<bool>(i++);                                          
  // boolean parameter specifying the update state
  bool rpc_update_flag = pro.get_input<bool>(i++);
  // boolean parameter specifying the voxel world alignment state
  bool rpc_shift_3d_flag = pro.get_input<bool>(i++);
  // uncertainty in meters
  float uncertainty = pro.get_input<float>(i++);
  // n_normal
  float n_normal = pro.get_input<float>(i++);
  // scale of image
  unsigned scale = pro.get_input<unsigned>(i++);

  // get parameters
  double noise_multiplier=1.5, smooth=1.5, edt_gaussian_sigma=3.0, edt_image_mean_scale=1.0;
  bool automatic_threshold=false, junctionp=false, aggressive_junction_closure=false;
  
  pro.parameters()->get_value(param_noise_multiplier_, noise_multiplier);
  pro.parameters()->get_value(param_smooth_, smooth);
  pro.parameters()->get_value(param_automatic_threshold_, automatic_threshold);
  pro.parameters()->get_value(param_junctionp_, junctionp);
  pro.parameters()->get_value(param_aggressive_junction_closure_, aggressive_junction_closure);
  pro.parameters()->get_value(param_edt_gaussian_sigma_, edt_gaussian_sigma);
  pro.parameters()->get_value(param_edt_image_mean_scale_, edt_image_mean_scale);

  //local variables
  vcl_ifstream file_inp;
  vcl_ofstream file_out;

  int offset_search_size = bvxm_util::convert_uncertainty_from_meters_to_pixels(uncertainty, vox_world->get_params()->lvcs(), camera_inp);

  // enforce the restriction (20<=offset_search_size<=100)
  offset_search_size = vnl_math_max(offset_search_size,20);
  offset_search_size = vnl_math_min(offset_search_size,100);

  vcl_cout << "Offset search size is: " << offset_search_size << "\n";

  int num_observations = vox_world->num_observations<EDGES>(0,scale);
  vcl_cout << "Number of observations before the update: " << num_observations << "\n";

  int ni = image.ni();
  int nj = image.nj();

  double best_offset_u = 0.0, best_offset_v = 0.0;

  vil_image_view<vxl_byte> edge_image_output(ni,nj,1);
  edge_image_output.fill(0);

  // render the expected edge image
  vil_image_view<vxl_byte> expected_edge_image_output(ni,nj,1);
  expected_edge_image_output.fill(0);
  vil_image_view_base_sptr dummy_img;
  bvxm_image_metadata camera_metadata_inp(dummy_img,camera_inp);
  vil_image_view_base_sptr expected_edge_image_sptr = new vil_image_view<float>(ni,nj,1);
  vox_world->expected_edge_image(camera_metadata_inp, expected_edge_image_sptr,n_normal,scale);
  vil_image_view<float> expected_edge_image(expected_edge_image_sptr);
  //float eei_min = vcl_numeric_limits<float>::max();
  //float eei_max = vcl_numeric_limits<float>::min();
  //// setting the output edge image for viewing purposes
  //for (int i=0; i<ni; i++) {
  //  for (int j=0; j<nj; j++) {
  //    eei_min = vnl_math_min(eei_min,expected_edge_image(i,j));
  //    eei_max = vnl_math_max(eei_max,expected_edge_image(i,j));
  //  }
  //}
  for (int i=0; i<ni; i++) {
    for (int j=0; j<nj; j++) {
//      expected_edge_image_output(i,j) = (int)((255.0*(expected_edge_image(i,j)-eei_min))/(eei_max-eei_min));
      expected_edge_image_output(i,j) = (int)(255.0*expected_edge_image(i,j));
    }
  }

  vil_image_view<vxl_byte> edge_image = bvxm_util::detect_edges(image,noise_multiplier,smooth,automatic_threshold,junctionp,aggressive_junction_closure);
  edge_image_output.deep_copy(edge_image);
 
  // part 1: correction
  // this part contains the correction rpc camera parameters using the expected edge image obtained
  // from the voxel model and edge map of the current image.
  // if the camera parameters are manually corrected by the user, this part should be omitted by setting the
  // "rpc_correction_flag" parameter to 0 (false).
  if (rpc_correction_flag && num_observations > 0)
  {
    double max_prob = vcl_numeric_limits<double>::min();

    // this is the two level offset search algorithm
    int offset_lower_limit_u = -offset_search_size;
    int offset_lower_limit_v = -offset_search_size;
    int offset_upper_limit_u =  offset_search_size;
    int offset_upper_limit_v =  offset_search_size;
    vcl_cout << "Estimating image offsets:" << vcl_endl;
    for (int u=offset_lower_limit_u; u<=offset_upper_limit_u; u++) {
      vcl_cout << '.';
      for (int v=offset_lower_limit_v; v<=offset_upper_limit_v; v++) {
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
          best_offset_u = (double)u;
          best_offset_v = (double)v;
        }
      }
    }
    vcl_cout << vcl_endl;
  }

  vcl_cout << "Estimated changes in offsets (u,v)=(" << best_offset_u << ',' << best_offset_v << ')' << vcl_endl;

  float nlx=0.f,nly=0.f,nlz=0.f;
  if (rpc_shift_3d_flag)
  {
    vpgl_local_rational_camera<double> *cam_input_temp = dynamic_cast<vpgl_local_rational_camera<double>*>(camera_inp.ptr());

    vgl_point_3d<double> origin_3d(0.0,0.0,0.0);
    vgl_point_2d<double> origin_2d = cam_input_temp->project(origin_3d);

    vgl_plane_3d<double> curr_plane(0.0,0.0,1.0,-0.001);
    vgl_point_3d<double> init_point(0.0,0.0,0.001);
    vgl_point_3d<double> world_point;
    vpgl_backproject::bproj_plane(cam_input_temp,origin_2d,curr_plane,init_point,world_point);

    vgl_vector_3d<double> motion_plane_normal = world_point - origin_3d;
    motion_plane_normal = normalize(motion_plane_normal);
    vgl_plane_3d<double> motion_plane(motion_plane_normal,origin_3d);

    init_point.set(origin_3d.x(),origin_3d.y(),origin_3d.z());
    vgl_point_3d<double> moved_origin;
    vgl_point_2d<double> origin_2d_shift(origin_2d.x()+best_offset_u,origin_2d.y()+best_offset_v);
    vpgl_backproject::bproj_plane(cam_input_temp,origin_2d_shift,motion_plane,init_point,moved_origin);

    vgl_vector_3d<double> motion = moved_origin - origin_3d;

    double motion_mult = 1.0/((double)(num_observations+1));
    motion = motion*motion_mult;

    float lx,ly,lz;
    vgl_point_3d<float> rpc_origin = vox_world->get_params()->rpc_origin();
    lx = rpc_origin.x();
    ly = rpc_origin.y();
    lz = rpc_origin.z();

    lx = lx*(1.0f-(float)motion_mult);
    ly = ly*(1.0f-(float)motion_mult);
    lz = lz*(1.0f-(float)motion_mult);

    vgl_point_3d<double> pt_3d_est_vec(lx,ly,lz);
    vgl_point_3d<double> pt_3d_updated = pt_3d_est_vec + motion;

    nlx = (float)pt_3d_updated.x();
    nly = (float)pt_3d_updated.y();
    nlz = (float)pt_3d_updated.z();
  }

  // correct the output for (local) rational camera using the estimated offset pair (max_u,max_v)
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
    offset_u += best_offset_u;
    offset_v += best_offset_v;
    cam_out_local.set_image_offset(offset_u,offset_v);
    camera_out = new vpgl_local_rational_camera<double>(cam_out_local);
  }
  else if (is_rational_cam) {
    vpgl_rational_camera<double> cam_out_rational(*cam_inp_rational);
    double offset_u,offset_v;
    cam_out_rational.image_offset(offset_u,offset_v);
    offset_u += best_offset_u;
    offset_v += best_offset_v;
    cam_out_rational.set_image_offset(offset_u,offset_v);
    camera_out = new vpgl_rational_camera<double>(cam_out_rational);
  }
  else {
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
  float new_n_normal = n_normal;
  if (rpc_update_flag) {
    vil_image_view<float> edge_image_float;
    vil_convert_cast(edge_image,edge_image_float);
    vil_math_scale_and_offset_values(edge_image_float,1.0/255.0,0.0);
    float edge_image_mean;
    vil_math_mean(edge_image_mean,edge_image_float,0);

    vil_image_view<float> edt_image;
    bvxm_util::edge_distance_transform(edge_image,edt_image);
    edt_image = bvxm_util::multiply_image_with_gaussian_kernel(edt_image,edt_gaussian_sigma);
    float edt_image_mean;
    vil_math_mean(edt_image_mean,edt_image,0);

    vil_math_scale_and_offset_values(edt_image,edge_image_mean/edt_image_mean,0.0f);
    vil_math_mean(edt_image_mean,edt_image,0);

    new_n_normal = n_normal + (((float)edt_image_mean_scale)*edt_image_mean);

    vil_image_view_base_sptr edt_image_sptr = new vil_image_view<float>(edt_image);

    bvxm_image_metadata camera_metadata_out(edt_image_sptr,camera_out);
    bool result = vox_world->update_edges(camera_metadata_out,0);

    if (!result){
      vcl_cerr << "error bvxm_rpc_registration: failed to update edge image\n";
      return false;
    }
  }

  if (rpc_shift_3d_flag) {
    vgl_point_3d<float> new_rpc_origin(nlx,nly,nlz);
    vox_world->get_params()->set_rpc_origin(new_rpc_origin);
  }

  // output
  unsigned j = 0;
  // update the camera and store
  pro.set_output_val<vpgl_camera_double_sptr>(j++, camera_out);
  // update the edge image and store
  pro.set_output_val<vil_image_view_base_sptr>(j++, new vil_image_view<vxl_byte>(edge_image_output));
  // update the edge image and store
  pro.set_output_val<vil_image_view_base_sptr>(j++, new vil_image_view<vxl_byte>(expected_edge_image_output));
  // update the edge image and store
  pro.set_output_val<float >(j++, new_n_normal);

  return true;
}
