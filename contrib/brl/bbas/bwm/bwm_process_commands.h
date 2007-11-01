#ifndef bwm_process_commands_h_
#define bwm_process_commands_h_

#include <vgui/vgui_command.h>
#include "bwm_command.h"
#include "bwm_process_mgr.h"
#include "bwm_tableau_img.h"

class bwm_jim_process_command: public bwm_tab_process_command
{
public:
  bwm_jim_process_command() {}
  ~bwm_jim_process_command() {}
  vcl_string name() {return "jim's process"; }
  void execute() 
  { if (tableau_->type_name().compare("bwm_tableau_img") == 0) {
      bwm_tableau_img* tab = static_cast<bwm_tableau_img*> (tableau_.as_pointer());
      tab->jim_process();}
  }
};

class bwm_step_edge_process_command: public bwm_tab_process_command
{
public:
  bwm_step_edge_process_command() {}
  ~bwm_step_edge_process_command() {}
  vcl_string name() {return "step edge process"; }
  void execute() 
  { if (tableau_->type_name().compare("bwm_tableau_img") == 0) {
      bwm_tableau_img* tab = static_cast<bwm_tableau_img*> (tableau_.as_pointer());
      tab->step_edges_vd();}
  }
};

class bwm_histogram_process_command: public bwm_tab_process_command
{
public:
  bwm_histogram_process_command() {}
  ~bwm_histogram_process_command() {}
  vcl_string name() {return "histogram"; }
  void execute() 
  { if (tableau_->type_name().compare("bwm_tableau_img") == 0) {
      bwm_tableau_img* tab = static_cast<bwm_tableau_img*> (tableau_.as_pointer());
      tab->hist_plot();}
  }
};

class bwm_intens_profile_process_command: public bwm_tab_process_command
{
public:
  bwm_intens_profile_process_command() {}
  ~bwm_intens_profile_process_command() {}
  vcl_string name() {return "intensity_profile"; }
  void execute() 
  { if (tableau_->type_name().compare("bwm_tableau_img") == 0) {
      bwm_tableau_img* tab = static_cast<bwm_tableau_img*> (tableau_.as_pointer());
      tab->intensity_profile();}
  }
};

class bwm_range_map_process_command: public bwm_tab_process_command
{
public:
  bwm_range_map_process_command() {}
  ~bwm_range_map_process_command() {}
  vcl_string name() {return "range_map"; }
  void execute() 
  { if (tableau_->type_name().compare("bwm_tableau_img") == 0) {
      bwm_tableau_img* tab = static_cast<bwm_tableau_img*> (tableau_.as_pointer());
      tab->range_map();}
  }
};

#endif
