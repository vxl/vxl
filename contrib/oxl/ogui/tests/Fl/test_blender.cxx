#include <FL/gl.h>
#include <vgui/vgui_glut.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Value_Slider.H>

#include <vgui/vgui_composite.h>
#include <vgui/vgui_observer.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_image_blender.h>
#include <vgui/vgui_image_tableau.h>

#include "Fl_blender_ui.h"


vgui_image_blender *blender;

void alpha_sliderCB(Fl_Value_Slider* slider, void *) {

  blender->alpha = slider->value();
  blender->notify();
  flpane->redraw();
}


class alpha_obs : public vgui_observer {
public:
  void update() {
    alpha_slider->value(blender->alpha);
  }
};


int main(int argc, char **argv)
{
  if (argc < 3) return 1;

  Fl_Window *window = make_window();

  blender = new vgui_image_blender(argv[1], 0.5);
  vgui_image_tableau image(argv[2]);

  vgui_composite comp;
  comp.add(&image);
  comp.add(blender);

  vgui_viewer2D viewer(&comp);

  flpane->set_tableau(&viewer);

  alpha_obs a_obs;

  blender->attach(&a_obs);
  blender->notify();

  window->end();
  window->show(argc, argv);

  flpane->show();

  return Fl::run();
}
