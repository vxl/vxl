#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include "../boxm2_vecf_orbit_params.h"
#include "../boxm2_vecf_orbit_param_stats.h"
#include <vcl_sstream.h>
#include <vcl_fstream.h>
int main(int argc, char ** argv)
{
  
  vul_arg_info_list arglist;
  vul_arg<vcl_string> base_dir_path(arglist, "-bdir", "Base patient directory", "");
  vul_arg<vcl_string> patient_idstr(arglist, "-pids", "Patient ids string", "");

  arglist.parse(argc, argv, false);

  vcl_string base_dir = base_dir_path();
  if(base_dir == ""){
    vcl_cout << "Must have a patient directory - fatal!\n";
    return -1;
  }
  vcl_string patient_ids = patient_idstr();
  if(patient_idstr() == ""){
    vcl_cout << "Must have a patient id string - fatal!\n";
    return -1;
  }
  boxm2_vecf_orbit_param_stats ops;
  vcl_stringstream ss(patient_ids);
  while(!ss.eof()){
    vcl_string patient_id;
        bool endid =false;
        while(!endid){
                char c;
                ss.get(c);
                if(c!=','&&!ss.eof())
                patient_id.push_back(c);
                else
                  endid = true;
        }
          
    vcl_string left_param_path = base_dir + patient_id + "/" + patient_id + "_left_orbit_params.txt";
    vcl_string right_param_path = base_dir + patient_id + "/" + patient_id + "_right_orbit_params.txt";
    boxm2_vecf_orbit_params left_params, right_params;
    vcl_ifstream istr_left(left_param_path.c_str());
    if(!istr_left){
      vcl_cout << left_param_path << " not valid\n";
      return -1;
    }
    vcl_ifstream istr_right(right_param_path.c_str());
    if(!istr_right){
      vcl_cout << right_param_path << " not valid\n";
      return -1;
    }
    istr_left >> left_params;
    istr_right >> right_params;
    vcl_pair<boxm2_vecf_orbit_params, boxm2_vecf_orbit_params> lr_pr(left_params,right_params);
    ops.add_param_pair(patient_id, lr_pr);
  }
  ops.generate_stats();
  ops.print_stats();
  vcl_cout << "\n\n======= xy fitting errors =========\n";
  ops.print_xy_fitting_error();
  vcl_cout << "\n\n======= xyz fitting errors =========\n";
  ops.print_xyz_fitting_error();
  vcl_string merged_plot_path = base_dir + "merged_plot.wrl";
  vcl_ofstream mstr(merged_plot_path.c_str());
  if(!mstr){
    vcl_cout << merged_plot_path << " not valid\n";
    return -1;
  }
  bool good = ops.merge_margins_and_crease();
  if(!good)
    return -1;
  good = ops.plot_merged_margins(mstr);
  if(!good)
    return -1;

  return 0;
}

 
