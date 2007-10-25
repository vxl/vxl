// This is /bwm/menu/bwm_tool_command.h
#ifndef bwm_command_h_
#define bwm_command_h_

//:
// \file
// \brief This file defines vgui_command classes for bvis
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 2/09/04
//
// Each class in this file is derived from vgui_command and
// each must define an execute member function which is called
// when the menu item is selected.
//
// \verbatim
//  Modifications
//      Amir Tamrakar 06/02/04: added the file load/save commands for the new file menu
//      Matt Leotta   07/08/04: merged the load/save commands back into the original command
//                              with a new parameter to disable the queue
// \endverbatim

#include <vgui/vgui_command.h>
#include <bwm/bwm_tableau_mgr.h>

// Macros
#define MENU_LOAD_TABLEAU(name, tableau_type, menu) menu.add( name, \
  bwm_tableau_mgr::instance()->load_tableau_by_type(tableau_type)) 

#define MENU_ADD_PROCESS_NAME(name, process_name,  menu) menu.add( name, \
  bwm_tableau_mgr::instance()->load_process(process_name))
           // new bwm_process_command( bwm_manager::instance()->process_manager()->get_process_by_name(name)) )

/*#define MENU_ADD_PROCESS_NAME2(name, process_name, menu) menu.add( name, \
            new bwm_process_command( bwm_manager::instance()->process_manager()->get_process_by_name( process_name ) ) )

#define FILE_MENU_ADD_PROCESS_NAME(name, process_name, menu) menu.add(name, \
            new bwm_process_command(bwm_manager::instance()->process_manager()->get_process_by_name(process_name), false))*/



//: \brief The command for selecting the active tool from a menu
class bwm_command : public vgui_command
{
 public:
  bwm_command() {}
  void execute() {};

  //bwm_tableau_sptr tab_;
};


//: \brief The command for selecting a process from a menu
/*class bwm_process_command : public vgui_command
{
 public:
  bwm_process_command(const bwm_process_sptr& process)
   : process_(process) {}
  void execute();

  bwm_process_sptr process_;
};*/


#endif //bwm_command_h_