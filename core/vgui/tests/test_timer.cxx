// This is core/vgui/tests/test_timer.cxx
// \author fsm
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_adaptor.h>

class tab : public vgui_tableau
{
 public:
  int ct;

  tab() : ct(-1) { }
  ~tab() { }

  bool handle(vgui_event const &e) {
    if (e.type == vgui_TIMER) {
      if (e.timer_id == 1234) {
        // timer loop
        vcl_cerr << "*" << vcl_flush;
        float dt = float(1.1-vcl_sin(ct/10.0))/5;
        ++ct;
        e.origin->post_timer(dt, 1234);
        return true;
      }
      else if (e.timer_id == 5678) {
        // first time through
        e.origin->post_timer(0, 1234);
        return true;
      }
      else {
        // nothing to do with us
        return false;
      }
    }
    else {
      // start it
      if (ct == -1) {
        vcl_cerr << __FILE__ ": hrrmph!\n";
        ct = 0;
        e.origin->post_timer(0, 5678);
      }
      return false;
    }
  }
};

int main(int argc,char **argv)
{
  vgui::init(argc, argv);

  return vgui::run(new tab, 256, 256, __FILE__);
}
