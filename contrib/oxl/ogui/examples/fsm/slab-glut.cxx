/*
  fsm
*/
#include <vcl_iostream.h>

#include <vil/vil_load.h>
#include <vil/vil_image.h>

#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_shell_tableau.h>

#include <vgui/vgui_event_loop.h>

#include <vgui/impl/glut/vgui_glut_slab.h>

static bool goodbye = false;

struct example_fred_tab : public vgui_tableau
{
  vgui_slot slot;
  example_fred_tab(vgui_tableau_sptr const& t) : slot(this, t) { }
  bool handle(vgui_event const &e) {
    if (e.type == vgui_KEY_DOWN && e.key == ' ')
      vgui_event_loop_finish(); // <-- this makes the event loop return soon.
    if (e.type == vgui_KEY_DOWN && e.key == 'q') {
      vgui_event_loop_finish(); // <-- this makes the event loop return soon.
      goodbye = true;           // <-- this stops main() from restarting it.
    }
    return slot.handle(e);
  }
};

struct example_fred_tab_new : public vgui_tableau_sptr {
  example_fred_tab_new(vgui_tableau_sptr const& t) : vgui_tableau_sptr(new example_fred_tab(t)) { }
};

int main(int argc, char **argv)
{
  // make a GLUT window and and GLUT slab.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize(512, 512);
  int win = glutCreateWindow(__FILE__);
  vgui_glut_slab slab(win);

  // some tableaux
  vil_image I = vil_load( argc>1 ? argv[1] : "az32_10.tif" );
  vgui_image_tableau_new image(I);
  vgui_viewer2D_new viewer(image);
  vgui_shell_tableau_new shell(viewer);
  example_fred_tab_new ft(shell);

  // run an event loop
  vgui_event_loop_attach(&slab, ft);
  while (true) {
    vcl_cerr << "starting event loop" << vcl_endl;
    vgui_event_loop();
    if (goodbye)
      break;
    vcl_cerr << "re";
  }
  vgui_event_loop_detach(&slab);

  return 0;
}
