// This is brl/bpro/core/bbas_pro/processes/brad_train_histograms_process.cxx
#include <bprb/bprb_func_process.h>
#include <brad/brad_hist_prob_feature_vector.h>
#include <brad/brad_eigenspace.h>
#include <bpro/core/bbas_pro/bbas_1d_array_string.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vil/vil_load.h>
#include <vcl_fstream.h>
//:
// \file
//: globals variables and functions

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

  vcl_vector<vil_image_resource_sptr> low_resources;
  for(unsigned i  = 0; i < nlow; ++i){
    vil_image_resource_sptr temp = 
      vil_load_image_resource(low_paths.data_array[i].c_str());
    if(!temp) return false;
    low_resources.push_back(temp);
  }
  unsigned nhigh = high_paths.data_array.size();
  vcl_vector<vil_image_resource_sptr> high_resources;
  for(unsigned i  = 0; i < nhigh; ++i){
    vil_image_resource_sptr temp = 
      vil_load_image_resource(high_paths.data_array[i].c_str());
    if(!temp) return false;
    high_resources.push_back(temp);
  }
  bsta_joint_histogram_3d<float> hist_init;

  vcl_vector<vil_image_resource_sptr> resources = low_resources;
  for(vcl_vector<vil_image_resource_sptr>::iterator rit = high_resources.begin();
	  rit != high_resources.end(); ++rit)
	  resources.push_back(*rit);
  if(frac==1.0)
    espace.init_histogram(resources, 20, hist_init);
  else
    espace.init_histogram_rand(resources, 20, hist_init, frac, nit, njt);
  low_hist = hist_init;
  high_hist = hist_init;
  if(frac==1.0){
  espace.update_histogram(low_resources, low_hist);
  espace.update_histogram(high_resources, high_hist);
  }else{
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
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types(6);
  input_types[0]="brad_eigenspace_sptr"; //eigenspace
  input_types[1]="bbas_1d_array_string_sptr"; //low atmos input images
  input_types[2]="bbas_1d_array_string_sptr"; //high atmos input images
  input_types[3]="double"; //fraction of image area to process
  input_types[4]="unsigned";//number of cols in a tile
  input_types[5]="unsigned";//number of rows in a tile
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types; // no outputs
  output_types.push_back("bsta_joint_histogram_3d_base_sptr"); // low atmos hist
  output_types.push_back("bsta_joint_histogram_3d_base_sptr"); // high atmos hist
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_train_histograms_process(bprb_func_process& pro)
{
  using namespace bbas_core_brad_train_hist;
  // Sanity check
  if (pro.n_inputs()< 3) {
    vcl_cout << "brad_train_histograms_process: The input number should be 1" << vcl_endl;
    return false;
  }
   brad_eigenspace_sptr es_ptr = pro.get_input<brad_eigenspace_sptr>(0);
  if(!es_ptr){
    vcl_cout << "in train_histograms_process, null eigenspace pointer\n";
    return false;
  }
  bbas_1d_array_string_sptr low_paths =
    pro.get_input<bbas_1d_array_string_sptr>(1);

  bbas_1d_array_string_sptr high_paths =
    pro.get_input<bbas_1d_array_string_sptr>(2);

  double frac = pro.get_input<double>(3);
  unsigned nit = pro.get_input<unsigned>(4);
  unsigned njt = pro.get_input<unsigned>(5);

  bsta_joint_histogram_3d<float> low_hist;
  bsta_joint_histogram_3d<float> high_hist;
  if(es_ptr->feature_vector_type() == "brad_hist_prob_feature_vector"){
    vcl_string t = "brad_hist_prob_feature_vector";
    brad_eigenspace<brad_hist_prob_feature_vector>* hp =
      dynamic_cast<brad_eigenspace<brad_hist_prob_feature_vector>* >(es_ptr.ptr());
    if(!hist_training_process(*low_paths,*high_paths, *hp,
                              frac, nit, njt,
                              low_hist, high_hist)){
      vcl_cout << "in train_histograms_process - training function failed\n";
      return false;
    }
  }else{
  vcl_cout << "in train_histograms_process - unknown eigenspace type\n";
  return false;
  }
  bsta_joint_histogram_3d_sptr low_ptr = new bsta_joint_histogram_3d<float>(low_hist);
  pro.set_output_val<bsta_joint_histogram_3d_sptr>(0, low_ptr);
  bsta_joint_histogram_3d_sptr high_ptr = 
    new bsta_joint_histogram_3d<float>(high_hist);
  pro.set_output_val<bsta_joint_histogram_3d_sptr>(1, high_ptr);
  return true;
}

