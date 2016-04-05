#include <fstream>
#include <string>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include "../boxm2_vecf_fit_face.h"
int main(int argc, char ** argv)
{

  vul_arg_info_list arglist;
  vul_arg<std::string> base_dir_path(arglist, "-sdir", "Base directory for subjects", "");
  vul_arg<std::string> idstr(arglist, "-sid", " subject id string", "");

  arglist.parse(argc, argv, false);

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

  std::string anchor_path = p_dir + id + "_face_anchors.txt";

  boxm2_vecf_fit_face fs;
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
        std::cout << "failed to compute face transform\n";
    return -1;
  }
  std::string param_path = base_dir + id + "/" + id + "_face_params.txt";
  std::ofstream ostr(param_path.c_str());
  if(ostr){
    ostr << fs.params();
    ostr.close();
  }else
    std::cout << "WARNING - not able to open param path " << param_path << '\n' << std::flush;

  std::string source_path = base_dir + "skull/skin_orig_appearance_sampled-r10-s1.05-t10.txt";
  std::string target_path = base_dir + id + "/" + id + "_trans_face.txt";
  good = fs.transform_face(source_path, target_path);
  if(good)
    return 0;
  return -1;
}
