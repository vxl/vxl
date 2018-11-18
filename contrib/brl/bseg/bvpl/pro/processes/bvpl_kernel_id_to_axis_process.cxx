// This is brl/bseg/bvpl/pro/processes/bvpl_kernel_id_to_axis_process.cxx
#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for converting a grid of vector ids, to direction
// \author Isabel Restrepo
// \date August 26, 2009
//
// \verbatim
//  Modifications
//  <none yet>
// \endverbatim

#include <bprb/bprb_parameters.h>
#include <bvxm/grid/bvxm_voxel_grid_base.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvpl/kernels/bvpl_kernel_factory.h>
#include <bvpl/bvpl_direction_to_color_map.h>
#include <vnl/vnl_float_3.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

namespace bvpl_kernel_id_to_axis_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 1;
}


//: set input and output types
bool bvpl_kernel_id_to_axis_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_kernel_id_to_axis_process_globals;

  // This process has 3 inputs and 1 output
  std::vector<std::string> input_types_(n_inputs_);
  unsigned i=0;
  input_types_[i++]="bvxm_voxel_grid_base_sptr"; //the inpud grid
  input_types_[i++]="bvpl_kernel_vector_sptr"; // a vector of kernels
  input_types_[i++]="vcl_string"; //path to output grid

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0]="bvxm_voxel_grid_base_sptr"; //the output grid

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//: Execute the process
bool bvpl_kernel_id_to_axis_process(bprb_func_process& pro)
{
  using namespace bvpl_kernel_id_to_axis_process_globals;
  // check number of inputs
  if (pro.input_types().size() != n_inputs_)
  {
    std::cout << pro.name() << "The number of inputs should be " << n_inputs_ << std::endl;
    return false;
  }

  bvxm_voxel_grid_base_sptr grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(0);
  bvpl_kernel_vector_sptr kernel = pro.get_input<bvpl_kernel_vector_sptr>(1);
  std::string output_world_dir = pro.get_input<std::string>(2);

  if (!grid_base.ptr())  {
    std::cerr << "In bvpl_kernel_id_to_axis_process -- input grid is not valid!\n";
    return false;
  }
  auto *grid
 = dynamic_cast< bvxm_voxel_grid<int>* >(grid_base.ptr());
  if (grid)
  {
    bvxm_voxel_grid<vnl_float_3> *axes_grid = new bvxm_voxel_grid<vnl_float_3>(output_world_dir, grid->grid_size());

    //iterate though grids
    bvxm_voxel_grid<int>::iterator grid_it = grid->begin();
    bvxm_voxel_grid<vnl_float_3>::iterator axes_grid_it = axes_grid->begin();
    for (; grid_it != grid->end(); ++grid_it, ++axes_grid_it)
    {
      bvxm_voxel_slab<int>::iterator slab_it = grid_it->begin();
      bvxm_voxel_slab<vnl_float_3>::iterator axes_slab_it = axes_grid_it->begin();
      for (; slab_it != grid_it->end(); ++slab_it, ++axes_slab_it)
      {
        if ((*slab_it)>-1)
          (*axes_slab_it) = kernel->kernels_[(*slab_it)]->axis();
        else
          (*axes_slab_it) =vnl_float_3(0,0,0);
      }
    }
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, axes_grid);
    return true;
  }
  else {
    std::cerr << "datatype not supported\n";
  }

  return false;
}
