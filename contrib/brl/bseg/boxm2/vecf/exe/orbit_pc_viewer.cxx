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
#include "../boxm2_vecf_point_cloud_orbit_viewer.h"

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
    std::string pc_in_path = base_dir + patient_id + "/" + patient_id + "_sampled_points.txt";
    std::string pc_out_path = base_dir + patient_id + "/" + patient_id + "_sampled_points_orbit_display.txt";
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
    boxm2_vecf_point_cloud_orbit_viewer pcov;
    pcov.set_point_cloud(pc_in_path);
    pcov.display_orbit(left_params, false);
    pcov.display_orbit(right_params, true);
    pcov.save_point_cloud(pc_out_path);
  }
  return 0;
}
