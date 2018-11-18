// This is brl/bseg/boxm/algo/pro/processes/boxm_fill_internal_cells_process.cxx
//:
// \file
// \brief process fills the internal cells of the octrees in the scene
//
// \author Isabel Restrepo
// \date December 3, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <boxm/algo/boxm_fill_internal_cells.h>

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>


namespace boxm_fill_internal_cells_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}

//: process takes 1 input and 1 output.
// input[0]: The input scene
// output[0]: The output scene

bool boxm_fill_internal_cells_process_cons(bprb_func_process& pro)
{
  using namespace boxm_fill_internal_cells_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "boxm_scene_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_fill_internal_cells_process(bprb_func_process& pro)
{
  using namespace boxm_fill_internal_cells_process_globals;

  if (pro.n_inputs() != n_inputs_)
  {
    std::cout << pro.name() << ": the number of inputs should be " << n_inputs_
             << " but instead it is " << pro.n_inputs() << std::endl;
    return false;
  }

  //get inputs:
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);

  //check input's validity
  if (!scene_base.ptr()) {
    std::cout <<  " :-- Grid is not valid!\n";
    return false;
  }

  if (auto *scene_in = dynamic_cast<boxm_scene<boct_tree<short, float> >* > (scene_base.as_pointer()))
  {
    boxm_fill_internal_cells<float> filler;
    pro.set_output_val<boxm_scene_base_sptr>(0, filler.traverse_and_fill(scene_in));
    return true;
  }

  std::cerr << "In boxm_fill_internal_cells_process: Unsupportted scene type\n";
  return false;
}
