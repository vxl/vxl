#ifndef bwm_tableau_mgr_h_
#define bwm_tableau_mgr_h_

#include "bwm_tableau_sptr.h"
#include "bwm_tableau_cam.h"
#include "bwm_command_sptr.h"
#include "io/bwm_io_config_parser.h"

#include <vcl_vector.h>

#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_dialog_extensions.h>
#include <vil/vil_image_resource_sptr.h>

#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_rational_camera.h>

class bwm_tableau_mgr {

public:
  typedef enum{PROJECTIVE=0, RATIONAL=1} BWM_CAMERA_TYPES;

  ~bwm_tableau_mgr();

  static bwm_tableau_mgr* bwm_tableau_mgr::instance();

  void add_tableau(vgui_tableau_sptr tab, vcl_string name);

  vgui_grid_tableau_sptr grid() { return grid_; }

  static void register_tableau(bwm_command_sptr tab_comm);

  void create_site();

  void create_img_tableau(vcl_string name, vcl_string& image_path);

  void create_cam_tableau(vcl_string name, vcl_string& image_path, vcl_string& cam_path, 
                                         unsigned camera_type);

  void create_coin3d_tableau(vcl_string name, vcl_string& cam_path, unsigned camera_type);

  void create_proj2d_tableau(vcl_string name,  vcl_string type, 
                             vcl_string coin3d_name, vcl_string image_path, 
                             vcl_string camera_path, unsigned camera_type);

  void create_lidar_tableau(vcl_string name, vcl_string first_ret, vcl_string second_ret);

  //void setup_scene();
  bwm_command_sptr load_tableau_by_type(vcl_string tableau_type);

  void load_tableaus();

  void load_img_tableau();

  void load_cam_tableau();

  void load_coin3d_tableau();

  void load_proj2d_tableau();

  void load_lidar_tableau();

  void remove_tableau();

  void exit() { vgui::quit(); }

  void display_image_path(bool display){display_image_path_=display;}

  static vcl_map<vcl_string, bwm_command_sptr> tab_types_;

private:

  bwm_tableau_mgr();

  static bwm_tableau_mgr* instance_;

  //: Tableaus are mapped to their names
  vcl_map<vcl_string, vgui_tableau_sptr> tableaus_;

  vgui_grid_tableau_sptr grid_;

  bwm_io_config_parser* parse_config();

  vil_image_resource_sptr load_image(vcl_string& filename, vgui_range_map_params_sptr& rmps);
  
  vgui_range_map_params_sptr range_params(vil_image_resource_sptr const& image);
  
  void add_to_grid(vgui_tableau_sptr tab);

  vpgl_proj_camera<double> read_projective_camera(vcl_string cam_path);

  vgui_tableau_sptr find_tableau(vcl_string name);

  vcl_vector<vcl_string> coin3d_tableau_names();

  //vpgl_rational_camera<double> * extract_nitf_camera(vil_image_resource_sptr img);

  //vpgl_rational_camera<double> * extract_nitf_camera(vcl_string img_path);

  void create_site_dialog(vgui_dialog_extensions &site_dialog,
                               vcl_string &site_name, 
                               vcl_string &site_dir, 
                               vcl_vector<vcl_string> &files,
                               bool* pyr_v, bool* act_v,
                               vcl_vector<vcl_string> &pyr_levels,
                               vcl_vector<vcl_string> &objs,
                               int *choices,
                               double &lat, double &lon, double &elev);

  bool display_image_path_;
};

#endif
