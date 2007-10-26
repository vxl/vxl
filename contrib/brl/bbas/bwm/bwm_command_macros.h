// This is /bwm/menu/bwm_command_macros.h
#ifndef bwm_command_macros_h_
#define bwm_command_macros_h_

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
#include <bwm/bwm_process_mgr.h>

// Macros
#define MENU_LOAD_TABLEAU(name, tableau_type, menu) menu.add( name, \
  bwm_tableau_mgr::instance()->load_tableau_by_type(tableau_type).as_pointer()) 

#define MENU_ADD_PROCESS_NAME(name, process_name,  menu) menu.add( name, \
  bwm_process_mgr::instance()->load_process(process_name).as_pointer())

#define MENU_TAB_ADD_PROCESS(name, process_name, menu, tab) menu.add(name, \
  bwm_process_mgr::instance()->load_tab_process(process_name, tab).as_pointer())


#endif //bwm_command_h_