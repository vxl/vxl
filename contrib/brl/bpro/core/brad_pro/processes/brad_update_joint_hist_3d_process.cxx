// This is brl/bpro/core/brad_pro/processes/brad_update_joint_hist_3d_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
#include <brad/brad_eigenspace.h>
#include <bpro/core/bbas_pro/bbas_1d_array_string.h>
#include <vsl/vsl_binary_io.h>
#include <vil/vil_load.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

namespace bbas_core_brad_update_hist
{
  template <class T>
  bool hist_update_process(bbas_1d_array_string const& paths,
                           brad_eigenspace<T> & espace,
                           double frac, unsigned nit, unsigned njt,
                           bsta_joint_histogram_3d<float>& hist
                          )
  {
    auto n = (unsigned)paths.data_array.size();

    std::vector<vil_image_resource_sptr> resources;
    for (unsigned i  = 0; i < n; ++i) {
      vil_image_resource_sptr temp =
        vil_load_image_resource(paths.data_array[i].c_str());
      if (!temp) return false;
      resources.push_back(temp);
    }
    if (frac==1.0)
      espace.update_histogram_blocked(resources, hist, nit, njt);
    else
      espace.update_histogram_rand(resources, hist, frac, nit, njt);
    return true;
  }
}

//: Constructor
bool brad_update_joint_hist_3d_process_cons(bprb_func_process& pro)
{
  using namespace bbas_core_brad_update_hist;
  std::vector<std::string> input_types(6);
  input_types[0]="brad_eigenspace_sptr"; //eigenspace
  input_types[1]="bsta_joint_histogram_3d_base_sptr"; //joint_hist_3d
  input_types[2]="bbas_1d_array_string_sptr"; //input images
  input_types[3]="double"; //fraction of image area to process
  input_types[4]="unsigned";//number of cols in a tile
  input_types[5]="unsigned";//number of rows in a tile
  //one output
  std::vector<std::string> output_types;
  output_types.emplace_back("bsta_joint_histogram_3d_base_sptr"); //joint_hist_3d

  return pro.set_input_types(input_types) &&
         pro.set_output_types(output_types);
}

//: Execute the process
bool brad_update_joint_hist_3d_process(bprb_func_process& pro)
{
  using namespace bbas_core_brad_update_hist;
  // Sanity check
  if (pro.n_inputs()< 5) {
    std::cout << "brad_update_joint_hist_3d_process: The input number should be 1" << std::endl;
    return false;
  }
  brad_eigenspace_sptr es_ptr = pro.get_input<brad_eigenspace_sptr>(0);
  if (!es_ptr) {
    std::cout << "in update_joint_hist_3d_process, null eigenspace pointer\n";
    return false;
  }
  bsta_joint_histogram_3d_base_sptr hptr =
    pro.get_input<bsta_joint_histogram_3d_base_sptr>(1);

  auto* hist = dynamic_cast<bsta_joint_histogram_3d<float>*>(hptr.ptr());

  if (!hist) {
    std::cout << "in update_joint_hist_3d_process, hist can't be cast\n";
    return false;
  }

  bbas_1d_array_string_sptr paths =
    pro.get_input<bbas_1d_array_string_sptr>(2);

  auto frac = pro.get_input<double>(3);
  auto nit = pro.get_input<unsigned>(4);
  auto njt = pro.get_input<unsigned>(5);

  CAST_CALL_EIGENSPACE(es_ptr, hist_update_process(*paths, *ep, frac, nit, njt,*hist), "in update_histogram_process - update function failed")

  bsta_joint_histogram_3d_base_sptr out_ptr =
    new bsta_joint_histogram_3d<float>(*hist);
  pro.set_output_val<bsta_joint_histogram_3d_base_sptr>(0, out_ptr);
  return true;
}
