#ifndef bwm_site_mgr_h_
#define bwm_site_mgr_h_

#include "bwm_tableau_factory.h"
#include "io/bwm_io_config_parser.h"
#include "process/bwm_site_process.h"

#include <vcl_map.h>
#include <vcl_string.h>
#include <vcl_vector.h>

#include <vgui/vgui_dialog.h>
#include <vgui/vgui_dialog_extensions.h>

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

private:
  bwm_site_mgr();
  static bwm_site_mgr* instance_;

  vcl_vector<bwm_io_tab_config*> inactive_tableaus_;
  vcl_vector<bwm_io_tab_config*> active_tableaus_;

  bwm_tableau_factory tableau_factory_;

  // object files are mapped to the file paths
  vcl_map<vcl_string, vcl_vector<vcl_string> > site_objs_;
  vcl_vector<vcl_string> object_types_;
  bwm_site_process* site_create_process_;
  vcl_string site_name_, site_dir_, pyr_exe_ ;
  vcl_string camera_path_, video_path_;
  vul_timer timer_;

  void create_site_dialog(vgui_dialog_extensions &site_dialog,
                               vcl_string &site_name,
                               vcl_string &site_dir,
                               vcl_string &pyr_exe_dir,
                               vcl_vector<vcl_string> &files,
                               bool* pyr_v, bool* act_v,
                               vcl_vector<vcl_string> &pyr_levels,
                               vcl_vector<vcl_string> &objs,
                               int *choices,
                               double &lat, double &lon, double &elev);

  bwm_io_config_parser* parse_config();
};

#endif

