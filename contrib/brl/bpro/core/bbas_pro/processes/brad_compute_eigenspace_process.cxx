// This is brl/bpro/core/bbas_pro/processes/brad_compute_eigenspace_process.cxx
#include <bprb/bprb_func_process.h>
#include <brad/brad_hist_prob_feature_vector.h>
#include <brad/brad_eigenspace.h>
#include <bpro/core/bbas_pro/bbas_1d_array_string.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vil/vil_load.h>
//:
// \file
// \brief global variables and functions

namespace bbas_core_brad_compute
{
  template <class T>
  bool eigen_training_process(bbas_1d_array_string const& paths, double frac,
                              unsigned nit, unsigned njt,
                              brad_eigenspace<T>& espace)
  {
    unsigned n = paths.data_array.size();

    vcl_vector<vil_image_resource_sptr> resources;
    for (unsigned i  = 0; i < n; ++i) {
      vil_image_resource_sptr temp =
        vil_load_image_resource(paths.data_array[i].c_str());
      if (!temp) return false;
      resources.push_back(temp);
    }
    if (frac >= 1.0) {
      if (!espace.compute_covariance_matrix(resources))
        return false;
    }
    else {
      if (!espace.compute_covariance_matrix_rand(resources, frac, nit, njt))
        return false;
    }

    return espace.compute_eigensystem();
  }
}


//: Constructor
bool brad_compute_eigenspace_process_cons(bprb_func_process& pro)
{
  using namespace bbas_core_brad_compute;
  //input
  vcl_vector<vcl_string> input_types(5);
  input_types[0]="brad_eigenspace_sptr"; //eigenspace
  input_types[1]="bbas_1d_array_string_sptr"; //input images
  input_types[2]="double"; //fraction of image area to process
  input_types[3]="unsigned";//number of cols in a tile
  input_types[4]="unsigned";//number of rows in a tile
  //output
  vcl_vector<vcl_string> output_types; // no outputs

  return pro.set_input_types(input_types) &&
         pro.set_output_types(output_types);
}

//: Execute the process
bool brad_compute_eigenspace_process(bprb_func_process& pro)
{
  using namespace bbas_core_brad_compute;
  // Sanity check
  if (pro.n_inputs()< 5) {
    vcl_cout << "brad_compute_eigenspace_process: The input number should be 3" << vcl_endl;
    return false;
  }
   brad_eigenspace_sptr es_ptr = pro.get_input<brad_eigenspace_sptr>(0);
  if (!es_ptr) {
    vcl_cout << "in compute_eigenspace_process, null eigenspace pointer\n";
    return false;
  }
  bbas_1d_array_string_sptr paths =pro.get_input<bbas_1d_array_string_sptr>(1);

  //fraction of resource area to process
  double frac = pro.get_input<double>(2);

  //tile dimensions
  unsigned nit = pro.get_input<unsigned>(3);
  unsigned njt = pro.get_input<unsigned>(4);

  if (es_ptr->feature_vector_type() == "brad_hist_prob_feature_vector") {
    vcl_string t = "brad_hist_prob_feature_vector";
    brad_eigenspace<brad_hist_prob_feature_vector>* hp =
      dynamic_cast<brad_eigenspace<brad_hist_prob_feature_vector>* >(es_ptr.ptr());
    if (!eigen_training_process(*paths, frac, nit, njt, *hp)) {
      vcl_cout << "in compute_eigenspace_process - creating eigenspace failed\n";
      return false;
    }
    else
      return true;
  }
  else {
    vcl_cout << "in compute_eigenspace_process - unknown eigenspace type\n";
    return false;
  }
}

