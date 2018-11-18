// This is brl/bpro/core/brad_pro/processes/brad_train_histograms_process.cxx
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief global variables and functions

#include <brad/brad_eigenspace.h>
#include <bpro/core/bbas_pro/bbas_1d_array_string.h>
#include <vil/vil_load.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

namespace bbas_core_brad_train_hist
{
  template <class T>
  bool hist_training_process(bbas_1d_array_string const& low_paths,
                             bbas_1d_array_string const& high_paths,
                             brad_eigenspace<T> & espace,
                             double frac, unsigned nit, unsigned njt,
                             bsta_joint_histogram_3d<float>& low_hist,
                             bsta_joint_histogram_3d<float>& high_hist
                            )
  {
    unsigned nlow = low_paths.data_array.size();
    unsigned nbins = 20;
    std::vector<vil_image_resource_sptr> low_resources;
    for (unsigned i  = 0; i < nlow; ++i){
      vil_image_resource_sptr temp =
        vil_load_image_resource(low_paths.data_array[i].c_str());
      if (!temp) return false;
      low_resources.push_back(temp);
    }
    unsigned nhigh = high_paths.data_array.size();
    std::vector<vil_image_resource_sptr> high_resources;
    for (unsigned i  = 0; i < nhigh; ++i){
      vil_image_resource_sptr temp =
        vil_load_image_resource(high_paths.data_array[i].c_str());
      if (!temp) return false;
      high_resources.push_back(temp);
    }
    bsta_joint_histogram_3d<float> hist_init;

    std::vector<vil_image_resource_sptr> resources = low_resources;
    for (auto & high_resource : high_resources)
      resources.push_back(high_resource);
    if (frac==1.0)
      espace.init_histogram_blocked(resources, nbins, hist_init, nit, njt);
    else
      espace.init_histogram_rand(resources, nbins, hist_init, frac, nit, njt);
    low_hist = hist_init;
    high_hist = hist_init;
    if (frac==1.0){
      espace.update_histogram_blocked(low_resources, low_hist, nit, njt);
      espace.update_histogram_blocked(high_resources, high_hist, nit, njt);
    }
    else {
      espace.update_histogram_rand(low_resources, low_hist, frac, nit, njt);
      espace.update_histogram_rand(high_resources, high_hist, frac, nit, njt);
    }
    return true;
  }
}


//: Constructor
bool brad_train_histograms_process_cons(bprb_func_process& pro)
{
  using namespace bbas_core_brad_train_hist;
  //inputs
  std::vector<std::string> input_types(6);
  input_types[0]="brad_eigenspace_sptr"; //eigenspace
  input_types[1]="bbas_1d_array_string_sptr"; //low atmos input images
  input_types[2]="bbas_1d_array_string_sptr"; //high atmos input images
  input_types[3]="double"; //fraction of image area to process
  input_types[4]="unsigned";//number of cols in a tile
  input_types[5]="unsigned";//number of rows in a tile

  //outputs
  std::vector<std::string> output_types;
  output_types.emplace_back("bsta_joint_histogram_3d_base_sptr"); // low atmos hist
  output_types.emplace_back("bsta_joint_histogram_3d_base_sptr"); // high atmos hist

  return pro.set_input_types(input_types) &&
         pro.set_output_types(output_types);
}

//: Execute the process
bool brad_train_histograms_process(bprb_func_process& pro)
{
  using namespace bbas_core_brad_train_hist;
  // Sanity check
  if (pro.n_inputs()< 3) {
    std::cout << "brad_train_histograms_process: The input number should be 1" << std::endl;
    return false;
  }
  brad_eigenspace_sptr es_ptr = pro.get_input<brad_eigenspace_sptr>(0);
  if (!es_ptr) {
    std::cout << "in train_histograms_process, null eigenspace pointer\n";
    return false;
  }
  bbas_1d_array_string_sptr low_paths =
    pro.get_input<bbas_1d_array_string_sptr>(1);

  bbas_1d_array_string_sptr high_paths =
    pro.get_input<bbas_1d_array_string_sptr>(2);

  auto frac = pro.get_input<double>(3);
  auto nit = pro.get_input<unsigned>(4);
  auto njt = pro.get_input<unsigned>(5);

  bsta_joint_histogram_3d<float> low_hist;
  bsta_joint_histogram_3d<float> high_hist;
  CAST_CALL_EIGENSPACE(es_ptr, hist_training_process(*low_paths,*high_paths, *ep,frac, nit, njt,low_hist, high_hist), "in train_histograms_process - training function failed")

  bsta_joint_histogram_3d_sptr low_ptr = new bsta_joint_histogram_3d<float>(low_hist);
  pro.set_output_val<bsta_joint_histogram_3d_sptr>(0, low_ptr);
  bsta_joint_histogram_3d_sptr high_ptr =
    new bsta_joint_histogram_3d<float>(high_hist);
  pro.set_output_val<bsta_joint_histogram_3d_sptr>(1, high_ptr);
  return true;
}
