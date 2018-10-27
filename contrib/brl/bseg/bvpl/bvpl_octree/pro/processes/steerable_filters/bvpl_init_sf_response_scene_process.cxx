//:
// \file
// \brief A process to initialize the response scene for steerable filter bank
// \author Isabel Restrepo
// \date 22-Aug-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/kernels/bvpl_gauss3d_x_kernel_factory.h>
#include <bvpl/kernels/bvpl_create_directions.h>
#include <bvpl/functors/bvpl_algebraic_functor.h>
#include <bvpl/bvpl_octree/bvpl_scene_vector_operator.h>
#include <bvpl/bvpl_octree/bvpl_scene_statistics.h>


#include <boxm/boxm_scene.h>

//: global variables
namespace bvpl_init_sf_response_scene_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 2;
}


//: sets input and output types
bool bvpl_init_sf_response_scene_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_init_sf_response_scene_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";  //input scene
  input_types_[1] = "vcl_string";            //output path

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "boxm_scene_base_sptr";  //response scene
  output_types_[1] = "boxm_scene_base_sptr";  //valid scene


  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//: the process
bool bvpl_init_sf_response_scene_process(bprb_func_process& pro)
{
  using namespace bvpl_init_sf_response_scene_process_globals;

  //get inputs:
  unsigned i = 0;
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(i++);
  std::string output_path = pro.get_input<std::string>(i++);

  //get scene
  typedef boct_tree<short, float> float_tree_type;
  typedef boct_tree<short, vnl_vector_fixed< float,10 > > out_tree_type;

  if (auto *scene_in = dynamic_cast<boxm_scene< float_tree_type >* >(scene_base.as_pointer()))
  {
    boxm_scene<out_tree_type> *scene_out =
    new boxm_scene<out_tree_type>(scene_in->lvcs(), scene_in->origin(), scene_in->block_dim(), scene_in->world_dim(), scene_in->max_level(), scene_in->init_level());
    scene_out->set_paths(output_path, "steerable_gauss_3d_scene");
    scene_out->set_appearance_model(VNL_FLOAT_10);
    scene_out->write_scene("steerable_gauss_3d_scene.xml");
    scene_in->clone_blocks_to_vector<10>(*scene_out);

    boxm_scene<boct_tree<short, bool> > *valid_scene =
    new boxm_scene<boct_tree<short, bool> >(scene_in->lvcs(), scene_in->origin(), scene_in->block_dim(), scene_in->world_dim(), scene_in->max_level(), scene_in->init_level());
    valid_scene->set_paths(output_path, "valid_scene");
    valid_scene->set_appearance_model(BOXM_BOOL);
    valid_scene->write_scene("valid_scene.xml");


    //store output
    boxm_scene_base_sptr scene_ptr=new boxm_scene_base();
    scene_ptr = scene_out;
    boxm_scene_base_sptr valid_scene_ptr=new boxm_scene_base();
    valid_scene_ptr = valid_scene;
    pro.set_output_val<boxm_scene_base_sptr>(0, scene_ptr);
    pro.set_output_val<boxm_scene_base_sptr>(1, valid_scene_ptr);
  }

  return true;
}
