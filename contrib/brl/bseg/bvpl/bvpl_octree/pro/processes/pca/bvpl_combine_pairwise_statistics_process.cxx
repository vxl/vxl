//:
// \file
// \author Isabel Restrepo
// \date 17-Mar-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>

#include <bvpl/bvpl_octree/bvpl_global_pca.h>

//:global variables
namespace bvpl_combine_pairwise_statistics_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bvpl_combine_pairwise_statistics_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_combine_pairwise_statistics_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i=0;
  input_types_[i++] = "vcl_string" ; //stats_file1
  input_types_[i++] = "vcl_string" ; //stats_file2
  input_types_[i++] = "vcl_string" ; //stats_file_out

  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_combine_pairwise_statistics_process(bprb_func_process& pro)
{
  using namespace bvpl_combine_pairwise_statistics_process_globals;

  //get inputs
  unsigned i= 0;
  std::string stats_file1 = pro.get_input<std::string>(i++);
  std::string stats_file2 = pro.get_input<std::string>(i++);
  std::string stats_file_out = pro.get_input<std::string>(i++);

  //load files
  vnl_matrix_fixed<double, 125, 125> S1(0.0);
  vnl_vector_fixed<double, 125> mean1(0.0);
  unsigned long nfeatures1 =0;
  {
    std::ifstream stats_ifs(stats_file1.c_str());
    if (!stats_ifs.is_open()) {
      std::cerr << "Error: Failed to open file1\n";
      return false;
    }
    stats_ifs >> nfeatures1 >> mean1 >> S1;
    stats_ifs.close();

  }

  vnl_matrix_fixed<double, 125, 125> S2(0.0);
  vnl_vector_fixed<double, 125> mean2(0.0);
  unsigned long nfeatures2 =0;

  {
    std::ifstream stats_ifs(stats_file2.c_str());
    if (!stats_ifs.is_open()) {
      std::cerr << "Warning: Failed to open file2 - output file will be identical to file1\n";
      std::ofstream stats_ofs(stats_file_out.c_str());
      stats_ofs.precision(15);
      stats_ofs << nfeatures1 << '\n' << mean1 << '\n' << S1;
      return true;
    }
    stats_ifs >> nfeatures2 >> mean2 >> S2;
    stats_ifs.close();
  }

  vnl_matrix_fixed<double, 125, 125> S_out(0.0);
  vnl_vector_fixed<double, 125> mean_out(0.0);
  double nfeatures_out =0.0;
  bvpl_global_pca<125>::combine_pairwise_statistics(mean1, S1, (double)nfeatures1, mean2, S2, (double)nfeatures2, mean_out, S_out, nfeatures_out);

  std::ofstream stats_ofs(stats_file_out.c_str());
  stats_ofs.precision(15);
  stats_ofs << nfeatures_out << '\n' << mean_out << '\n' << S_out;
  stats_ofs.close();

  return true;
}
