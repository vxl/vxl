#ifndef bwm_utils_h_
#define bwm_utils_h_

#include <vcl_string.h>

class bwm_utils {
  public:
    static vcl_string select_file();
    static void load_from_txt(vcl_string filename,
      vcl_string &tab_type, vcl_string &tab_name,
      vcl_string &img_path, vcl_string &cam_path,
      int &cam_type);
};

#endif