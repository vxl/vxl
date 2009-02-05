// This is brl/bseg/bvxm/pro/processes/bvxm_save_edges_raw_process.cxx
//:
// \file
// \brief A process that saves the voxel world edge probability grid in a binary format readable by the Drishti volume rendering program
//        (http://anusf.anu.edu.au/Vizlab/drishti/)
//
// \author Ibrahim Eden (modification of bvxm_save_occupancy_raw_process)
// \date 03/28/08
// \verbatim
//  Modifications
//   Brandon Mayer - 1/28/09 - converted process-class to function as to conform with new bvxm_process architecture.
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>

bool bvxm_save_edges_raw_process_cons(bprb_func_process& pro)
{
  // process takes 3 inputs:
  //input[0]: The voxel world
  //input[1]: The filename to write to
  //input[2]: The scale of the world
  unsigned n_inputs_ = 3;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "unsigned";
  return pro.set_input_types(input_types_);
}

bool bvxm_save_edges_raw_process(bprb_func_process& pro)
{

  unsigned n_inputs_ = 3;
  if (pro.n_inputs()<n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  

  //get the inputs
  unsigned i = 0;
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  vcl_string filename = pro.get_input<vcl_string>(i++);
  unsigned scale = pro.get_input<unsigned>(i++);

  if ( !world ){
    vcl_cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }

  return world->save_edges_raw(filename,scale);
}
