#include <vcl_iostream.h>

#include <vgui/vgui.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_event_server.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D.h>

vgui_image_tableau *image_ptr;

void run_event_server(void const*)
{
  vcl_cerr << "starting event server\n";

  vgui_event_server eserve(image_ptr);

  eserve.reset();
  while (true) {
    eserve.next();
    vgui_event e = eserve.event();

    //vcl_cerr << "e.type " << e.type << vcl_endl;

    switch (e.type) {
    case vgui_KEY_PRESS:
      vcl_cerr << "vgui_KEY_PRESS \'" << char(e.key) << "\'\n";
      if (e.key == 'q') {
        vcl_cerr << "quitting event_server\n";

        return;
      }
      break;
    case vgui_KEY_RELEASE:
      vcl_cerr << "vgui_KEY_RELEASE \'" << char(e.key) << "\'\n";
      break;
    case vgui_MOTION:
      vcl_cerr << "vgui_MOTION " << e.wx << ' ' << e.wy << vcl_endl;
      break;
    case vgui_BUTTON_UP:
      vcl_cerr << "vgui_BUTTON_UP\n";
      break;
    case vgui_BUTTON_DOWN:
      vcl_cerr << "vgui_BUTTON_DOWN\n";
      break;
    case vgui_ENTER:
      vcl_cerr << "vgui_ENTER\n";
      break;
    case vgui_LEAVE:
      vcl_cerr << "vgui_LEAVE\n";
      break;
    case vgui_EVENT_NULL:
      vcl_cerr << "vgui_EVENT_NULL\n";
      break;
    default:
      //vcl_cerr << "unknown event\n";
      break;
    }
  }
}

int main(int argc, char **argv)
{
  if (argc < 2) return 1;

  vgui::init(argc, argv);

  vgui_image_tableau image( argv[1] ? argv[1] : "az32_10.tif" );
  image_ptr = &image;
  vgui_viewer2D viewer(&image);

  vgui_menu test_menu;
  test_menu.add("Event Server", run_event_server);

#if 0
  vgui_shell_tableau shell(&viewer);
  vgui_window *win = vgui::adapt(&shell, 512,512, test_menu);
#endif // 0
  vgui_window *win = vgui::adapt(&viewer, 512,512, test_menu);

  win->get_adaptor()->bind_popups(vgui_MODIFIER_NULL, vgui_RIGHT);
  win->show();

  return vgui::run();
}
