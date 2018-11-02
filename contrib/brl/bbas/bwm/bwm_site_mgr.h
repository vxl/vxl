#ifndef bwm_site_mgr_h_
#define bwm_site_mgr_h_
//:
// \file
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <bwm/bwm_tableau_factory.h>
#include <bwm/io/bwm_io_config_parser.h>
#include <bwm/process/bwm_site_process.h>

#include <vgui/vgui_dialog.h>
#include <vgui/vgui_dialog_extensions.h>

#include <vul/vul_timer.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class bwm_site_mgr
{
 public:
  typedef enum {MESH_FEATURE, MESH_IMAGE_PROCESSING, MESH_TERRAIN, VSOL, OBJ_UNDEF} BWM_OBJECT_TYPES;

  static bwm_site_mgr* instance();

  virtual ~bwm_site_mgr();

  void init_site();
  void create_site();
  void edit_site();
  void load_site();
  void save_site();
  void load_video_site();
  void save_video_site();

  void load_img_tableau();
  void load_cam_tableau();
  void load_video_tableau();
  void save_video_world_points_vrml();
  void save_video_cameras_vrml();
  void save_video_cams_and_world_pts_vrml();
  //: compute 3-d parameters, site bounding box and GSD
  void compute_3d_world_params();
  //:site info
  std::string site_name() { return site_name_;}
  std::string site_dir() { return site_dir_;}
  //: depth map scene io
  void load_depth_map_scene();
  void save_depth_map_scene();
 private:
  bwm_site_mgr();
  static bwm_site_mgr* instance_;

  std::vector<bwm_io_tab_config*> inactive_tableaus_;
  std::vector<bwm_io_tab_config*> active_tableaus_;

  bwm_tableau_factory tableau_factory_;

  //: object files are mapped to the file paths
  std::map<std::string, std::vector<std::string> > site_objs_;
  std::vector<std::string> object_types_;
#ifdef HAS_MFC
#if HAS_MFC
  bwm_site_process* site_create_process_;
#endif
#endif
  std::string site_name_, site_dir_, pyr_exe_ ;
  std::string camera_path_, video_path_;
  vul_timer timer_;

  double xmin_;   double ymin_;   double zmin_;  double res_;
  double xmax_;   double ymax_;   double zmax_;  bool defaultparam_;


  void create_site_dialog(vgui_dialog_extensions &site_dialog,
                          std::string &site_name,
                          std::string &site_dir,
                          std::string &pyr_exe_dir,
                          std::vector<std::string> &files,
                          bool* pyr_v, bool* act_v,
                          std::vector<std::string> &pyr_levels,
                          std::vector<std::string> &objs,
                          int *choices,
                          double &lat, double &lon, double &elev);

  bwm_io_config_parser* parse_config();
};
#endif // bwm_site_mgr_h_
