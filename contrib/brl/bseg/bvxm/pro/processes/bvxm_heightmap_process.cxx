//This is brl/bseg/bvxm/pro/processes/bvxm_change_detection_display.cxx
//:
// \file
// \brief A class for generating a height map from a given camera viewpoint
//        -  Input:
//             - bvxm_voxel_world_sptr
//             - vpgl_camera_double_sptr
//
//        -  Output:
//             - vil_image_view_base_sptr   generated image
//
// \author  Gamze D. Tunali
// \date    04/17/2008
// \verbatim
//  Modifications
//   Isabel Restrepo - 1/27/09 - converted process-class to functions which is the new design for bvxm_processes.
// \endverbatim
#include <bprb/bprb_func_process.h>
#include <bvxm/bvxm_voxel_world.h>
#include <vil/vil_image_view.h>


bool bvxm_heightmap_process(bprb_func_process& pro)
{
  // This process has 4 inputs:
  //input[0]: The camera to render the heightmap from
  //input[1]: number of pixels (x)
  //input[2]: number of pixels (y)
  //input[3]: The voxel world
  unsigned n_inputs_ = 4;
  if(pro.n_inputs()<n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false; 
  }
  
  // get the inputs:

  // camera
  unsigned i = 0;
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);

  // npixels x
  unsigned npixels_x = pro.get_input<unsigned>(i++);

  // npixels y
  unsigned npixels_y = pro.get_input<unsigned>(i++);

  //voxel_world
  bvxm_voxel_world_sptr voxel_world =  pro.get_input<bvxm_voxel_world_sptr>(i++);

   //check inputs validity
  if (!camera) {
    vcl_cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }
   //check inputs validity
  if (!voxel_world) {
    vcl_cout << pro.name() <<" :--  Input 3  is not valid!\n";
    return false;
  }

  vil_image_view<unsigned> *hmap = new vil_image_view<unsigned>(npixels_x, npixels_y, 1);
  
  voxel_world->heightmap(camera,*hmap);
  
  //store output
  unsigned j =0;
  vcl_vector<vcl_string> output_types_(1);
  output_types_[j++] = "vil_image_view_base_sptr";
  pro.set_output_types(output_types_);
  
  j=0;
  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(hmap);
  pro.set_output(j++, output0);

  return true;
}
