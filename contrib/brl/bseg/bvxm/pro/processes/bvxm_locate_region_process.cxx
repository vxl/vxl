//This is brl/bseg/bvxm/pro/processes/bvxm_locate_region_process.cxx
#include "bvxm_locate_region_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vil/vil_image_view_base.h>
#include <vpgl/vpgl_camera_double_sptr.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>

bool bvxm_locate_region_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_locate_region_process_globals;
  //process takes 4inputs
  //input[0]: The observation image
  //input[1]: The mask image
  //input[2]: The camera of the observation
  //input[3]: The voxel world
  //input[4]: The path for output grid
  //input[5]: The appearance model type, the supported strings are:
  //          -apm_mog_grey
  //          -apm_mog_rgb
  //          -apm_mog_mc_2_3
  //          -apm_mog_mc_3_3
  //          -apm_mog_mc_4_3

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vil_image_view_base_sptr";
  input_types_[2] = "vpgl_camera_double_sptr";
  input_types_[3] = "bvxm_voxel_world_sptr";
  input_types_[4] = "vcl_string";
  input_types_[5] = "vcl_string";

  if (!pro.set_input_types(input_types_))
    return false;

  //output has 0 output

  return true;
}

bool bvxm_locate_region_process(bprb_func_process& pro)
{
  using namespace bvxm_locate_region_process_globals;

  if (pro.n_inputs() < n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  if ( !img ) {
      std::cout << pro.name() << " :-- Input " << (i-1) << " is not valid!" << std::endl;
      return false;
  }
  vil_image_view_base_sptr mask = pro.get_input<vil_image_view_base_sptr>(i++);

  //vil_image_view<float> mask = *(vil_convert_cast(float(),mask_base));

  //vil_save(mask,"./binary_mask");
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  if ( !camera ) {
      std::cout << pro.name() << " :-- Input " << (i-1) << " is not valid!" << std::endl;
      return false;
  }
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  if ( !world ) {
      std::cout << pro.name() << " :-- Input " << (i-1) << " is not valid!" << std::endl;
      return false;
  }
  std::string output_path = pro.get_input<std::string>(i++);
  std::string voxel_type = pro.get_input<std::string>(i++);
  unsigned bin_index = 0;
  unsigned scale = 0;

  //create metadata:
  bvxm_image_metadata observation(img,camera);

  //create output_grid:
  vgl_vector_3d<unsigned int> grid_size = world->get_params()->num_voxels(scale);
  bvxm_voxel_grid_base_sptr grid_out = new bvxm_voxel_grid<float>(output_path, grid_size);


  bool result = true;

  if (voxel_type == "apm_mog_grey")
    result = world->region_probability_density<APM_MOG_GREY>(observation,mask,grid_out, bin_index,scale);
  else
    std::cerr << "Error in: bvxm_locate_region_processor: Unsuppported appearance model\n";

  return result;
}
