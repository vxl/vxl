#include <vcl_iostream.h>
#include <FL/gl.h>
#include <vgui/vgui_glut.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Roller.H>

#include <vcl_cmath.h>

#include <vgui/vgui.h>
#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_easy2D.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer3D.h>
#include <vrml/vgui_vrml.h>
#include <vgui/vgui_tview.h>

#include "Fl_twowin_ui.h"


int main(int argc, char **argv) {

  if (argc < 9) {
    vcl_cerr << "First 6 command line arguments: image files\n"
             << "Next two command line arguments: VRML camera files\n";
    return 1;
  }

  vgui::init("fltk");
  Fl_Window *window = make_window();

  vgui_deck_tableau deck;

  for (int i=0; i<6; ++i) {
    vgui_image_tableau *image = new vgui_image_tableau(argv[i+1]);

    vgui_easy2D *easy2D = new vgui_easy2D(image);

    vgui_viewer2D *viewer2D = new vgui_viewer2D(easy2D);

    deck.add(viewer2D);
  }

  vgui_vrml vrml_simple(argv[7]);
  vgui_vrml vrml_nice(argv[8]);

  vgui_viewer3D viewer1(&vrml_simple);
  vgui_viewer3D viewer2(&vrml_nice);

  deck.add(&viewer1);
  deck.add(&viewer2);

  vgui_tview tview(&deck);
  vgui_viewer2D view2(&tview);

  flpane1->set_tableau(&deck);
  flpane2->set_tableau(&view2);

  window->end();
  window->show();

  flpane1->show();
  flpane2->show();

  return Fl::run();
}
