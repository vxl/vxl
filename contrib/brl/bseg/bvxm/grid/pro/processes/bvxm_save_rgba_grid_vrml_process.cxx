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


#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
#include <bvxm/grid/io/bvxm_vrml_voxel_grid.h>
#include <vnl/vnl_float_4.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

namespace bvxm_save_rgba_grid_vrml_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;
}

bool bvxm_save_rgba_grid_vrml_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_save_rgba_grid_vrml_process_globals;

  // process takes 3 inputs but has no outputs
  //input[0]: The voxel_grid
  //input[1]: Threshold - voxels with alpha below this value are ignored
  //input[2]: The filename to write to
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_grid_base_sptr";
  input_types_[1] = "float";
  input_types_[2] = "vcl_string";

  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvxm_save_rgba_grid_vrml_process(bprb_func_process& pro)
{
  using namespace bvxm_save_rgba_grid_vrml_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    std::cout << pro.name() << "The number of inputs should be " << n_inputs_ << std::endl;
    return false;
  }
  bvxm_voxel_grid_base_sptr grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(0);
  auto threshold = pro.get_input<float>(1);
  std::string volume_path = pro.get_input<std::string>(2);
  std::ofstream os(volume_path.c_str());

  // create the grid from in memory file and save
  if ( auto *grid = dynamic_cast<bvxm_voxel_grid<vnl_float_4 >* >(grid_base.ptr())) {
    bvrml_write::write_vrml_header(os);
    //bvxm_vrml_voxel_grid::write_vrml_grid_as_spheres(os,grid,threshold);
    bvxm_vrml_voxel_grid::write_vrml_grid_as_pointers(os,grid,threshold);
    return true;
  }
  else if (auto *grid = dynamic_cast<bvxm_voxel_grid<float>* >(grid_base.ptr()))
  {
    bvrml_write::write_vrml_header(os);
    bvxm_vrml_voxel_grid::write_vrml_grid_as_spheres(os,grid,threshold);
    return true;
  }
  else
      std::cerr << "Grid type not supportted yet, but you can add one!\n";
  return false;
}
