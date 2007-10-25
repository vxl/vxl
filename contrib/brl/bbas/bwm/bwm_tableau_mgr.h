#ifndef bwm_tableau_mgr_h_
#define bwm_tableau_mgr_h_

#include "bwm_tableau_sptr.h"
#include "bwm_tableau_cam.h"

#include <vcl_vector.h>

#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui.h>

#include <vil/vil_image_resource_sptr.h>

#include <vpgl/vpgl_proj_camera.h>

class bwm_tableau_mgr {
public:
  
  ~bwm_tableau_mgr();

  static bwm_tableau_mgr* bwm_tableau_mgr::instance();

  void add_tableau(vgui_tableau_sptr tab, vcl_string name);

  vgui_grid_tableau_sptr grid() { return grid_; }

  static void register_tableau(vcl_string type);
  void register_process(vcl_string process);

  void create_img_tableau(vcl_string name, vcl_string& image_path);

  void create_cam_tableau(vcl_string name, vcl_string& image_path, vcl_string& cam_path, 
                                         unsigned camera_type);

  void create_coin3d_tableau(vcl_string name, vcl_string& cam_path, unsigned camera_type);

  void create_proj2d_tableau(vcl_string name,  vcl_string type, 
                             vcl_string coin3d_name, vcl_string image_path, 
                             vcl_string camera_path, unsigned camera_type);

  void create_lidar_tableau(vcl_string name, vcl_string first_ret, vcl_string second_ret);

  //void setup_scene();
  vgui_command_sptr load_tableau_by_type(vcl_string tableau_type);
  vgui_command_sptr load_process(vcl_string name);

  void load_tableaus();

  void load_img_tableau();

  void load_cam_tableau();

  void load_coin3d_tableau();

  void load_proj2d_tableau();

  void load_lidar_tableau();

  void remove_tableau();

  void exit() { vgui::quit(); }

  //: change the mode of the correspondence
  void mode_corr();
  //: record the current correspondences into memory
  void rec_corr();
  //: save the current correspondences into a file
  void save_corr();
  void delete_last_corr();
  void delete_corr();
  static vcl_map<vcl_string, vgui_tableau_sptr> tab_types_;
  static vcl_map<vcl_string, vcl_string> process_map;
private:

  bwm_tableau_mgr();

  static bwm_tableau_mgr* instance_;

  //static vcl_map<vcl_string, vgui_tableau_sptr> tab_types_;

  //: Tableaus are mapped to their names
  vcl_map<vcl_string, vgui_tableau_sptr> tableaus_;

  vgui_grid_tableau_sptr grid_;

  vil_image_resource_sptr load_image(vcl_string& filename, vgui_range_map_params_sptr& rmps);
  
  vgui_range_map_params_sptr range_params(vil_image_resource_sptr const& image);

  vcl_string bwm_tableau_mgr::select_file();
  
  void add_to_grid(vgui_tableau_sptr tab);

  vpgl_proj_camera<double> read_projective_camera(vcl_string cam_path);

  vgui_tableau_sptr find_tableau(vcl_string name);

  vcl_vector<vcl_string> coin3d_tableau_names();

};

#endif
