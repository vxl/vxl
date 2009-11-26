// This is brl/bseg/bvxm/pro/processes/bvxm_expected_edge_image_process.cxx
#include "bvxm_expected_edge_image_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvxm/bvxm_edge_util.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>

#include <brip/brip_vil_float_ops.h>

#include <vpgl/algo/vpgl_backproject.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <bvxm/bvxm_edge_ray_processor.h>

#include <vcl_cstdio.h>

//: set input and output types
bool bvxm_expected_edge_image_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_expected_edge_image_process_globals;

  // process takes 6 inputs:
  //input[0]: The voxel world
  //input[1]: The current camera
  //input[2]: Expected image ni()
  //input[3]: Expected image nj()
  //input[4]: n_normal
  //input[5]: Scale of the image

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "unsigned";
  input_types_[3] = "unsigned";
  input_types_[4] = "float";
  input_types_[5] = "unsigned";
  if (!pro.set_input_types(input_types_))
    return false;

  // process has 1 outputs:
  // output[0]: Expected edge image
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  return pro.set_output_types(output_types_);
}

//:  optimizes rpc camera parameters based on edges
bool bvxm_expected_edge_image_process(bprb_func_process& pro)
{
  using namespace bvxm_expected_edge_image_process_globals;

  //check number of inputs
  if ( !pro.verify_inputs()) {
    vcl_cout << pro.name() << " - invalid inputs " << vcl_endl;
    return false;
  }

  // get the inputs
  // voxel world
  bvxm_voxel_world_sptr vox_world = pro.get_input<bvxm_voxel_world_sptr>(0);
  // camera
  vpgl_camera_double_sptr camera_inp = pro.get_input<vpgl_camera_double_sptr>(1);
  bvxm_edge_ray_processor edge_proc(vox_world);

  unsigned ni = pro.get_input<unsigned>(2);
  unsigned nj = pro.get_input<unsigned>(3);

  // n_normal
  float n_normal = pro.get_input<float>(4);
  // scale of image
  unsigned scale = pro.get_input<unsigned>(5);

  int num_observations = vox_world->num_observations<EDGES>(0,scale);
  vcl_cout << "Number of observations in curren edge world: " << num_observations << '\n';

  // render the expected edge image
  vil_image_view_base_sptr dummy_img;
  bvxm_image_metadata camera_metadata_inp(dummy_img,camera_inp);
  vil_image_view<float>* fimg = new vil_image_view<float>(ni,nj,1);
  fimg->fill(0.0f);
  vil_image_view_base_sptr expected_edge_image_sptr = fimg;

  edge_proc.expected_edge_image(camera_metadata_inp,
                                expected_edge_image_sptr,
                                n_normal,scale);

  for (unsigned i=0; i<ni; i++) {
    for (unsigned j=0; j<nj; j++) {
      (*fimg)(i,j) = static_cast<unsigned char>(255.0*((*fimg)(i,j)));
    }
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, fimg);

  return true;
}
