//-*- c++ -*-------------------------------------------------------------------
//
// Module: test_win
// Author: Philip C. Pritchett, Oxford RRG
// Created: 05 Oct 99
//
//-----------------------------------------------------------------------------

#include <vul/vul_arg.h>
#include <vgui/vgui.h>
#include <vgui/vgui_viewer3D.h>
#include <vgui/vgui_blackbox.h>
#include <vrml/vgui_vrml.h>

int main(int argc, char ** argv)
{
  vgui::init(argc,argv);

  vul_arg<char*> vrml_file(0, "VRML File");
  vul_arg_parse(argc, argv);

  vgui_vrml vrml(vrml_file);
  vgui_viewer3D viewer(&vrml);
  vgui_blackbox recorder(&viewer);

#if 0
  vgui_shell_tableau shell(&recorder);
  return vgui::run(&shell, 512, 512, "test_blackbox");
#endif // 0
  return vgui::run(&recorder, 512, 512, "test_blackbox");
}
