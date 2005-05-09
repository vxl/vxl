//: 
// \file  basic01_pick_tableau.cxx
// \brief This example pick a line, box or a point with a pick tableau
// \author    Kongbin Kang
// \date        2005-05-09
// 


#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <bgui/bgui_picker_tableau.h>
#include <vgui/vgui_shell_tableau.h>
//global pointer to the rubberband tableau
static bgui_picker_tableau_sptr picker = 0;
static vgui_easy2D_tableau_sptr easy = 0;

//the meunu callback functions
static void create_box()
{
  float x1, y1, x2, y2;
  picker->set_line_width(2.0);
  picker->set_color(0, 1, 0);
  picker->pick_box(&x1, &y1, &x2, &y2);

  vcl_cerr << "corner points are (" << x1 << ", " << y1 << ") and (" << x2 << ", " << y2 << ")\n"; 

  //: draw the box in easy2D tableau
  float x[4], y[4];
  x[0]=x[3] = x1; x[1]=x[2]=x2;
  y[0]=y[1] = y1; y[2]=y[3]=y2;
  easy->add_polygon(4, x, y);
}


// Create the edit menu
vgui_menu create_menus()
{
  vgui_menu edit;
  edit.add("CreateBox",create_box,(vgui_key)'B',vgui_CTRL);
  vgui_menu bar;
  bar.add("Edit",edit);
  return bar;
}

int main(int argc, char ** argv)
{
  vgui::init(argc,argv);
  if (argc <= 1)
  {
    vcl_cerr << "Please give an image filename on the command line\n";
    return 0;
  }

  // Make the tableau hierarchy.
  vgui_image_tableau_new image(argv[1]);
  easy = vgui_easy2D_tableau_new(image);
  picker = bgui_picker_tableau_new(easy);
  vgui_viewer2D_tableau_new viewer(picker);
  vgui_shell_tableau_new shell(viewer);

  // Create and run the window
  return vgui::run(shell, 512, 512, create_menus());
}
