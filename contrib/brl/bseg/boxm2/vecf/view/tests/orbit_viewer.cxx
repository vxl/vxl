#include "../boxm2_vecf_orbit_tableau.h"
//executable args
#include <vcl_fstream.h>
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
  vcl_strcpy(my_argv[argc], "--mfc-use-gl");
  vgui::init(my_argc, my_argv);
#else
    //init vgui (should choose/determine toolkit)
  vgui::init(argc, argv);
#endif

  vul_arg_info_list arglist;
  vul_arg<vcl_string> base_dir_path_arg(arglist, "-bdir", "Base model directory", "");
  vul_arg<vcl_string> bid_arg(arglist, "-bid", "orbit_params", "");
  vul_arg<vcl_string> image_path_arg(arglist, "-img", "image path", "");

  arglist.parse(argc, argv, false);
  vcl_string base_dir_path = base_dir_path_arg();
  vcl_string bid_str = bid_arg();
  vcl_string image_str = image_path_arg();

  // check if base directory exists
  bool good = vul_file::exists(base_dir_path);
  if(!good){
    vcl_cout << base_dir_path << " is not valid\n";
    return -1;
  }
  vcl_string left_param_path = base_dir_path + bid_str + "/" + bid_str + "_left_orbit_params.txt";
  vcl_string right_param_path = base_dir_path + bid_str + "/" + bid_str + "_right_orbit_params.txt";
  vcl_string image_path = base_dir_path + bid_str + "/" + image_str;

  // check for valid file paths
  good = vul_file::exists(left_param_path);
  good = good && vul_file::exists(right_param_path);
  if(!good){
    vcl_cout << left_param_path << " or " << right_param_path << " is not valid\n";
    return -1;
  }
  good = vul_file::exists(image_path);
  if(!good){
    vcl_cout << image_path << " is not valid\n";
    return -1;
  }
  boxm2_vecf_orbit_tableau_sptr otab = boxm2_vecf_orbit_tableau_new();
  if(!otab->set_image(image_path))
    return -1;
  otab->set_params(left_param_path, false);
  otab->set_params(right_param_path, true);
  otab->draw_orbit(false);
  otab->draw_orbit(true);
  unsigned w = 400, h = 340;
  vcl_string title = "Orbit Display";
  vgui_window* win = vgui::produce_window(w, h, title);
  win->get_adaptor()->set_tableau(otab);
  win->show();
  vgui::run();
}
