// This is brl/bseg/bvxm/grid/pro/processes/bvxm_save_grid_raw_process.cxx

//:
// \file
// \brief A process that saves a scalar grid in binary format readable by the Drishti volume rendering program
//        (http://anusf.anu.edu.au/Vizlab/drishti/)
//
// \author Isabel Restrepo
// \date Jun 19, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim


#include <bprb/bprb_func_process.h>
#include <bvxm/grid/io/bvxm_io_voxel_grid.h>


namespace bvxm_save_grid_raw_process_globals
{
  const unsigned n_inputs_ = 3;
}

bool bvxm_save_grid_raw_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_save_grid_raw_process_globals;
  // process takes 3 inputs:
  //input[0]: The filename of input grid
  //input[1]: The filename to write to
  //input[2]: String identyfying datatype
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";
  input_types_[1] = "vcl_string";
  input_types_[2] = "vcl_string";
  if (!pro.set_input_types(input_types_))
    return false;

  return true;
}

bool bvxm_save_grid_raw_process(bprb_func_process& pro)
{
  using namespace bvxm_save_grid_raw_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }

  vcl_string grid_path = pro.get_input<vcl_string>(0);
  vcl_string volume_path = pro.get_input<vcl_string>(1);
  vcl_string datatype = pro.get_input<vcl_string>(2);

  // create the grid from in memory file and save
  if (datatype == "float") {
    bvxm_voxel_grid_base_sptr grid_base = new bvxm_voxel_grid<float>(grid_path);
    bvxm_grid_save_raw<float>(grid_base,volume_path);
    return true;
  }
  else
    vcl_cerr << "Grid type not supportted yet, but you can add one!\n";
}
