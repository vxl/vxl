//:
// \file
// \brief A process to perform linear combination of blocks in situ i.e block1 = s1*block1+ s2*block2
// \author Isabel Restrepo
// \date 2-Feb-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <boxm/boxm_scene.h>
#include <boxm/algo/boxm_linear_operations.h>

//:global variables
namespace boxm_linear_combination_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 0;
}


//:sets input and output types
bool boxm_linear_combination_process_cons(bprb_func_process& pro)
{
  using namespace boxm_linear_combination_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "boxm_scene_base_sptr" ; //scene1
  input_types_[i++] = "boxm_scene_base_sptr" ; //scene2
  input_types_[i++] = "float" ; //s1
  input_types_[i++] = "float" ; //s2
  input_types_[i++] = "int" ; //block index in x-dimension
  input_types_[i++] = "int" ; //block index in y-dimension
  input_types_[i++] = "int" ; //block index in z-dimension

  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool boxm_linear_combination_process(bprb_func_process& pro)
{
  using namespace boxm_linear_combination_process_globals;

  //get inputs
  unsigned i = 0;
  boxm_scene_base_sptr scene_base1 = pro.get_input<boxm_scene_base_sptr>(i++);
  boxm_scene_base_sptr scene_base2 = pro.get_input<boxm_scene_base_sptr>(i++);
  auto s1 = pro.get_input<float>(i++);
  auto s2 = pro.get_input<float>(i++);
  int block_i = pro.get_input<int>(i++);
  int block_j = pro.get_input<int>(i++);
  int block_k = pro.get_input<int>(i++);

  if (!(scene_base1 && scene_base2))
  {
    std::cerr << "In boxm_linear_combination_process: Null input scene\n";
    return false;
  }

  switch (scene_base1->appearence_model())
  {
    case BOXM_FLOAT:
    {
      if (scene_base2->appearence_model()!= BOXM_FLOAT)
      {
        std::cerr << "In boxm_linear_combination_process, datatype not supported\n";
        return false;
      }
      auto *scene1 = static_cast<boxm_scene<boct_tree<short, float> >*> (scene_base1.as_pointer());
      auto *scene2 = static_cast<boxm_scene<boct_tree<short, float> >*> (scene_base2.as_pointer());

      scene1->load_block(block_i,block_j,block_k);
      scene2->load_block(block_i,block_j,block_k);

      boxm_block<boct_tree<short, float> > *block1 = scene1->get_block(block_i,block_j,block_k);
      boxm_block<boct_tree<short, float> > *block2 = scene2->get_block(block_i,block_j,block_k);

      //The result is block1 = s1*block1+ s2*block2
      boxm_linear_combination(block1, block2, s1, s2);

      //make sure to write the block back to disk
      scene1->write_active_block();

      //clean memory
      scene1->unload_active_blocks();
      scene2->unload_active_blocks();

      break;
    }
    default:
      std::cerr << "In boxm_linear_combination_process: Invalid datatype\n";
      return false;
      break;
  }

  return true;
}
