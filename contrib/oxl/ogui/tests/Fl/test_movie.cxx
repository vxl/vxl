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

#include <vnl/vnl_math.h>
#include <vul/vul_sprintf.h>

#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_observer.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui.h>

#include "Fl_movie_ui.h"


vgui_deck_tableau* deck;


bool timer_on=false;

void tickCB(void*) {

  if (speed->value() == 0) return;

  if (timer_on) {
    //vcl_cerr << "." << flush;
    double rate = 5/speed->value();

    if (rate < 0) {
      Fl::add_timeout(0-rate,tickCB);

      if (speed->value() != 0) {
        deck->prev();
      }
    }
    else {
      Fl::add_timeout(rate,tickCB);
      if (speed->value() != 0) {
        deck->next();
      }
    }
    flpane->redraw();
  }
}

void playCB(Fl_Button *w, void *data) {
  if (w->value() == 0)
    w->value(1);

  timer_on=true;
  tickCB(0);
  image_name->hide();
  scroll_pane->hide();
}

void stopCB(Fl_Button *w, void *data) {
  timer_on=false;

  deck->notify();

  image_name->show();
  scroll_pane->show();
  play->value(0);
}

void image_nameCB(Fl_Output *, void *) {
  vcl_cerr << "image_nameCB\n";
}

void speedCB(Fl_Roller *roller, void *) {
  vul_sprintf val("%.1f", vnl_math_abs(roller->value()/5));
  fps->value(val.c_str());

  if (timer_on) {
    Fl::remove_timeout(tickCB);
    tickCB(0);
  }
}

void startCB(Fl_Button *, void *) {
  //vcl_cerr << "startCB\n";
  deck->index(0);
  deck->notify();
  flpane->redraw();
}

void endCB(Fl_Button *, void *) {
  //vcl_cerr << "endCB\n";
  deck->index(deck->size()-1);
  deck->notify();
  flpane->redraw();
}

void scroll_paneCB(Fl_Scrollbar *, void *) {
  //vcl_cerr << "endCB\n";

  deck->index(scroll_pane->value());
  deck->notify();
  flpane->redraw();
}


class ImageNameObserver : public vgui_observer {
public:
  void update() {
    image_name->value(deck->current()->name().c_str());
  }
};

class ScrollPaneObserver : public vgui_observer {
public:
  void update() {
    scroll_pane->value(deck->index(),1,0,deck->size());
  }
};


int main(int argc, char **argv) {

  if (argc < 13) return 1;

  vgui::init("fltk");

  Fl_Window *window = make_window();

  deck = new vgui_deck_tableau;

  for (int i=0; i<12; ++i) {
    vgui_image_tableau *image = new vgui_image_tableau(argv[i+1]);
    deck->add(image);
  }

  vgui_viewer2D viewer(deck);

  flpane->set_tableau(&viewer);

  ImageNameObserver imagename_obs;
  ScrollPaneObserver scrollpane_obs;

  deck->attach(&imagename_obs);
  deck->attach(&scrollpane_obs);

  deck->notify();

  vul_sprintf val("%.1f", vnl_math_abs(speed->value()/5));
  fps->value(val.c_str());

  window->end();
  window->show(argc, argv);

  flpane->show();

  return Fl::run();
}
