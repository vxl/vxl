// This is /bwm/menu/bwm_tool_command.h
#ifndef bwm_command_h_
#define bwm_command_h_

//:
// \file
// \brief This file defines vgui_command classes for bwm
// \author Gamze Tunali (gtunali@brown.edu)
// \date 2/09/04
//
// Each class in this file is derived from vgui_command and
// each must define an execute member function which is called
// when the menu item is selected.
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgui/vgui_command.h>
#include <bwm/bwm_tableau_mgr.h>

// Macros
#define MENU_LOAD_TABLEAU(name, tableau_type, menu) menu.add( name, \
  bwm_tableau_mgr::instance()->load_tableau_by_type(tableau_type).as_pointer()) 

#define MENU_ADD_PROCESS_NAME(name, process_name,  menu) menu.add( name, \
  bwm_tableau_mgr::instance()->load_process(process_name).as_pointer())


//: \brief The command for selecting the active tool from a menu
class bwm_command : public vgui_command
{
 public:
  bwm_command() {}
  virtual ~bwm_command() {}
  virtual vcl_string name() { return "bwm_command"; }
  void execute() {};
};


#endif //bwm_command_h_