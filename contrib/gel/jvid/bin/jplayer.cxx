#include <vcl_ios.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for vcl_exit()

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/internals/vgui_accelerate.h>

#ifdef HAS_X11
#include <vgui/internals/vgui_accelerate_x11.h>
#endif

#include <vidl/vidl_io.h>
#include <vidl/vidl_avicodec.h>

#include <jvid/jvx_manager.h>

//-----------------------------------------------------------------------------
// External refs
//-----------------------------------------------------------------------------

#ifdef HAS_MFC
#include <vgui/impl/mfc/vgui_mfc_app_init.h>
extern int vgui_accelerate_mfc_tag_function();
extern int vgui_mfc_tag_function();
vgui_mfc_app_init theAppinit;
#endif

#ifdef HAS_X11
extern int vgui_accelerate_x11_tag_function();
#endif

#ifdef HAS_GTK
extern int vgui_gtk_tag_function();
#endif

#ifdef HAS_GLUT
extern int vgui_glut_tag_function();
#endif
static jvx_manager* jvm = new jvx_manager();

void quit_callback()
{
  vcl_exit(1);
}

void file_callback()
{
  jvm->load_video_file();
}
void play_callback()
{
  jvm->play_video();
}
int main(int argc, char** argv)
{
  // Register video codec
  vidl_io::register_codec(new vidl_avicodec);

#ifdef HAS_GTK
  vgui_gtk_tag_function();
#endif

#ifdef HAS_GLUT
  vgui_glut_tag_function();
#endif

#ifdef HAS_MFC
  vgui_mfc_tag_function();
  vgui_accelerate_mfc_tag_function();
#endif

   // turn off the mfc acceleration since this seems to stop
   // us from doing the double buffering. It also seems to add
   // a certain amount of overhead

  vgui_accelerate::vgui_mfc_acceleration = false;

  vcl_cout << "Joe Video\n";

   // Initialize the toolkit.
  vgui::init(argc, argv);

  vgui_menu menubar;
  vgui_menu menufile;

  menufile.add( "Quit", new vgui_command_cfunc( quit_callback ));
  menufile.add( "Load", new vgui_command_cfunc( file_callback ));
  menufile.add( "Play", new vgui_command_cfunc( play_callback ));
  menubar.add( "File", menufile);
  // Initialize the window
  unsigned w = 400, h = 340;

  jvm->set_grid_size_changeable(true);
  vcl_string title = "Joe Video Player";
  vgui_window *win = vgui::produce_window(w, h, menubar, title);
  win->get_adaptor()->set_tableau(jvm);
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  jvm->post_redraw();
  return  vgui::run();
}
