#ifndef bwm_io_structs_h_
#define bwm_io_structs_h_

#include <vcl_string.h>

// XML Tags
#define SITE_TAG "BWM_SITE"
#define SITE_HOME_TAG "home"
#define PYRAMID_EXE_TAG "PyramidExePath"
#define TABLEAUS_TAG "Tableaus"
#define CAMERA_TABLEAU_TAG "CameraTableau"
#define IMAGE_TABLEAU_TAG "ImageTableau"
#define COIN3D_TABLEAU_TAG "Coin3DTableau"
#define PROJ2D_TABLEAU_TAG "Proj2DTableau"
#define IMAGE_PATH_TAG "imagePath"
#define CAMERA_PATH_TAG "cameraPath"
#define OBJECTS_TAG "Objects"
#define OBJECT_TAG "Object"
#define LVCS_TAG "lvcs"

// Correspondence Tags
#define CORRESPONDENCES_TAG "Correspondences"
#define CORRESP_TAG "Correspondence"
#define CORRESP_ELM_TAG "corr_elm"
#define CORRESP_PT_TAG "corr_point"
#define CORRESP_WORLD_PT_TAG "corr_world_point"
#define CORR_CAMERA_TAG "corr_camera_tab"


class bwm_io_tab_config 
{
public:
  bwm_io_tab_config(vcl_string type, vcl_string tab_name, bool active_or_not)
    : type_name(type), name(tab_name), status(active_or_not) {}
  vcl_string name;
  vcl_string type_name;
  bool status;
};

struct bwm_io_tab_config_img : public bwm_io_tab_config {
  bwm_io_tab_config_img(vcl_string type, vcl_string name, bool status, vcl_string path) 
    : bwm_io_tab_config(type, name, status), img_path(path) {}
  vcl_string img_path;
};

  
struct bwm_io_tab_config_cam : public bwm_io_tab_config {
  bwm_io_tab_config_cam(vcl_string type, vcl_string name, bool status, 
    vcl_string i_path, vcl_string c_path, vcl_string c_type) 
    : bwm_io_tab_config(type, name, status), img_path(i_path), 
    cam_path(c_path), cam_type(c_type) {}
  vcl_string img_path;
  vcl_string cam_path;
  vcl_string cam_type; 
};

struct bwm_io_tab_config_coin3d: public bwm_io_tab_config {
  bwm_io_tab_config_coin3d(vcl_string type, vcl_string name, bool status, 
    vcl_string c_path, vcl_string c_type) 
    : bwm_io_tab_config(type, name, status), cam_path(c_path), cam_type(c_type) {}
  vcl_string cam_path;
  vcl_string cam_type; 
};

struct bwm_io_tab_config_proj2d: public bwm_io_tab_config {
  bwm_io_tab_config_proj2d(vcl_string type, vcl_string name, bool status, 
    vcl_string p_type, vcl_string c_path, vcl_string c_type, vcl_string coin3d) 
    : bwm_io_tab_config(type, name, status), proj2d_type(p_type), cam_path(c_path), 
    cam_type(c_type), coin3d_tab_name(coin3d) {}
  vcl_string cam_path;
  vcl_string cam_type; 
  vcl_string proj2d_type;
  vcl_string coin3d_tab_name;
};

#endif