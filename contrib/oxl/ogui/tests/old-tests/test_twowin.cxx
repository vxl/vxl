//-*- c++ -*-------------------------------------------------------------------
//
// Module: test_twowin
// Author: Philip C. Pritchett, Oxford RRG
// Created: 31 Oct 99
//
//-----------------------------------------------------------------------------

#include <vul/vul_arg.h>

#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_viewer3D.h>
#include <vrml/vgui_vrml.h>

int main(int argc, char ** argv)
{
  vgui::init(argc, argv);

  vul_arg<char*> image_file("-i", "Image File" ,0);
  vul_arg<char*> vrml_file("-v", "VRML File" ,0);
  vul_arg_parse(argc, argv);

  if (!image_file() && !vrml_file()) {
    vcl_cerr << "Specify image with -i and vrml with -v\n";
    return 0;
  }

  if (image_file) {
    vgui_image_tableau *image = new vgui_image_tableau(image_file);
    vgui_viewer2D *viewer = new vgui_viewer2D(image);

#if 0
    vgui_shell_tableau *shell = new vgui_shell_tableau(viewer);
    vgui::adapt(shell, 256, 256, image_file());
#endif // 0
    vgui::adapt(viewer, 256,256, image->name());
  }

  if (vrml_file) {
    vgui_vrml *vrml = new vgui_vrml(vrml_file);
    vgui_viewer3D *viewer = new vgui_viewer3D(vrml);

#if 0
    vgui_shell_tableau *shell = new vgui_shell_tableau(viewer);
    vgui::adapt(shell, 256, 256, vrml_file());
#endif // 0
    vgui::adapt(viewer, 256,256, vrml->name() );
  }

  return vgui::run();
}
