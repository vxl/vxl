#include <string>
#include <iostream>
#include <cstring>
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <segv/segv_vil_menus.h>
#include <segv/segv_vil_segmentation_manager.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

int main(int argc, char* argv[])
{
#if 1
  char** my_argv = new char*[argc+1];
  my_argv[1] = new char[13];
  char* temp = my_argv[1];
  std::strcpy(my_argv[1], "--mfc-use-gl");
  my_argv[0] = argv[0];
  for (int i=1; i<argc; ++i)
    my_argv[i+1] = argv[i];

   // Initialize the toolkit.
  vgui::init(++argc, my_argv);
  delete [] temp;//init shifts argv positions
  delete [] my_argv;
#else
  vgui::init(argc, argv);
#endif
  vgui_menu menubar = segv_vil_menus::get_menu();
  unsigned w = 400, h = 340;
  std::string title = "SEG_VIL";
  vgui_window* win = vgui::produce_window(w, h, menubar, title);
  segv_vil_segmentation_manager* segm=segv_vil_segmentation_manager::instance();
  segm->set_window(win);
  win->get_adaptor()->set_tableau(segm);
  // Read a set of image file names with blank space separation
  // to load before startup
  if(argc>=2)
    for(int i = 1; i<argc; ++i){
      char* file = argv[i];
      std::string path(file);
      segm->load_image_nomenu(path);
    }
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  vgui::run();
  return 0;
}
