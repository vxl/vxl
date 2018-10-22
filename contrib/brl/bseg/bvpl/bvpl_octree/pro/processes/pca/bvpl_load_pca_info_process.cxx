//:
// \file
// \author Isabel Restrepo
// \date 3-Mar-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vul/vul_file.h>

#include <bvpl/bvpl_octree/bvpl_discover_pca_kernels.h>

//:global variables
namespace bvpl_load_pca_info_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}


//:sets input and output types
bool bvpl_load_pca_info_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_load_pca_info_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string" ;  //directory path

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bvpl_discover_pca_kernels_sptr";  //pca info

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_load_pca_info_process(bprb_func_process& pro)
{
  using namespace bvpl_load_pca_info_process_globals;

  //get inputs
  std::string pca_dir = pro.get_input<std::string>(0);

  if (!vul_file::is_directory(pca_dir))
    return false;

  bvpl_discover_pca_kernels_sptr pca_extractor = new bvpl_discover_pca_kernels(pca_dir);

  //store output
  pro.set_output_val<bvpl_discover_pca_kernels_sptr>(0, pca_extractor);

  return true;
}
