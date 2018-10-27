//:
// \file
// \brief A process to run derivatives of gaussian in 3 dimensions - x, y, x
// \author Isabel Restrepo
// \date 13-Oct-2010

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
namespace bvpl_compute_gauss_gradients_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 1;
}


//: sets input and output types
bool bvpl_compute_gauss_gradients_cons(bprb_func_process& pro)
{
  using namespace bvpl_compute_gauss_gradients_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";  //input scene
  input_types_[1] = "vcl_string";            //output path

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "boxm_scene_base_sptr";  //output scene

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//: the process
bool bvpl_compute_gauss_gradients(bprb_func_process& pro)
{
  using namespace bvpl_compute_gauss_gradients_globals;

  //get inputs:
  unsigned i = 0;
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(i++);
  std::string output_path = pro.get_input<std::string>(i++);

  //Create the factory and get the vector of kernels
  bvpl_gauss3d_x_kernel_factory factory(1.0f,1.0f,1.0f);
  bvpl_create_directions_xyz dir;
  bvpl_kernel_vector_sptr kernels = factory.create_kernel_vector(dir);

  //get scene
  typedef boct_tree<short, float> float_tree_type;
  typedef boct_tree<short, vnl_vector_fixed< float,3 > > grad_tree_type;

  if (auto *scene_in = dynamic_cast<boxm_scene< float_tree_type >* >(scene_base.as_pointer()))
  {
    boxm_scene<grad_tree_type> *scene_out =
    new boxm_scene<grad_tree_type>(scene_in->lvcs(), scene_in->origin(), scene_in->block_dim(), scene_in->world_dim(), scene_in->max_level(), scene_in->init_level());
    scene_out->set_paths(output_path, "float_gradient_scene");
    scene_out->set_appearance_model(VNL_FLOAT_3);
    scene_out->write_scene("float_gradient_scene.xml");

    bvpl_algebraic_functor functor;
    bvpl_scene_vector_operator vector_oper;
    vector_oper.operate(*scene_in, functor, kernels, *scene_out);
    //store output
    boxm_scene_base_sptr scene_ptr=new boxm_scene_base();
    scene_ptr = scene_out;
    pro.set_output_val<boxm_scene_base_sptr>(0, scene_ptr);

    bsta_histogram<float> magnitude_hist;
    bvpl_compute_scene_statistics(scene_out, magnitude_hist);
    std::cout << "Histogram:\n" << magnitude_hist << std::endl;
  }

  return true;
}
