/*
  fsm
*/

#include <vgui/vgui.h>
#include <vgui/vgui_debug_tableau.h>
#include <vgui/vgui_shell_tableau.h>

int main(int argc,char **argv)
{
  vgui::init(argc, argv);
  
  vgui_debug_tableau_new echo(0);
  
  //vgui_shell_tableau shell(&echo);
  //return vgui::run(&shell, 256, 256, __FILE__);
  return vgui::run(echo, 256, 256, __FILE__);
}
