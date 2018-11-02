#include <iostream>
#include <fstream>
#include "../boxm2_vecf_estimate_camera_from_canthus.h"
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
int main(int argc , char** argv){
  boxm2_vecf_estimate_camera_from_canthus estimator;
  vul_arg_info_list arglist;
  vul_arg<std::string> base_dir_path(arglist, "-bdir", "Current work dir", "");
  vul_arg<std::string> right_dlib_path(arglist, "-rpath", "Path to file of right orbit parts", "");
  vul_arg<std::string> left_dlib_path(arglist, "-lpath", "Path to file of left orbit parts", "");
  vul_arg<std::string> alfw_path(arglist, "-alfw", "Path to file of alfw face landmarks", "");
  vul_arg<unsigned> image_height(arglist, "-h", "image height", 500);
  vul_arg<unsigned> image_width(arglist, "-w", "image width", 500);

  arglist.parse(argc, argv, false);

  bool success = true;
  success =  estimator.parse_files(left_dlib_path(), right_dlib_path(), alfw_path());
  if( ! success){
    return -1;
  }
  unsigned h = image_height();
  unsigned w = image_width();
  estimator.add_dlib_part(vgl_point_2d<double>(w,h), "image_shape");
  success = estimator.estimate_camera();
  vgl_h_matrix_2d<double> H = estimator.homography();
  std::ofstream camera_file((base_dir_path() + "canthus_camera.txt").c_str());
  if(! camera_file){
    std::cout<<" could not open file "<<"canthus_camera.txt"<<std::endl;
  }
    camera_file << H << std::endl;
    std::cout<< H.get_matrix() <<std::endl;

}
