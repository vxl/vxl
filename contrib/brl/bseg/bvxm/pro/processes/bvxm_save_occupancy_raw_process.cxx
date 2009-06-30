// This is brl/bseg/bvxm/pro/processes/bvxm_save_occupancy_raw_process.cxx

//:
// \file
// \brief Save the voxel world occupancy grid in binary format
// A process that saves the voxel world occupancy grid in a binary format
// readable by the Drishti volume rendering program
// (http://anusf.anu.edu.au/Vizlab/drishti/)
//
// \author Daniel Crispell
// \date March 05, 2008
// \verbatim
//  Modifications
//   Brandon Mayer - Jan 28, 2009 - converted process-class to function to conform with new bvxm_process architecture.
// \endverbatim


#include <bprb/bprb_func_process.h>

#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>

namespace bvxm_save_occupancy_raw_process_globals
{
  const unsigned n_inputs_ = 4;
}

bool bvxm_save_occupancy_raw_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_save_occupancy_raw_process_globals;
  // process takes 3 inputs:
  //input[0]: The voxel world
  //input[1]: The filename to write to
  //input[2]: scale of the voxel default is 0.
  //input[3]: the appearence model as defined in bvxm_voxel_traits.h 
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "unsigned";
  input_types_[3] = "vcl_string";
  if (!pro.set_input_types(input_types_))
    return false;

  return true;
}

bool bvxm_save_occupancy_raw_process(bprb_func_process& pro)
{
  using namespace bvxm_save_occupancy_raw_process_globals;

  if (pro.n_inputs()<n_inputs_){
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  vcl_string filename = pro.get_input<vcl_string>(i++);
  unsigned scale = pro.get_input<unsigned>(i++);
  vcl_string apm = pro.get_input<vcl_string>(i++);

  if ( !world ){
    vcl_cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }

  if (apm == "ocp") 
    return world->save_occupancy_raw<OCCUPANCY>(filename,scale);
  else if (apm == "ocp_opinion") 
    return world->save_occupancy_raw<OCCUPANCY_OPINION>(filename,scale);
  else if (apm == "lidar") 
    return world->save_occupancy_raw<LIDAR>(filename,scale);
  else if (apm == "edges") 
    return world->save_occupancy_raw<EDGES>(filename,scale);
  else if (apm == "float") 
    return world->save_occupancy_raw<FLOAT>(filename,scale);
  else {
    vcl_cout << "bvxm_save_occupancy_raw_process: The appearence model [" << apm << "]is not defined!" << vcl_endl;
    return false;
  }
}
