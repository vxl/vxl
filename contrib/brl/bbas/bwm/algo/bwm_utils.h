#ifndef bwm_utils_h_
#define bwm_utils_h_

#include <vcl_string.h>
#include <vgui/vgui_range_map_params.h>
#include <vil/vil_image_resource_sptr.h>

class bwm_utils {
  public:
    static vcl_string select_file();
    static void load_from_txt(vcl_string filename,
      vcl_string &tab_type, vcl_string &tab_name,
      vcl_string &img_path, vcl_string &cam_path,
      int &cam_type);
  
    static vil_image_resource_sptr load_image(vcl_string& filename, 
      vgui_range_map_params_sptr& rmps);

    static void show_error(vcl_string);
};


#endif