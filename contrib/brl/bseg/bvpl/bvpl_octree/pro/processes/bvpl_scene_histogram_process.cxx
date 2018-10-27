//:
// \file
// \brief Process to explore the histogram of a scene
// \author Isabel Restrepo
// \date 18-Feb-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_scene_statistics.h>

//:global variables
namespace bvpl_scene_histogram_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bvpl_scene_histogram_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_scene_histogram_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";

  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_scene_histogram_process(bprb_func_process& pro)
{
  using namespace bvpl_scene_histogram_process_globals;

  //get inputs
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);

  if (!scene_base){
    std::cerr << "Null scene\n";
    return false;
  }

  switch (scene_base->appearence_model())
  {
    case BOXM_FLOAT:
    {
      typedef boct_tree<short, float > tree_type;
      auto *scene_in = static_cast<boxm_scene<tree_type>* > (scene_base.as_pointer());
      bsta_histogram<float> hist;
      bvpl_compute_scene_statistics(scene_in, hist);
      hist.pretty_print();
      break;
    }
    default:
      std::cout << "bvpl_block_kernel_operator_process: undefined APM type" << std::endl;
      return false;
  }

  return true;
}
