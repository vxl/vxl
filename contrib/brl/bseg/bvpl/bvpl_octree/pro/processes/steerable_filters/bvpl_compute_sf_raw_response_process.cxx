//:
// \file
// \brief A process to run derivatives of gaussian in 3 dimensions - x, y, x
// \author Isabel Restrepo
// \date 13-Oct-2010

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/functors/bvpl_algebraic_functor.h>
#include <bvpl/bvpl_octree/bvpl_gauss3D_steerable_filters.h>


#include <boxm/boxm_scene.h>

//: global variables
namespace bvpl_compute_sf_raw_response_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
}


//: sets input and output types
bool bvpl_compute_sf_raw_response_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_compute_sf_raw_response_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "boxm_scene_base_sptr";  //input/response scene
  input_types_[i++] = "boxm_scene_base_sptr";  //valid scene


  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//: the process
bool bvpl_compute_sf_raw_response_process(bprb_func_process& pro)
{
  using namespace bvpl_compute_sf_raw_response_process_globals;

  //get inputs:
  unsigned i = 0;
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(i++);
  boxm_scene_base_sptr valid_scene_base = pro.get_input<boxm_scene_base_sptr>(i++);

  //get scene
  typedef boct_tree<short, vnl_vector_fixed< float,10 > > response_tree_type;
  typedef boct_tree<short, bool > valid_tree_type;


  if (auto *scene_in = dynamic_cast<boxm_scene< response_tree_type >* >(scene_base.as_pointer()))
  {
    double cell_length = scene_in->finest_cell_length();

    if (auto *valid_scene = dynamic_cast<boxm_scene< valid_tree_type >* >(valid_scene_base.as_pointer()))
    {
      bvpl_gauss3D_steerable_filters sf;
      sf.basis_response_at_leaves(scene_in,valid_scene, cell_length);
    }
  }

  return true;
}
