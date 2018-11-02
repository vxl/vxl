#ifndef bwm_io_structs_h_
#define bwm_io_structs_h_

#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// XML Tags
#define SITE_TAG "BWM_SITE"
#define SITE_HOME_TAG "home"
#define PYRAMID_EXE_TAG "PyramidExePath"
#define TABLEAUS_TAG "Tableaus"
#define CAMERA_TABLEAU_TAG "CameraTableau"
#define IMAGE_TABLEAU_TAG "ImageTableau"
#define COIN3D_TABLEAU_TAG "Coin3DTableau"
#define PROJ2D_TABLEAU_TAG "Proj2DTableau"
#define VIDEO_TABLEAU_TAG "Proj2DTableau"
#define IMAGE_PATH_TAG "imagePath"
#define CAMERA_PATH_TAG "cameraPath"
#define OBJECTS_TAG "Objects"
#define OBJECT_TAG "Object"
#define LVCS_TAG "lvcs"
// Tags for KML
#define PHOTO_OVL_TAG "PhotoOverlay"
#define KML_CAMERA_TAG "Camera"
#define KML_LON_TAG "longitude"
#define KML_LAT_TAG "latitude"
#define KML_ALT_TAG "altitude"
#define KML_HEAD_TAG "heading"
#define KML_TILT_TAG "tilt"
#define KML_ROLL_TAG "roll"
#define KML_RFOV_TAG "rightFov"
#define KML_TFOV_TAG "topFov"
#define KML_NEAR_TAG "near"
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
  bwm_io_tab_config(std::string type, std::string tab_name, bool active_or_not)
    : name(tab_name), type_name(type), status(active_or_not) {}

  bwm_io_tab_config(bwm_io_tab_config const& t)
    : name(t.name), type_name(t.type_name), status(t.status) {}

  virtual bwm_io_tab_config* clone()=0; // {return new bwm_io_tab_config(type_name, name, status); }
  virtual ~bwm_io_tab_config() {}
  std::string name;
  std::string type_name;
  bool status;
};

struct bwm_io_tab_config_img : public bwm_io_tab_config {
  bwm_io_tab_config_img(std::string name, bool status, std::string path)
    : bwm_io_tab_config(IMAGE_TABLEAU_TAG, name, status), img_path(path) {}
  bwm_io_tab_config_img(bwm_io_tab_config_img const& t)
    : bwm_io_tab_config(t), img_path(t.img_path) {}

  bwm_io_tab_config_img* clone() {return new bwm_io_tab_config_img(name, status, img_path); }
  std::string img_path;
};


struct bwm_io_tab_config_cam : public bwm_io_tab_config {
  bwm_io_tab_config_cam(std::string name, bool status,
                        std::string i_path, std::string c_path, std::string c_type,
                        bool /*adj*/=false)
    : bwm_io_tab_config(CAMERA_TABLEAU_TAG, name, status), img_path(i_path),
    cam_path(c_path), cam_type(c_type) {}

  bwm_io_tab_config_cam(bwm_io_tab_config_cam const& t)
    : bwm_io_tab_config(t), img_path(t.img_path), cam_path(t.cam_path), cam_type(t.cam_type) {}

  bwm_io_tab_config_cam* clone() {return new bwm_io_tab_config_cam(name, status, img_path, cam_path, cam_type); }
  std::string img_path;
  std::string cam_path;
  std::string cam_type;
};

struct bwm_io_tab_config_coin3d: public bwm_io_tab_config {
  bwm_io_tab_config_coin3d(std::string name, bool status,
                           std::string c_path, std::string c_type)
    : bwm_io_tab_config(COIN3D_TABLEAU_TAG, name, status), cam_path(c_path), cam_type(c_type) {}

  bwm_io_tab_config_coin3d* clone() {return new bwm_io_tab_config_coin3d(name, status, cam_path, cam_type); }
  std::string cam_path;
  std::string cam_type;
};

struct bwm_io_tab_config_proj2d: public bwm_io_tab_config {
  bwm_io_tab_config_proj2d(std::string name, bool status,
                           std::string p_type, std::string c_path,
                           std::string c_type, std::string coin3d)
    : bwm_io_tab_config(PROJ2D_TABLEAU_TAG, name, status), cam_path(c_path),
      cam_type(c_type), proj2d_type(p_type), coin3d_tab_name(coin3d) {}

  bwm_io_tab_config_proj2d* clone() { return new bwm_io_tab_config_proj2d(name, status, proj2d_type, cam_path, cam_type, coin3d_tab_name); }
  std::string cam_path;
  std::string cam_type;
  std::string proj2d_type;
  std::string coin3d_tab_name;
};

struct bwm_io_tab_config_video: public bwm_io_tab_config {
  bwm_io_tab_config_video(std::string name, bool status,
                          std::string frames,  std::string cameras)
    : bwm_io_tab_config(VIDEO_TABLEAU_TAG, name, status), video_path(frames), camera_glob(cameras) {}

  bwm_io_tab_config_video* clone() { return new bwm_io_tab_config_video(name, status, video_path, camera_glob); }
  std::string video_path;
  std::string camera_glob;
};

#endif
