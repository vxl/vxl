//This is brl/bseg/bvxm/pro/processes/bvxm_locate_region_process.cxx
//:
// \file
// \brief A class for update process of a voxel world.
//
// \author Isabel Restrepo
// \date 01/30/2008
// \verbatim
//  Modifications
//   Brandon Mayer - 1/28/09 - converted process-class to function to conform with new bvxm_process architecture.
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vil/vil_image_view_base.h>
#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>

namespace bvxm_locate_region_process_globals
{
  const unsigned n_inputs_ = 6;

}


bool bvxm_locate_region_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_locate_region_process_globals;
  //process takes 4inputs
  //input[0]: The observation image
  //input[1]: The mask image
  //input[2]: The camera of the observation
  //input[3]: The voxel world
  //input[4]: The path for output grid
  //input[5]: The apperance model type, the supported strings are:
  //          -apm_mog_grey
  //          -apm_mog_rgb
  //          -apm_mog_mc_2_3
  //          -apm_mog_mc_3_3
  //          -apm_mog_mc_4_3

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vil_image_view_base_sptr";
  input_types_[2] = "vpgl_camera_double_sptr";
  input_types_[3] = "bvxm_voxel_world_sptr";
  input_types_[4] = "vcl_string";
  input_types_[5] = "vcl_string";

  if(!pro.set_input_types(input_types_))
    return false;

  //output has 0 output

  
  return true;


}

bool bvxm_locate_region_process(bprb_func_process& pro)
{

  using namespace bvxm_locate_region_process_globals;
  
  if (pro.n_inputs() < n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr mask = pro.get_input<vil_image_view_base_sptr>(i++);

  //vil_image_view<float> mask = *(vil_convert_cast(float(),mask_base));

  //vil_save(mask,"./binary_mask");
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  vcl_string output_path = pro.get_input<vcl_string>(i++);
  vcl_string voxel_type = pro.get_input<vcl_string>(i++);
  unsigned bin_index = 0;
  unsigned scale = 0;

  //check input's validity
  i = 0;
  if ( !img ){
      vcl_cout << pro.name() << " :-- Input " << i++ << " is not valid!\n";
      return false;
  }

  if ( !camera ){
      vcl_cout << pro.name() << " :-- Input " << i++ << " is not valid!\n";
      return false;
  }

  if ( !world ){
      vcl_cout << pro.name() << " :-- Input " << i++ << " is not valid!\n";
      return false;
  }


  //create metadata:
  bvxm_image_metadata observation(img,camera);
  unsigned max_scale = world->get_params()->max_scale();

  //create output_grid:
  vgl_vector_3d<unsigned int> grid_size = world->get_params()->num_voxels(scale);
  bvxm_voxel_grid_base_sptr grid_out = new bvxm_voxel_grid<float>(output_path, grid_size);
  

  bool result = true;

  if (voxel_type == "apm_mog_grey")
    result = world->region_probability_density<APM_MOG_GREY>(observation,mask,grid_out, bin_index,scale);
  else 
    vcl_cerr << "Error in: bvxm_locate_region_processor: Unsuppported appereance model" << vcl_endl;

  vcl_cout.flush();
  return true;

}

