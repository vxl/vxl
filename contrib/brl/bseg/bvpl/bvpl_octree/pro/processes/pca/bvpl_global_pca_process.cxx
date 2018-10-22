//:
// \file
// \brief A process that performs PCA analysis given the statistics collected from various scenes using bvpl_pca_global
// \author Isabel Restrepo
// \date 23-Mar-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_global_pca.h>

//:global variables
namespace bvpl_global_pca_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bvpl_global_pca_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_global_pca_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";   //path pca_global_info file
  input_types_[1] = "vcl_string";   //path to statistics file

  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_global_pca_process(bprb_func_process& pro)
{
  using namespace bvpl_global_pca_process_globals;

  //get inputs
  std::string pca_file = pro.get_input<std::string>(0);
  std::string stats_file = pro.get_input<std::string>(1);

  //load file into matrices
  vnl_matrix_fixed<double, 125, 125> S(0.0);
  vnl_vector_fixed<double, 125> mean(0.0);
  unsigned long nfeatures =0;
  {
    std::ifstream stats_ifs(stats_file.c_str());
    if (!stats_ifs.is_open()) {
      std::cerr << "Error: Failed to open stats file\n";
      return false;
    }
    stats_ifs >> nfeatures >> mean >> S;
  }

  bvpl_global_pca<125> global_pca(pca_file);

  global_pca.set_up_pca_evd(S,mean,nfeatures);

  return true;
}
