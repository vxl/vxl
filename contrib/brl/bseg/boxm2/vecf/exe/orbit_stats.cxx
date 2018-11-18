#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include "../boxm2_vecf_orbit_params.h"
#include "../boxm2_vecf_orbit_param_stats.h"

int main(int argc, char ** argv)
{

  vul_arg_info_list arglist;
  vul_arg<std::string> base_dir_path(arglist, "-bdir", "Base patient directory", "");
  vul_arg<std::string> patient_idstr(arglist, "-pids", "Patient ids string", "");

  arglist.parse(argc, argv, false);

  std::string base_dir = base_dir_path();
  if(base_dir == ""){
    std::cout << "Must have a patient directory - fatal!\n";
    return -1;
  }
  std::string patient_ids = patient_idstr();
  if(patient_idstr() == ""){
    std::cout << "Must have a patient id string - fatal!\n";
    return -1;
  }
  boxm2_vecf_orbit_param_stats ops;
  std::stringstream ss(patient_ids);
  while(!ss.eof()){
    std::string patient_id;
        bool endid =false;
        while(!endid){
                char c;
                ss.get(c);
                if(c!=','&&!ss.eof())
                patient_id.push_back(c);
                else
                  endid = true;
        }

    std::string left_param_path = base_dir + patient_id + "/" + patient_id + "_left_orbit_params.txt";
    std::string right_param_path = base_dir + patient_id + "/" + patient_id + "_right_orbit_params.txt";
    boxm2_vecf_orbit_params left_params, right_params;
    std::ifstream istr_left(left_param_path.c_str());
    if(!istr_left){
      std::cout << left_param_path << " not valid\n";
      return -1;
    }
    std::ifstream istr_right(right_param_path.c_str());
    if(!istr_right){
      std::cout << right_param_path << " not valid\n";
      return -1;
    }
    istr_left >> left_params;
    istr_right >> right_params;
    std::pair<boxm2_vecf_orbit_params, boxm2_vecf_orbit_params> lr_pr(left_params,right_params);
    ops.add_param_pair(patient_id, lr_pr);
  }
  ops.generate_stats();
  ops.print_stats();
  std::cout << "\n\n======= xy fitting errors =========\n";
  ops.print_xy_fitting_error();
  std::cout << "\n\n======= xyz fitting errors =========\n";
  ops.print_xyz_fitting_error();
  std::string merged_plot_path = base_dir + "merged_plot.wrl";
  std::ofstream mstr(merged_plot_path.c_str());
  if(!mstr){
    std::cout << merged_plot_path << " not valid\n";
    return -1;
  }
  bool good = ops.merge_margins_and_crease();
  if(!good)
    return -1;
  good = ops.plot_merged_margins(mstr);
  if(!good)
    return -1;
  ops.compute_feature_vectors();
  ops.compute_covariance_matrix();
  ops.separation_stats();
  return 0;
}
