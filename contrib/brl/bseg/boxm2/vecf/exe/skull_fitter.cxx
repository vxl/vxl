#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include "../boxm2_vecf_fit_skull.h"
int main(int argc, char ** argv)
{

  vul_arg_info_list arglist;
  vul_arg<vcl_string> base_dir_path(arglist, "-bdir", "Base directory", "");
  vul_arg<vcl_string> idstr(arglist, "-bid", " id string", "");
  vul_arg<vcl_string> show_model_arg(arglist, "-smod", "Show model", "true");
  vul_arg<vcl_string> non_lin_arg(arglist, "-nlin", "non-linear refine parameters", "true");
  vul_arg<vcl_string> dlib_arg(arglist, "-dlib", "2-d data from dlib", "false");

  arglist.parse(argc, argv, false);
  vcl_string show_model_str = show_model_arg();

  vcl_string non_lin_str = non_lin_arg();

  vcl_string dlib_str = dlib_arg();

  vcl_string base_dir = base_dir_path();

  if(base_dir == ""){
    vcl_cout << "Must have a base directory - fatal!\n";
    return -1;
  }
  vcl_string id = idstr();
  if(id == ""){
    vcl_cout << "Must have an id string - fatal!\n";
    return -1;
  }
  //  sub-directory named by id
  vcl_string p_dir =  base_dir + id + "/";

  vcl_string anchor_path = p_dir + id + "_skull_anchors.txt";

  boxm2_vecf_fit_skull fs;
  bool good = true;

  good = fs.read_anchor_file(anchor_path);
  if(!good){
    vcl_cout << "failed to read left anchor file\n";
    return -1;
  }
  good = fs.compute_auxillary_points();
  if(!good){
    vcl_cout << "failed to compute auxillary points\n";
    return -1;
  }

  good = fs.set_trans();
  if(!good){
        vcl_cout << "failed to compute skull transform\n";
    return -1;
  }
  vcl_string source_path = base_dir + "skull/skull-top-2x-r-zeroaxis-samp-1.0-r35-norm.txt";
  vcl_string target_path = base_dir + id + "/" + id + "_trans_skull.txt";
  good = fs.transform_skull(source_path, target_path);
  if(!good){
    vcl_cout << "ERROR: invalid path given to transform_skull: " << __FILE__ << __LINE__ << vcl_endl;
    return -1;
  }

  source_path = base_dir + "skull/mandible-2x-zero-samp-1.0-r35-norm.txt";
  target_path = base_dir + id + "/" + id + "_trans_mandible.txt";
  good = fs.transform_skull(source_path, target_path);
  if(!good){
    vcl_cout << "ERROR: invalid path given to transform_skull: " << __FILE__ << __LINE__ << vcl_endl;
    return -1;
  }

  return 0;
}


