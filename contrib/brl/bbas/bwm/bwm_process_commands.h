#ifndef bwm_process_commands_h_
#define bwm_process_commands_h_

#include <vgui/vgui_command.h>
#include "bwm_command.h"
#include "bwm_process_mgr.h"
#include "bwm_observer_mgr.h"
#include "bwm_tableau_img.h"

class bwm_jim_process_command: public bwm_tab_process_command
{
public:
  bwm_jim_process_command() {}
  ~bwm_jim_process_command() {}
  vcl_string name() {return "jim's process"; }
  void execute() 
  { //if (tableau_->type_name().compare("bwm_tableau_img") == 0) {
      bwm_tableau_img* tab = static_cast<bwm_tableau_img*> (tableau_.as_pointer());
      tab->jim_process();}
};

class bwm_step_edge_process_command: public bwm_tab_process_command
{
public:
  bwm_step_edge_process_command() {}
  ~bwm_step_edge_process_command() {}
  vcl_string name() {return "step_edge"; }
  void execute() 
  {   bwm_tableau_img* tab = static_cast<bwm_tableau_img*> (tableau_.as_pointer());
      tab->step_edges_vd();}
};

class bwm_detect_lines_process_command: public bwm_tab_process_command
{
public:
  bwm_detect_lines_process_command() {}
  ~bwm_detect_lines_process_command() {}
  vcl_string name() {return "detect_lines"; }
  void execute() 
  {   bwm_tableau_img* tab = static_cast<bwm_tableau_img*> (tableau_.as_pointer());
      tab->lines_vd();}
};


class bwm_histogram_process_command: public bwm_tab_process_command
{
public:
  bwm_histogram_process_command() {}
  ~bwm_histogram_process_command() {}
  vcl_string name() {return "histogram"; }
  void execute() 
  {
      bwm_tableau_img* tab = static_cast<bwm_tableau_img*> (tableau_.as_pointer());
      tab->hist_plot();}
};

class bwm_intens_profile_process_command: public bwm_tab_process_command
{
public:
  bwm_intens_profile_process_command() {}
  ~bwm_intens_profile_process_command() {}
  vcl_string name() {return "intensity_profile"; }
  void execute() 
  { 
      bwm_tableau_img* tab = static_cast<bwm_tableau_img*> (tableau_.as_pointer());
      tab->intensity_profile();}
};

class bwm_range_map_process_command: public bwm_tab_process_command
{
public:
  bwm_range_map_process_command() {}
  ~bwm_range_map_process_command() {}
  vcl_string name() {return "range_map"; }
  void execute() 
  {
      bwm_tableau_img* tab = static_cast<bwm_tableau_img*> (tableau_.as_pointer());
      tab->range_map();}
};

class bwm_start_corr_command: public vgui_command
{
public:
  bwm_start_corr_command() {}
  ~bwm_start_corr_command() {}
  vcl_string name() {return "start_corr"; }
  void execute() { bwm_observer_mgr::instance()->start_corr(); }

};

class bwm_corr_mode_command: public vgui_command
{
public:
  bwm_corr_mode_command() {}
  ~bwm_corr_mode_command() {}
  vcl_string name() {return "corr_mode"; }
  void execute() { bwm_observer_mgr::instance()->set_corr_mode(); }

};

class bwm_rec_corr_command: public vgui_command
{
public:
  bwm_rec_corr_command() {}
  ~bwm_rec_corr_command() {}
  vcl_string name() {return "rec_corr"; }
  void execute() { bwm_observer_mgr::instance()->collect_corr(); }

};

class bwm_save_corr_command: public vgui_command
{
public:
  bwm_save_corr_command() {}
  ~bwm_save_corr_command() {}
  vcl_string name() {return "save_corr"; }
  void execute() { bwm_observer_mgr::instance()->save_corr_XML(); }

};
class bwm_del_last_corr_command: public vgui_command
{
public:
  bwm_del_last_corr_command() {}
  ~bwm_del_last_corr_command() {}
  vcl_string name() {return "del_last_corr"; }
  void execute() { bwm_observer_mgr::instance()->delete_last_corr(); }

};

class bwm_del_corr_command: public vgui_command
{
public:
  bwm_del_corr_command() {}
  ~bwm_del_corr_command() {}
  vcl_string name() {return "del_corr"; }
  void execute() { bwm_observer_mgr::instance()->delete_all_corr(); }

};

class bwm_move_to_corr_command: public vgui_command
{
public:
  bwm_move_to_corr_command() {}
  ~bwm_move_to_corr_command() {}
  vcl_string name() {return "move_to_corr"; }
  void execute() { bwm_observer_mgr::instance()->move_to_corr(); }
};

class bwm_adjust_camera_offsets_command: public vgui_command
{
public:
  bwm_adjust_camera_offsets_command() {}
  ~bwm_adjust_camera_offsets_command() {}
  vcl_string name() {return "adjust_camera_offsets"; }
  void execute() { bwm_observer_mgr::instance()->adjust_camera_offsets(); }
};

class bwm_corresp_process_command: public bwm_menu_process_command {
public:
  bwm_corresp_process_command() {}
  ~bwm_corresp_process_command() {}
  vcl_string name() {return "correspondence"; }
  void set_menu(vgui_menu& menu) {
    menu_ = menu; 
    menu.add("Start Correspondences", new bwm_start_corr_command());
    menu.add("Correspondence Mode", new bwm_corr_mode_command());
    menu.add("Record Correspondence", new bwm_rec_corr_command());
    menu.add("Save Correspondences", new bwm_save_corr_command());
    menu.add("Delete Last Correspondence", new bwm_del_last_corr_command());
    menu.add("Delete All Correspondences", new bwm_del_corr_command());
    menu.add("Move to First Correspondence", new bwm_move_to_corr_command());
    menu.add("Adjust Cameras (One Corr.)",
             new bwm_adjust_camera_offsets_command());
  }
  void execute() {}
};

#endif
