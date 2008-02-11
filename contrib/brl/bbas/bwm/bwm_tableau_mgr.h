#ifndef bwm_tableau_mgr_h_
#define bwm_tableau_mgr_h_
//:
// \file

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bwm_tableau_sptr.h"
#include "bwm_tableau_cam.h"
#include "bwm_command_sptr.h"
#include "bwm_corr_sptr.h"
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
class bwm_observer_video;
class bwm_tableau_mgr
{
 public:

  ~bwm_tableau_mgr();

  static bwm_tableau_mgr* bwm_tableau_mgr::instance();

  void add_tableau(bwm_tableau_img* tab, vcl_string name);//vgui_tableau_sptr tab, vcl_string name);

  vgui_grid_tableau_sptr grid() { return grid_; }

  static void register_tableau(bwm_command_sptr tab_comm);

  bwm_command_sptr load_tableau_by_type(vcl_string tableau_type);

  void remove_tableau();

  void exit();

  void display_image_path(bool display){display_image_path_=display;}

  void add_corresp(vcl_string tab_name, bwm_corr_sptr corr, double X, double Y);

  static vcl_map<vcl_string, bwm_command_sptr> tab_types_;

  void redraw() { grid_->post_redraw(); }

 private:

  bwm_tableau_mgr();

  //: initialize the environment to load a new site
  void init_env();

  static bwm_tableau_mgr* instance_;

  //: Tableaux are mapped to their names
  vcl_map<vcl_string, vgui_tableau_sptr> tableaus_;

  vgui_grid_tableau_sptr grid_;

  //: bool to keep the add rows and columns to the grid.
  // It alternately adds rows and columns
  bool row_added_;

  bool display_image_path_;

  //vgui_range_map_params_sptr range_params(vil_image_resource_sptr const& image);

  void add_to_grid(vgui_tableau_sptr tab);

  void add_to_grid(vgui_tableau_sptr tab, unsigned& col, unsigned& row);

  vgui_tableau_sptr find_tableau(vcl_string name);


  
};

#endif
