#ifndef bwm_tableau_mgr_h_
#define bwm_tableau_mgr_h_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bwm_tableau_sptr.h"
#include "bwm_tableau_cam.h"
#include "bwm_command_sptr.h"
#include "io/bwm_io_config_parser.h"
#include "process/bwm_site_process.h"

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
  typedef enum {RATIONAL=0, PROJECTIVE=1} BWM_CAMERA_TYPES;
  typedef enum {MESH_FEATURE, MESH_IMAGE_PROCESSING, MESH_TERRAIN, VSOL, OBJ_UNDEF} BWM_OBJECT_TYPES;

  ~bwm_tableau_mgr();

  static bwm_tableau_mgr* bwm_tableau_mgr::instance();

  void add_tableau(vgui_tableau_sptr tab, vcl_string name);

  vgui_grid_tableau_sptr grid() { return grid_; }

  static void register_tableau(bwm_command_sptr tab_comm);

  void create_site();

  void edit_site();

  void load_site();

  void save_site();

  void create_img_tableau(vcl_string name, vcl_string& image_path);

  void create_cam_tableau(vcl_string name, vcl_string& image_path, vcl_string& cam_path, 
                          BWM_CAMERA_TYPES camera_type, vsol_point_3d_sptr lvcs = 0);

  bwm_command_sptr load_tableau_by_type(vcl_string tableau_type);

  void load_img_tableau();

  void load_cam_tableau();

  void remove_tableau();

  void exit() { vgui::quit(); }

  void display_image_path(bool display){display_image_path_=display;}

  static vcl_map<vcl_string, bwm_command_sptr> tab_types_;
  
    
private:

  bwm_tableau_mgr();

  static bwm_tableau_mgr* instance_;

  //: Tableaus are mapped to their names
  vcl_map<vcl_string, vgui_tableau_sptr> tableaus_;
  vcl_vector<bwm_io_tab_config*> inactive_tableaus_;

  vgui_grid_tableau_sptr grid_;

  // object files are mapped to the file paths
  vcl_map<vcl_string, vcl_vector<vcl_string> > site_objs_;

  vcl_vector<vcl_string> object_types_;

  //: bool to keep the add rows and columns to the grid, 
  // it alternately adds rows and columns
  bool row_added_;

  bool display_image_path_;

  bwm_site_process* site_create_process_;

  bwm_io_config_parser* parse_config();

  vil_image_resource_sptr load_image(vcl_string& filename, vgui_range_map_params_sptr& rmps);
  
  vgui_range_map_params_sptr range_params(vil_image_resource_sptr const& image);
  
  void add_to_grid(vgui_tableau_sptr tab);

  void add_to_grid(vgui_tableau_sptr tab, unsigned& col, unsigned& row);

  vpgl_proj_camera<double> read_projective_camera(vcl_string cam_path);

  vgui_tableau_sptr find_tableau(vcl_string name);

  //vcl_vector<vcl_string> coin3d_tableau_names();

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

  void show_error(vcl_string);
};

#endif
