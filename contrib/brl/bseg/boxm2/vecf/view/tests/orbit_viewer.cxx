#include <iostream>
#include <fstream>
#include "../boxm2_vecf_orbit_tableau.h"
//executable args
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
int main(int argc, char ** argv)
{
#ifdef WIN32
  int my_argc = argc+1;
  char** my_argv = new char*[argc+1];
  for (int i=0; i<argc; i++)
    my_argv[i] = argv[i];
  my_argv[argc] = new char[13];
  std::strcpy(my_argv[argc], "--mfc-use-gl");
  vgui::init(my_argc, my_argv);
#else
    //init vgui (should choose/determine toolkit)
  vgui::init(argc, argv);
#endif

  vul_arg_info_list arglist;
  vul_arg<std::string> base_dir_path_arg(arglist, "-bdir", "Base model directory", "");
  vul_arg<std::string> bid_arg(arglist, "-bid", "orbit_params", "");
  vul_arg<std::string> image_path_arg(arglist, "-img", "image path", "");
  vul_arg<std::string> points_path_arg(arglist, "-pts", "points output path", "");
  vul_arg<unsigned>  num_points_arg(arglist,  "-n_pts", "Number of points for each curve", 0);

  arglist.parse(argc, argv, false);
  std::string base_dir_path = base_dir_path_arg();
  std::string bid_str = bid_arg();
  std::string image_str = image_path_arg();
  std::string points_path = points_path_arg();
  unsigned int  num_pts = num_points_arg();

  // check if base directory exists
  bool good = vul_file::exists(base_dir_path);
  if(!good){
    std::cout <<"base dir path "<< base_dir_path << " is not valid\n";
    return -1;
  }
  bool export_points = true;
  good = vul_file::exists(points_path);
  if(!good){
    std::cout<<" points export file " << points_path << " is not valid; not exporting points\n";
    export_points = false;
  }
  std::string left_param_path = base_dir_path + bid_str + "/" + bid_str + "_left_orbit_params.txt";
  std::string right_param_path = base_dir_path + bid_str + "/" + bid_str + "_right_orbit_params.txt";
  std::string image_path = base_dir_path + bid_str + "/" + image_str;
  std::string left_parts_path = base_dir_path + bid_str + "/" + bid_str + "_left_orbit_parts.txt";
  std::string right_parts_path = base_dir_path + bid_str + "/" + bid_str + "_right_orbit_parts.txt";

  // check for valid file paths
  good = vul_file::exists(left_param_path);
  good = good && vul_file::exists(right_param_path);
  if(!good){
    std::cout << left_param_path << " or " << right_param_path << " is not valid\n";
    return -1;
  }
  good = vul_file::exists(image_path);
  if(!good){
    std::cout <<" image path "<< image_path << " is not valid\n";
    return -1;
  }
  good = vul_file::exists(left_parts_path);
  good = good && vul_file::exists(right_parts_path);
  if(!good){
    std::cout << left_parts_path << " or " << right_parts_path << " is not valid\n";
    return -1;
  }
  boxm2_vecf_orbit_tableau_sptr otab = boxm2_vecf_orbit_tableau_new();
  if(!otab->set_image(image_path))
    return -1;
  otab->set_points_filename(points_path);
  otab->set_params(left_param_path, false);
  otab->set_params(right_param_path, true);
  otab->set_dlib_parts(left_parts_path);
  otab->set_dlib_parts(right_parts_path);
  otab->draw_orbit(false,num_pts);
  otab->draw_orbit(true,num_pts);
  otab->draw_dlib_parts(false);
  otab->draw_dlib_parts(true);
  if (!export_points){
    unsigned w = 400, h = 340;
    std::string title = "Orbit Display";
    vgui_window* win = vgui::produce_window(w, h, title);
    win->get_adaptor()->set_tableau(otab);
    win->show();
    vgui::run();
  }
}
