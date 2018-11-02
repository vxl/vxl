// This is brl/bseg/bvxm/pro/processes/bvxm_rpc_registration_process.cxx
#include <iostream>
#include <cstdio>
#include "bvxm_rpc_registration_process.h"
//:
// \file
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
#include <bvxm/bvxm_edge_ray_processor.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: set input and output types
bool bvxm_rpc_registration_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_rpc_registration_process_globals;

  // process takes 6 inputs:
  //input[0]: The voxel world
  //input[1]: The current camera
  //input[2]: The current image
  //input[3]: The flag indicating whether to align the 3D voxel world along with image
  //input[4]: Uncertainty in meters
  //input[5]: Scale of the image

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "bvxm_voxel_world_sptr";
  input_types_[i++] = "vpgl_camera_double_sptr";
  input_types_[i++] = "vil_image_view_base_sptr";
  input_types_[i++] = "bool";
  input_types_[i++] = "float";
  input_types_[i++] = "unsigned";
  if (!pro.set_input_types(input_types_))
    return false;

  // process has 2 outputs:
  // output[0]: The optimized camera
  // output[1]: Expected edge image

  std::vector<std::string> output_types_(n_outputs_);
  unsigned j = 0;
  output_types_[j++] = "vpgl_camera_double_sptr";
  output_types_[j++] = "vil_image_view_base_sptr";
  output_types_[j++] = "double";
  output_types_[j++] = "double";
  return pro.set_output_types(output_types_);
}

//:  optimizes rpc camera parameters based on edges
bool bvxm_rpc_registration_process(bprb_func_process& pro)
{
  using namespace bvxm_rpc_registration_process_globals;

  //check number of inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  // voxel world
  bvxm_voxel_world_sptr vox_world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  // camera
  vpgl_camera_double_sptr camera_inp = pro.get_input<vpgl_camera_double_sptr>(i++);
  // image
  vil_image_view_base_sptr edge_image_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<vxl_byte> edge_image(edge_image_sptr);
  bvxm_edge_ray_processor edge_proc(vox_world);
  // boolean parameter specifying the voxel world alignment state
  bool rpc_shift_3d_flag = pro.get_input<bool>(i++);
  // uncertainty in meters
  auto uncertainty = pro.get_input<float>(i++);
  // scale of image
  auto scale = pro.get_input<unsigned>(i++);

  float n_normal = vox_world->get_params()->edges_n_normal();

  //local variables
  std::ifstream file_inp;
  std::ofstream file_out;

  int offset_search_size = bvxm_util::convert_uncertainty_from_meters_to_pixels(uncertainty, vox_world->get_params()->lvcs(), camera_inp);

  std::cout << "Offset search size is: " << offset_search_size << " given uncertainty value " << uncertainty <<'\n';

  int num_observations = vox_world->num_observations<EDGES>(0,scale);
  std::cout << "Number of observations before the update: " << num_observations << '\n';

  int ni = edge_image.ni();
  int nj = edge_image.nj();

  double best_offset_u = 0.0, best_offset_v = 0.0;

  // render the expected edge image
  vil_image_view<vxl_byte> expected_edge_image_output(ni,nj,1);
  expected_edge_image_output.fill(0);
  vil_image_view_base_sptr dummy_img;
  bvxm_image_metadata camera_metadata_inp(dummy_img,camera_inp);
  vil_image_view_base_sptr expected_edge_image_sptr = new vil_image_view<float>(ni,nj,1);
  edge_proc.expected_edge_image(camera_metadata_inp, expected_edge_image_sptr,n_normal,scale);
  vil_image_view<float> expected_edge_image(expected_edge_image_sptr);
  //float eei_min = std::numeric_limits<float>::max();
  //float eei_max = std::numeric_limits<float>::min();
  //// setting the output edge image for viewing purposes
  //for (int i=0; i<ni; i++) {
  //  for (int j=0; j<nj; j++) {
  //    eei_min = std::min(eei_min,expected_edge_image(i,j));
  //    eei_max = std::max(eei_max,expected_edge_image(i,j));
  //  }
  //}
  for (int i=0; i<ni; i++) {
    for (int j=0; j<nj; j++) {
      //      expected_edge_image_output(i,j) = (int)((255.0*(expected_edge_image(i,j)-eei_min))/(eei_max-eei_min));
      expected_edge_image_output(i,j) = static_cast<unsigned char>(255.0*expected_edge_image(i,j));
    }
  }

  // this part contains the correction rpc camera parameters using the expected edge image obtained
  // from the voxel model and edge map of the current image.
  // if the camera parameters are manually corrected by the user, this part should be omitted by setting the
  // "rpc_correction_flag" parameter to 0 (false).
  double max_prob = std::numeric_limits<double>::min();

  // this is the two level offset search algorithm
  int offset_lower_limit_u = -offset_search_size;
  int offset_lower_limit_v = -offset_search_size;
  int offset_upper_limit_u =  offset_search_size;
  int offset_upper_limit_v =  offset_search_size;
  std::cout << "Estimating image offsets:" << std::endl;
  for (int u=offset_lower_limit_u; u<=offset_upper_limit_u; u++) {
    std::cout << '.';
    for (int v=offset_lower_limit_v; v<=offset_upper_limit_v; v++) {
      // for each offset pair (u,v)
      double prob = 0.0;
      double norm = 0.0;
      // find the total probability of the edge image given the expected edge image
      for (int m=offset_search_size; m<ni-offset_search_size; m++) {
        for (int n=offset_search_size; n<nj-offset_search_size; n++) {
          if (edge_image(m,n)==255) {
            prob += expected_edge_image(m-u,n-v);
          }
          norm += expected_edge_image(m-u,n-v);
        }
      }

      if (norm>0.0)
        prob/=norm;

      // if maximum is found
      if (prob > max_prob) {
        max_prob = prob;
        best_offset_u = (double)u;
        best_offset_v = (double)v;
      }
    }
  }
  std::cout << std::endl;

  std::cout << "Estimated changes in offsets (u,v)=(" << best_offset_u << ',' << best_offset_v << ')' << std::endl;

#if 0
  file_out.clear();
  file_out.open("offsets.txt",std::ofstream::app);
  file_out << best_offset_u << ' ' << best_offset_v << '\n';
  file_out.close();
#endif

  float nlx=0.f,nly=0.f,nlz=0.f;
  double motion_mult = 0.0;
  if (rpc_shift_3d_flag)
  {
    auto *cam_input_temp = dynamic_cast<vpgl_local_rational_camera<double>*>(camera_inp.ptr());

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

    motion_mult = 1.0/((double)(num_observations+1));
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

    vgl_point_3d<float> new_rpc_origin(nlx,nly,nlz);
    vox_world->get_params()->set_rpc_origin(new_rpc_origin);
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
    offset_u += (best_offset_u*(1.0-motion_mult));
    offset_v += (best_offset_v*(1.0-motion_mult));
    std::cout << "Estimated changes with motion_mult in offsets (u,v)=(" << best_offset_u*(1.0-motion_mult) << ',' << best_offset_v*(1.0-motion_mult) << ')' << std::endl;
    cam_out_local.set_image_offset(offset_u,offset_v);
    camera_out = new vpgl_local_rational_camera<double>(cam_out_local);
  }
  else if (is_rational_cam) {
    vpgl_rational_camera<double> cam_out_rational(*cam_inp_rational);
    double offset_u,offset_v;
    cam_out_rational.image_offset(offset_u,offset_v);
    offset_u += (best_offset_u*(1.0-motion_mult));
    offset_v += (best_offset_v*(1.0-motion_mult));
    cam_out_rational.set_image_offset(offset_u,offset_v);
    camera_out = new vpgl_rational_camera<double>(cam_out_rational);
  }
  else {
    std::cerr << "error: process expects camera to be a vpgl_rational_camera or vpgl_local_rational_camera.\n";
    return false;
  }

  // output
  unsigned j = 0;
  // update the camera and store
  pro.set_output_val<vpgl_camera_double_sptr>(j++, camera_out);
  // update the edge image and store
  pro.set_output_val<vil_image_view_base_sptr>(j++, new vil_image_view<vxl_byte>(expected_edge_image_output));
  pro.set_output_val<double>(j++, best_offset_u);
  pro.set_output_val<double>(j++, best_offset_v);

  return true;
}
