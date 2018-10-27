//:
// \file
// \brief
// \author Isabel Restrepo
// \date 1-Feb-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_block_kernel_operator.h>
#include <bvpl/bvpl_octree/sample/bvpl_octree_sample.h>
#include <bvpl/functors/bvpl_edge_geometric_mean_functor.h>
#include <bvpl/functors/bvpl_edge_algebraic_mean_functor.h>
#include <bvpl/functors/bvpl_gauss_convolution_functor.h>
#include <bvpl/functors/bvpl_positive_gauss_conv_functor.h>
#include <bvpl/functors/bvpl_algebraic_functor.h>

#include <boxm/boxm_scene.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/bsta_attributes.h>

#include <vul/vul_file.h>

namespace bvpl_block_kernel_operator_process_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 0;
}

//: process takes 6 inputs and has 1 output.
// * input[0]: The boxm_scene
// * input[1]: The kernels; bvpl_kernel_vector_sptr
// * input[2,3,4]: block index (i,j,k);
// * input[5]: String identifying functor type
// * input[6]: Output path to a dir where to store response blocks


bool bvpl_block_kernel_operator_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_block_kernel_operator_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i=0;
  input_types_[i++] = "boxm_scene_base_sptr";
  input_types_[i++] = "bvpl_kernel_sptr";
  input_types_[i++] = "int";
  input_types_[i++] = "int";
  input_types_[i++] = "int";
  input_types_[i++] = "vcl_string";
  input_types_[i++] = "vcl_string";
  input_types_[i++] = "double";

  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_block_kernel_operator_process(bprb_func_process& pro)
{
  using namespace bvpl_block_kernel_operator_process_globals;

  if (pro.n_inputs() < n_inputs_)
  {
    std::cerr << pro.name() << ": the input number should be " << n_inputs_
             << " but instead it is " << pro.n_inputs() << '\n';
    return false;
  }

  //get inputs:
  unsigned i = 0;
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(i++);
  bvpl_kernel_sptr kernel = pro.get_input<bvpl_kernel_sptr>(i++);
  int block_i = pro.get_input<int>(i++);
  int block_j = pro.get_input<int>(i++);
  int block_k = pro.get_input<int>(i++);
  std::string functor_name = pro.get_input<std::string>(i++);
  std::string output_path = pro.get_input<std::string>(i++);
  auto cell_length = pro.get_input<double>(i++);
  //short level = 0;

  //print inputs
  std::cout << "In bvpl_block_kernel_operator:\n"
           << "Index(i,j,k) : (" << block_i << ',' << block_j << ',' << block_k << ")\n"
           << "Functor Name: " << functor_name << '\n'
           << "Output path: " << output_path  << '\n'
           << "Cell length" << cell_length << std::endl;

  //check input's validity
  if (!scene_base.ptr()) {
    std::cerr <<  " :-- Grid is not valid!\n";
    return false;
  }

  if (!kernel) {
    std::cerr << pro.name() << " :-- Kernel is not valid!\n";
    return false;
  }


  switch (scene_base->appearence_model())
  {
    case BSTA_GAUSS_F1:
    {
      typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
      typedef boct_tree<short, gauss_type > tree_type;
      auto *scene_in = static_cast<boxm_scene<tree_type>* > (scene_base.as_pointer());


      //parameters of the output scene are the same as those of the input scene
      boxm_scene<tree_type> *scene_out =
      new boxm_scene<tree_type>(scene_in->lvcs(), scene_in->origin(), scene_in->block_dim(), scene_in->world_dim(), scene_in->max_level(), scene_in->init_level());
      scene_out->set_paths(output_path, "gauss_response_scene");
      scene_out->set_appearance_model(BSTA_GAUSS_F1);
      if (!vul_file::exists(output_path + "/gauss_response_scene.xml"))
        scene_out->write_scene("/gauss_response_scene.xml");

      //parameters of the output scene are the same as those of the input scene
      boxm_scene<boct_tree<short,bool> > *valid_scene =
      new boxm_scene<boct_tree<short,bool > >(scene_in->lvcs(), scene_in->origin(), scene_in->block_dim(), scene_in->world_dim(), scene_in->max_level(), scene_in->init_level());
      valid_scene->set_paths(output_path, "valid_scene");
      valid_scene->set_appearance_model(BOXM_BOOL);
      if (!vul_file::exists(output_path + "/valid_scene.xml"))
        valid_scene->write_scene("/valid_scene.xml");

      if (functor_name == "gauss_convolution") {
        bvpl_gauss_convolution_functor functor;
        bvpl_block_kernel_operator block_oper;
        //operate on scene
        block_oper.operate(*scene_in, functor, kernel, block_i, block_j, block_k, *scene_out, *valid_scene, cell_length);
        //clean memory
        scene_in->unload_active_blocks();
        scene_out->unload_active_blocks();

        return true;
      }
      else if (functor_name == "positive_gauss_convolution") {
        bvpl_positive_gauss_conv_functor functor;
        bvpl_block_kernel_operator block_oper;
        //operate on scene
        block_oper.operate(*scene_in, functor, kernel, block_i, block_j, block_k, *scene_out, *valid_scene, cell_length);
        //clean memory
        scene_in->unload_active_blocks();
        scene_out->unload_active_blocks();

        return true;
      }
      else
        return false;

      break;
    }
    case BOXM_FLOAT:
    {
      typedef boct_tree<short, float > tree_type;
      auto *scene_in = static_cast<boxm_scene<tree_type>* > (scene_base.as_pointer());

      //parameters of the output scene are the same as those of the input scene
      boxm_scene<tree_type> *scene_out =
      new boxm_scene<tree_type>(scene_in->lvcs(), scene_in->origin(), scene_in->block_dim(), scene_in->world_dim(), scene_in->max_level(), scene_in->init_level());
      scene_out->set_paths(output_path, "response_scene");
      scene_out->set_appearance_model(BOXM_FLOAT);
      if (!vul_file::exists(output_path + "/float_response_scene.xml"))
        scene_out->write_scene("/float_response_scene.xml");

      boxm_scene<boct_tree<short,bool> > *valid_scene =
      new boxm_scene<boct_tree<short,bool> >(scene_in->lvcs(), scene_in->origin(), scene_in->block_dim(), scene_in->world_dim(), scene_in->max_level(), scene_in->init_level());
      valid_scene->set_paths(output_path, "valid_scene");
      valid_scene->set_appearance_model(BOXM_BOOL);
      if (!vul_file::exists(output_path + "/valid_scene.xml"))
        valid_scene->write_scene("/valid_scene.xml");

      if (functor_name == "edge_algebraic_mean")
      {
        bvpl_edge_algebraic_mean_functor<float> functor;
        bvpl_block_kernel_operator block_oper;
        //operate on scene
        block_oper.operate(*scene_in, functor, kernel, block_i, block_j, block_k, *scene_out, *valid_scene, cell_length);

        //clean memory
        scene_in->unload_active_blocks();
        scene_out->unload_active_blocks();
        return true;
      }
      else if (functor_name == "algebraic")
      {
        bvpl_algebraic_functor functor;
        bvpl_block_kernel_operator block_oper;
        //operate on scene
        block_oper.operate(*scene_in, functor, kernel, block_i, block_j, block_k, *scene_out, *valid_scene, cell_length);

        //clean memory
        scene_in->unload_active_blocks();
        scene_out->unload_active_blocks();
        return true;
      }
      else
        return false;

      break;
    }
    default:
      std::cout << "bvpl_block_kernel_operator_process: undefined APM type" << std::endl;
      return false;
  }
  return false;
}
