#ifndef bwm_utils_h_
#define bwm_utils_h_

#include <iostream>
#include <string>
#include <vcl_compiler.h>
#include <vgui/vgui_range_map_params.h>
#include <vil/vil_image_resource_sptr.h>

class bwm_utils
{
  public:
    static std::string select_file();
    static void load_from_txt(std::string filename,
                              std::string &tab_type, std::string &tab_name,
                              std::string &img_path, std::string &cam_path,
                              int &cam_type);

    static vil_image_resource_sptr load_image(std::string& filename,
                                              vgui_range_map_params_sptr& rmps);

    static void show_error(std::string);
};

#endif
