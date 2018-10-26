//:
// \file
// \brief A process to compute reconstruction error over all samples in a scene (test + train)
// \author Isabel Restrepo
// \date 13-Jan-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_discover_pca_kernels.h>


//:global variables
namespace bvpl_compute_pca_test_error_process_globals
{
  constexpr unsigned n_inputs_ = 1;        //directory path, where pca_info.xml is
  constexpr unsigned n_outputs_ = 1;       //error file
}


//:sets input and output types
bool bvpl_compute_pca_test_error_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_compute_pca_test_error_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string" ;

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vcl_string";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_compute_pca_test_error_process(bprb_func_process& pro)
{
  using namespace bvpl_compute_pca_test_error_process_globals;

  //get inputs
  std::string pca_dir = pro.get_input<std::string>(0);

  bvpl_discover_pca_kernels pca_extractor(pca_dir);

  vnl_vector<double> t_error;
  pca_extractor.compute_testing_error(t_error);
  std::ofstream error_ofs((pca_dir+ "/test_error.txt").c_str());

  if (error_ofs)
    error_ofs << t_error;

  //store output
  pro.set_output_val<std::string>(0, pca_dir+ "/test_error.txt");

  return true;
}
