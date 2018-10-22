//:
// \file
// \brief  A process that computes pca statistics (scatter matrix and mean) on a per block basis
// \author Isabel Restrepo
// \date 14-Mar-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_global_pca.h>

//:global variables
namespace bvpl_pca_global_statistics_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bvpl_pca_global_statistics_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_pca_global_statistics_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i =0;
  input_types_[i++] = "bvpl_global_pca_125_sptr" ; //global pca class
  input_types_[i++] = "int"; //scene_id (this can be confirmed in xml file pca_info.xml)
  input_types_[i++] = "int";   //block Indices
  input_types_[i++] = "int";
  input_types_[i++] = "int";
  input_types_[i++] = "vcl_string"; //path to file to hold statistics of this block

  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_pca_global_statistics_process(bprb_func_process& pro)
{
  using namespace bvpl_pca_global_statistics_process_globals;

  //get inputs
  unsigned i = 0;
  bvpl_global_pca_125_sptr global_pca = pro.get_input<bvpl_global_pca_125_sptr>(i++);
  int scene_id = pro.get_input<int>(i++);
  int block_i = pro.get_input<int>(i++);
  int block_j = pro.get_input<int>(i++);
  int block_k = pro.get_input<int>(i++);
  std::string stats_file = pro.get_input<std::string>(i++);

  if(!global_pca)
  {
    std::cerr << "Global PCA is NULL \n";
    return false;
  }
  vnl_matrix_fixed<double, 125, 125> S(0.0);
  vnl_vector_fixed<double, 125> mean(0.0);
  unsigned long nfeatures = 0;


  if(global_pca->sample_statistics(scene_id, block_i, block_j, block_k, S, mean, nfeatures))
  {
    std::ofstream stats_ofs(stats_file.c_str());
    stats_ofs.precision(15);
    stats_ofs << nfeatures << '\n' << mean << '\n' << S;
    stats_ofs.close();
  }

  return true;
}
