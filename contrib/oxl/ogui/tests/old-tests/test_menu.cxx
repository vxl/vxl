//:
// \file
// \author Philip C. Pritchett, Oxford RRG
// \date   05 Oct 99
//-----------------------------------------------------------------------------

#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D.h>

static void dummy(const void*)
{
  vgui::out << "Dummy function called\n";
}


vgui_menu create_menus()
{
  vgui_menu file;
  file.add("Open",dummy);
  file.separator();
  file.add("Quit",dummy);

  vgui_menu image;
  image.add("Center image",dummy);
  image.add("Show histogram",dummy);

  vgui_menu corners;
  corners.add("Load corners", dummy);
  corners.add("Compute corners", dummy);

  vgui_menu twoview;
  twoview.add("Load FMatrix", dummy);
  twoview.add("Load HMatrix2D", dummy);


  vgui_menu mview;
  mview.add("Corners",corners);
  mview.add("Two View", twoview);


  vgui_menu bar;
  bar.add("File",file);
  bar.add("Image",image);
  bar.add("MView",mview);
  bar.add("Dummy",dummy);

  return bar;
}


int main(int argc, char ** argv)
{
  if (argc < 2) return 1;

  vgui::init(argc,argv);

  vgui_image_tableau image(argv[1]);
  vgui_viewer2D viewer(&image);

#if 0
  vgui_shell_tableau shell(&viewer);
  return vgui::run(&shell, 512, 512, create_menus());
#endif // 0
  return vgui::run(&viewer,512,512,create_menus());
}
