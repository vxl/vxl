//:
// \file
// \author Isabel Restrepo
// \date 4-Mar-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_discover_pca_kernels.h>

#include <vul/vul_file.h>

//:global variables
namespace bvpl_normalize_pca_training_error_process_globals
{
  constexpr unsigned n_inputs_ = 1;  //directory path
  constexpr unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bvpl_normalize_pca_training_error_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_normalize_pca_training_error_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";

  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_normalize_pca_training_error_process(bprb_func_process& pro)
{
  using namespace bvpl_normalize_pca_training_error_process_globals;

  //get inputs
  std::string pca_dir = pro.get_input<std::string>(0);

  if (!vul_file::is_directory(pca_dir)){
    std::cerr << " Invalid pca directory\n";
    return false;
  }


  bvpl_discover_pca_kernels pca_extractor(pca_dir);

  vnl_vector<double> error;
  pca_extractor.compute_normalized_training_error(error);

  std::ofstream ofs((pca_dir + "/normalized_training_error.txt").c_str());
  ofs.precision(15);
  ofs << error;

  return true;
}
