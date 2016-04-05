#ifndef bwm_tableau_mgr_h_
#define bwm_tableau_mgr_h_
//:
// \file

#ifdef _MSC_VER
#pragma once
#endif // _MSC_VER

#include <bwm/bwm_tableau_sptr.h>
#include <bwm/bwm_tableau_cam.h>
#include <bwm/bwm_command_sptr.h>
#include <bwm/bwm_corr_sptr.h>
#include <bwm/io/bwm_io_config_parser.h>
#include <bwm/process/bwm_site_process.h>

#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_dialog_extensions.h>

class bwm_tableau_mgr
{
 public:

  ~bwm_tableau_mgr();

  static bwm_tableau_mgr* instance();

  void add_tableau(bwm_tableau_img* tab, std::string name);

  vgui_grid_tableau_sptr grid() { return grid_; }

  static void register_tableau(bwm_command_sptr tab_comm);

  static bool is_registered(std::string const& name);

  bwm_command_sptr load_tableau_by_type(std::string tableau_type);

  std::string save_camera(std::string tab_name);

  void save_cameras();

  void remove_tableau();

  void exit();

  void display_image_path(bool display) {display_image_path_=display;}

  void add_corresp(std::string tab_name, bwm_corr_sptr corr, double X, double Y);

  static std::map<std::string, bwm_command_sptr> tab_types_;

  void redraw() { grid_->post_redraw(); }

  vgui_tableau_sptr active_tableau();

  void set_draw_mode_vertex();
  void set_draw_mode_edge();
  void set_draw_mode_face();
  void set_draw_mode_mesh();

  void zoom_to_fit();
  void scroll_to_point();

 private:

  bwm_tableau_mgr();

  //: initialize the environment to load a new site
  void init_env();

  static bwm_tableau_mgr* instance_;

  //: Tableaux are mapped to their names
  std::map<std::string, vgui_tableau_sptr> tableaus_;

  vgui_grid_tableau_sptr grid_;

  bool display_image_path_;

  //: bool to keep the add rows and columns to the grid.
  // It alternately adds rows and columns
  bool row_added_;

  void add_to_grid(vgui_tableau_sptr tab);

  void add_to_grid(vgui_tableau_sptr tab, unsigned& col, unsigned& row);

  vgui_tableau_sptr find_tableau(std::string name);

  void set_observer_draw_mode(int mode);
};

#endif
