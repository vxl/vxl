//-*- c++ -*-------------------------------------------------------------------
//
// Module: test_clear
// Author: Philip C. Pritchett, Oxford RRG
// Created: 12 Nov 99
//
//-----------------------------------------------------------------------------


#include <vul/vul_arg.h>
#include <vgui/vgui.h>
#include <vgui/vgui_clear_tableau.h>
#include <vgui/vgui_composite.h>
#include <vgui/vgui_viewer3D.h>
#include <vrml/vgui_vrml.h>


int main(int argc, char ** argv)
{
  vgui::init(argc,argv);

  vul_arg<char*> vrml_file1(0, "VRML File 1");
  vul_arg<char*> vrml_file2(0, "VRML File 2");
  vul_arg_parse(argc, argv);


  vgui_vrml vrml1(vrml_file1);
  vgui_viewer3D viewer1(&vrml1);
  //viewer1.set_active(false);

  vgui_vrml vrml2(vrml_file2);
  vgui_viewer3D viewer2(&vrml2);

  vgui_clear_tableau clear2;
  clear2.set_mask(GL_DEPTH_BUFFER_BIT);

  vgui_composite comp(&viewer1, &clear2, &viewer2);

#if 0
  vgui_shell_tableau shell(&comp);
  return vgui::run(&shell, 512, 512, "test_clear");
#endif // 0
  return vgui::run(&comp, 512, 512, "test_clear");
}
