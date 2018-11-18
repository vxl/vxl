#include <iostream>
#include <fstream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include "../boxm2_vecf_fit_skull.h"
int main(int argc, char ** argv)
{

  vul_arg_info_list arglist;
  vul_arg<std::string> base_dir_path(arglist, "-bdir", "Base directory", "");
  vul_arg<std::string> idstr(arglist, "-bid", " id string", "");
  vul_arg<std::string> show_model_arg(arglist, "-smod", "Show model", "true");
  vul_arg<std::string> non_lin_arg(arglist, "-nlin", "non-linear refine parameters", "true");
  vul_arg<std::string> dlib_arg(arglist, "-dlib", "2-d data from dlib", "false");

  arglist.parse(argc, argv, false);
  std::string show_model_str = show_model_arg();

  std::string non_lin_str = non_lin_arg();

  std::string dlib_str = dlib_arg();

  std::string base_dir = base_dir_path();

  if(base_dir == ""){
    std::cout << "Must have a base directory - fatal!\n";
    return -1;
  }
  std::string id = idstr();
  if(id == ""){
    std::cout << "Must have an id string - fatal!\n";
    return -1;
  }
  //  sub-directory named by id
  std::string p_dir =  base_dir + id + "/";

  std::string anchor_path = p_dir + id + "_skull_anchors.txt";

  boxm2_vecf_fit_skull fs;
  bool good = true;

  good = fs.read_anchor_file(anchor_path);
  if(!good){
    std::cout << "failed to read left anchor file\n";
    return -1;
  }
  good = fs.compute_auxillary_points();
  if(!good){
    std::cout << "failed to compute auxillary points\n";
    return -1;
  }

  good = fs.set_trans();
  if(!good){
        std::cout << "failed to compute skull transform\n";
    return -1;
  }
  std::string source_path = base_dir + "skull/skull-top-2x-r-zeroaxis-samp-1.0-r35-norm.txt";
  std::string target_path = base_dir + id + "/" + id + "_trans_skull.txt";
  good = fs.transform_skull(source_path, target_path);
  if(!good){
    std::cout << "ERROR: invalid path given to transform_skull: " << __FILE__ << __LINE__ << std::endl;
    return -1;
  }

  source_path = base_dir + "skull/mandible-2x-zero-samp-1.0-r35-norm.txt";
  target_path = base_dir + id + "/" + id + "_trans_mandible.txt";
  good = fs.transform_skull(source_path, target_path);
  if(!good){
    std::cout << "ERROR: invalid path given to transform_skull: " << __FILE__ << __LINE__ << std::endl;
    return -1;
  }

  return 0;
}
