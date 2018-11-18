// This is brl/bseg/bvpl/bvpl_octree/pro/processes/bvpl_save_vrml_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief process saves a boxm_scene of kernel responses to a vrml file
//
// \author Isabel Restrepo
// \date December 10, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/bvpl_octree/bvpl_octree_vrml_util.h>
#include <bvpl/bvpl_octree/sample/bvpl_octree_sample.h>
#include <bvpl/kernels/bvpl_kernel.h>

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>


namespace bvpl_save_vrml_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;
}

//: process takes 3 inputs and has no outputs.
// input[0]: The scene
// input[1]: The vector of kernels
// input[2]: Path to output vrml file
// No outputs

bool bvpl_save_vrml_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_save_vrml_process_globals;

  //process takes 3 inputs but has no outputs
  std::vector<std::string> output_types_(n_outputs_);
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "bvpl_kernel_vector_sptr";
  input_types_[2] = "vcl_string";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_save_vrml_process(bprb_func_process& pro)
{
  using namespace bvpl_save_vrml_process_globals;

  if (pro.n_inputs() != n_inputs_)
  {
    std::cout << pro.name() << ": the input number should be " << n_inputs_
             << " but instead it is " << pro.n_inputs() << std::endl;
    return false;
  }

  //get inputs:
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);
  bvpl_kernel_vector_sptr kernel_vector = pro.get_input<bvpl_kernel_vector_sptr>(1);
  std::string vrml_path = pro.get_input<std::string>(2);

  //check input's validity
  if (!scene_base.ptr()) {
    std::cout <<  " :-- Grid is not valid!\n";
    return false;
  }

  //:Note initial implementation is for fixed types, but this can be changed if more cases are needed

  std::ofstream ofs(vrml_path.c_str());

  if (auto *scene_in =
      dynamic_cast<boxm_scene<boct_tree<short, bvpl_octree_sample<float> > >* > (scene_base.as_pointer()))
  {
    bvpl_octree_vrml_util::write_scene_as_disks(ofs, scene_in, kernel_vector);
  }
  else if (auto *scene_in =
           dynamic_cast<boxm_scene<boct_tree<short, bvpl_octree_sample<bsta_num_obs<bsta_gauss_sf1> > > >* >(scene_base.as_pointer()))
  {
    bvpl_octree_vrml_util::write_scene_as_disks(ofs, scene_in, kernel_vector);
  }
  else {
    std::cerr << "In bvpl_save_vrml_process: Unsupported scene type\n";
  }

  return true;
}
