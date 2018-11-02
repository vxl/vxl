// This is brl/bbas/bwm/bwm_command.h
#ifndef bwm_command_h_
#define bwm_command_h_
//:
// \file
// \brief This file defines vgui_command classes for bwm
// \author Gamze Tunali (gtunali@brown.edu)
// \date February 09, 2004
//
// Each class in this file is derived from vgui_command and
// each must define an execute member function which is called
// when the menu item is selected.
//
#include <iostream>
#include <string>
#include <bwm/bwm_tableau_sptr.h>
#include <bwm/bwm_tableau.h>

#include <vgui/vgui_command.h>
#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_menu.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: \brief The command for selecting the active tool from a menu
class bwm_command : public vgui_command
{
 public:
  bwm_command() {}
  virtual ~bwm_command() {}
  virtual std::string name() const { return "bwm_command"; }
  virtual void execute() {}
};

class bwm_tab_process_command : public bwm_command
{
 public:
  bwm_tab_process_command() {}
  virtual ~bwm_tab_process_command() {}
  virtual std::string name() const { return "bwm_tab_process_command"; }

  void set_tableau(vgui_tableau_sptr tab) {tableau_ = tab; }

 protected:
  vgui_tableau_sptr tableau_;
};

class bwm_menu_process_command : public bwm_command
{
 public:
  bwm_menu_process_command() {}
  virtual ~bwm_menu_process_command() {}
  virtual std::string name() const { return "bwm_tab_process_command"; }

  virtual void set_menu(vgui_menu& menu) {menu_ = menu; }

 protected:
  vgui_menu menu_;
};

#endif //bwm_command_h_
