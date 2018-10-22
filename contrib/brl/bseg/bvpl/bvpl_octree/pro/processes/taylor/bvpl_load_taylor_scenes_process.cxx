//:
// \file
// \brief A process to load to memory all smart pointers of scenes needed for Taylor reconstruction
// \author Isabel Restrepo
// \date 14-Feb-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vul/vul_file.h>

#include <bvpl/bvpl_octree/bvpl_taylor_basis.h>

//:global variables
namespace bvpl_load_taylor_scenes_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 1;
}


//:sets input and output types
bool bvpl_load_taylor_scenes_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_load_taylor_scenes_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "vcl_string";  //dir to Taylor scenes
  input_types_[i++] = "int";         // min and max of Taylor kernel
  input_types_[i++] = "int";
  input_types_[i++] = "int";
  input_types_[i++] = "int";
  input_types_[i++] = "int";
  input_types_[i++] = "int";
  std::vector<std::string> output_types_(n_outputs_);
  i = 0;
  output_types_[i++] = "bvpl_taylor_scenes_map_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_load_taylor_scenes_process(bprb_func_process& pro)
{
  using namespace bvpl_load_taylor_scenes_process_globals;

  //get inputs
  unsigned i = 0;
  std::string taylor_dir = pro.get_input<std::string>(i++);
  int min_x = pro.get_input<int>(i++);
  int min_y = pro.get_input<int>(i++);
  int min_z = pro.get_input<int>(i++);
  int max_x = pro.get_input<int>(i++);
  int max_y = pro.get_input<int>(i++);
  int max_z = pro.get_input<int>(i++);

  if (!vul_file::is_directory(taylor_dir)) {
    std::cerr<<"Error in bvpl_load_taylor_scenes_process_process: Taylor directory doesn't exist\n";
  }

  bvpl_taylor_basis_loader loader(taylor_dir, vgl_point_3d<int>(min_x,min_y, min_z), vgl_point_3d<int>(max_x, max_y, max_z));
  bvpl_taylor_scenes_map_sptr  taylor_scenes = new bvpl_taylor_scenes_map(loader);

  //store output
  pro.set_output_val<bvpl_taylor_scenes_map_sptr>(0, taylor_scenes);

  return true;
}
