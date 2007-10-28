#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


#include <bwm/bwm_tableau_mgr.h>
#include <bwm/bwm_process_mgr.h>
#include <bwm/bwm_menu.h>
#include <bwm/bwm_macros.h>
#include <bwm/bwm_load_commands.h>

#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>

#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>

int main(int argc, char** argv)
{

  _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

  // initialize vgui
  // Ming: force option "--mfc-use-gl" to use gl 
  //       so that it is MUCH faster if running on a
  //       computer with nice graphic card.
  //vgui::init(argc, argv);
  int my_argc = argc+1;
  char** my_argv = new char*[argc+1];
  for (int i=0; i<argc; i++)
    my_argv[i] = argv[i];
  my_argv[argc] = "--mfc-use-gl";
  vgui::init(my_argc, my_argv);
  delete []my_argv;

  // tableau types registering
  REG_TABLEAU(bwm_load_img_command);
  REG_TABLEAU(bwm_load_cam_command);
  REG_TABLEAU(bwm_load_coin3d_command);

  // correspondence processes
  REG_PROCESS(bwm_corr_mode_command);
  REG_PROCESS(bwm_rec_corr_command);
  REG_PROCESS(bwm_save_corr_command);
  REG_PROCESS(bwm_del_last_corr_command);
  REG_PROCESS(bwm_del_corr_command);

  // image processes
  REG_PROCESS(bwm_histogram_process_command);
  REG_PROCESS(bwm_intens_profile_process_command);
  REG_PROCESS(bwm_range_map_process_command);

  REG_PROCESS(bwm_move_to_corr_command);

  // Set up the app_menu
  vgui_menu main_menu;
  bwm_menu app_menu;
  vgui_menu menu_holder = app_menu.add_to_menu(main_menu);
  
  bwm_tableau_mgr* mgr =  bwm_tableau_mgr::instance();

  // Put the grid into a shell tableau at the top the hierarchy
  vgui_shell_tableau_new shell(mgr->grid());

  // Create a window, add the tableau and show it on screen.
  //  int result =  vgui::run(shell, 600, 600, menu_holder, "Brown World Modeler" );
  vgui_window* win = vgui::produce_window(600, 600, menu_holder, "Brown World Modeler");
  win->get_adaptor()->set_tableau(shell);
  win->set_statusbar(true);
  win->set_title("World Modeler - V1.0");
  win->show();
  vgui::run();
  delete bwm_tableau_mgr::instance();

}
