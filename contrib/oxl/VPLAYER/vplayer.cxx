//:
//  \file

#include <vcl_compiler.h>
#include <vcl_vector.h>

#include <vil/vil_image.h>

#include <vgui/vgui.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_easy2D.h>
#include <vgui/vgui_rubberbander.h>
#include <vgui/vgui_composite.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_image_tableau.h>

#include <vidl/vidl_io.h>
#include <vidl/vidl_avicodec.h>

#include "vplayer_file.h"
#include "vplayer_geometry.h"
#include "vplayer_video.h"

#ifdef VCL_WIN32
# include <vgui/impl/mfc/vgui_mfc_app_init.h>
vgui_mfc_app_init theAppinit;
#endif

class xcv_tableau : public vgui_grid_tableau
{
public:
  xcv_tableau(int nb_images)
   : vgui_grid_tableau(nb_images, 1)
  {
  }

  ~xcv_tableau() {}
};
typedef vgui_tableau_sptr_t<xcv_tableau> xcv_tableau_sptr;

xcv_tableau_sptr xcv_tab;
//-----------------------------------------------------------------------------
//: Gets the list of all easy2D tableaux in xcv.
//-----------------------------------------------------------------------------
vcl_vector<vgui_easy2D_sptr> get_easy2D_list()
{
  vcl_vector<vgui_easy2D_sptr> easy_tabs;
  vcl_vector<vgui_tableau_sptr> all_tabs = xcv_tab->get_tableau_list();
  for (unsigned i=0; i<all_tabs.size(); i++)
  {
    vgui_easy2D_sptr easy;
    easy.vertical_cast(all_tabs[i]);
    easy_tabs.push_back(easy);
  }
  return easy_tabs;
}
//-----------------------------------------------------------------------------
//: Gets the last selected row and column position.
//-----------------------------------------------------------------------------
void get_current(unsigned* col, unsigned* row)
{
  xcv_tab->get_last_selected_position(col, row);
}
//-----------------------------------------------------------------------------
//: Return the underlying rubberbander from the tableau at the given position.
//  This function returns NULL if it fails.
//-----------------------------------------------------------------------------
vgui_rubberbander_sptr get_rubberbander_at(unsigned col, unsigned row)
{
  vgui_tableau_sptr top_tab = xcv_tab->get_tableau_at(col, row);
  if (top_tab)
  {
    vgui_rubberbander_sptr tab;
    tab.vertical_cast(top_tab);
    return tab;
  }
  else
  {
    vgui_macro_warning << "Unable to get tableau at (" << col <<", "<<row<<")\n";
    return NULL;
  }
}

//-----------------------------------------------------------------------------
//: Return the underlying easy2D from the tableau at the given position.
//  This function returns NULL if it fails.
//-----------------------------------------------------------------------------
vgui_easy2D_sptr get_easy2D_at(unsigned col, unsigned row)
{
  vgui_easy2D_sptr ret;
  vgui_rubberbander_sptr rub = get_rubberbander_at(col, row);
  if (rub) {
    ret.vertical_cast(
      rub);
    return ret;
  }
  vgui_macro_warning << "Unable to get easy2D at (" << col << ", " << row << ")\n";
  return 0;
}
//-----------------------------------------------------------------------------
//: Gets the image tableau from the tableau at the given position.
//  This function returns NULL if it fails.
//-----------------------------------------------------------------------------
vgui_image_tableau_sptr get_image_tableau_at(unsigned col, unsigned row)
{
  vgui_image_tableau_sptr ret;
  vgui_easy2D_sptr tab = get_easy2D_at(col, row);
  if (tab)
  {
    ret.vertical_cast(
      tab);
  }
  vgui_macro_warning << "Unable to get vgui_image_tableau at (" << col << ", "
    << row << ")\n";
  return 0;
}
vgui_menu vplayer_menubar;

vgui_menu create_menubar()
{
  vplayer_menubar.add("File", vplayer_file::create_file_menu());
  vplayer_menubar.add("Geometry", vplayer_geometry::create_geometry_menu());
  vplayer_menubar.add("Video",vplayer_video::create_video_menu());
  return vplayer_menubar;
}

//-----------------------------------------------------------------------------
// main.
//-----------------------------------------------------------------------------
extern int vgui_mfc_tag_function();
extern int vgui_gtk_tag_function();
extern int vgui_accelerate_x11_tag_function();
extern int vgui_accelerate_mfc_tag_function();
int main(int argc, char** argv)
{
  // Register video codec
  vidl_io::register_codec(new vidl_avicodec);

#ifdef HAS_GTK
  vgui_gtk_tag_function();
#endif
#ifdef HAS_MESA
  vgui_accelerate_x11_tag_function();
#endif
#ifdef VCL_WIN32
  vgui_mfc_tag_function();
  vgui_accelerate_mfc_tag_function();
#endif

  vgui::init(argc,argv);

  xcv_tab = new xcv_tableau(argc-1);
  int argcount = 1;
  unsigned window_width = 0;
  unsigned window_height = 0;

  vgui_image_tableau_new image;
  vgui_easy2D_new easy(image);
  vgui_rubberbander_new rubber(new vgui_rubberbander_easy2D_client(easy));
  vgui_composite_new c(easy, rubber);
  vgui_viewer2D_new view(c);
  xcv_tab->add_next(view);
  argcount++;
  if (image->height() > window_height)
     window_height = image->height();
   window_width = window_width + image->width();
  if (window_width == 0)
    window_width = 512;
  if (window_width > 1024)
    window_width = 1024;
  if (window_height == 0)
    window_height = 512;
  if (window_height > 1024)
    window_height = 1024;

  // Create a window, add the tableau and show it on screen:
  vcl_string title = "Oxford RRG Video player - v0.5";
  vgui_window *win = vgui::produce_window(window_width, window_height,
                                          create_menubar(), title);
  win->get_adaptor()->set_tableau(xcv_tab);
  win->set_statusbar(true);
  win->enable_vscrollbar(false);
  win->enable_hscrollbar(false);
  win->show();
  return vgui::run();
}
