// This is brl/bseg/bvxm/grid/pro/processes/bvxm_save_rgba_grid_vrml_process.cxx

//:
// \file
// \brief A process that saves a grid of <vnl_float_4> as vrml spheres with r,g,b,transparency
//
// \author Isabel Restrepo
// \date Sept 22, 2009
// \verbatim
//  Modifications
//   <none>
// \endverbatim


#include <bprb/bprb_func_process.h>
#include <bvxm/grid/io/bvxm_vrml_voxel_grid.h>
#include <vnl/vnl_float_4.h>
#include <vcl_fstream.h>

namespace bvxm_save_rgba_grid_vrml_process_globals
{
  const unsigned n_inputs_ = 3;
  const unsigned n_outputs_ = 0;
}

bool bvxm_save_rgba_grid_vrml_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_save_rgba_grid_vrml_process_globals;

  // process takes 3 inputs but has no outputs
  //input[0]: The voxel_grid
  //input[1]: Threshold - voxels with alpha below this value are ignored
  //input[2]: The filename to write to
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_grid_base_sptr";
  input_types_[1] = "float";
  input_types_[2] = "vcl_string";

  vcl_vector<vcl_string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvxm_save_rgba_grid_vrml_process(bprb_func_process& pro)
{
  using namespace bvxm_save_rgba_grid_vrml_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }
  bvxm_voxel_grid_base_sptr grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(0);
  float threshold = pro.get_input<float>(1);
  vcl_string volume_path = pro.get_input<vcl_string>(2);
  vcl_ofstream os(volume_path.c_str());

  // create the grid from in memory file and save
  if ( bvxm_voxel_grid<vnl_float_4 > *grid = dynamic_cast<bvxm_voxel_grid<vnl_float_4 >* >(grid_base.ptr())) {
    bvrml_write::write_vrml_header(os);
    //bvxm_vrml_voxel_grid::write_vrml_grid_as_spheres(os,grid,threshold);
    bvxm_vrml_voxel_grid::write_vrml_grid_as_pointers(os,grid,threshold);
    return true;
  }
  else if (bvxm_voxel_grid<float> *grid = dynamic_cast<bvxm_voxel_grid<float>* >(grid_base.ptr()))
  {
    bvrml_write::write_vrml_header(os);
    bvxm_vrml_voxel_grid::write_vrml_grid_as_spheres(os,grid,threshold);
    return true;
  }
  else
      vcl_cerr << "Grid type not supportted yet, but you can add one!\n";
  return false;
}


