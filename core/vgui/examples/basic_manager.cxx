// This is core/vgui/examples/basic_manager.cxx
#include "basic_manager.h"
#include <vcl_cstdlib.h> // for vcl_exit()
#include <vil1/vil1_load.h>
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_image_tableau.h>

basic_manager *basic_manager::instance()
{
  static basic_manager *instance_ = 0;
  if (!instance_)
  {
    instance_ = new basic_manager();
    instance_->init();
  }
  return instance_;
}

void basic_manager::init()
{
  itab_ = vgui_image_tableau_new();//keep the image tableau handy
  vgui_viewer2D_tableau_sptr viewer = vgui_viewer2D_tableau_new(itab_);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(viewer);
  this->add_child(shell);
}

//the event handler
bool basic_manager::handle(vgui_event const & e)
{
  if (e.key == 'b')
    vgui::out << "I saw a 'b'\n";
  //pass the event to the shell
  return this->child.handle(e);
}

void basic_manager::quit()
{
  vcl_exit(1);
}

void basic_manager::load_image()
{
  vgui_dialog load_image_dlg("Load image file");
  static vcl_string image_filename = "";
  static vcl_string ext = "*.*";
  load_image_dlg.file("Image Filename:", ext, image_filename);
  if (!load_image_dlg.ask())
    return;
  img_ = vil1_load(image_filename.c_str());
  itab_->set_image(img_);
}
