#include "vgui_wx_app.h"
#include <iostream>
vgui_wx_app * vgui_wx_app::instance_ = nullptr;
vgui_wx_app *
vgui_wx_app::instance()
{
  if (!instance_)
    instance_ = new vgui_wx_app();
  return instance_;
}
void
vgui_wx_app::delete_instance()
{
  delete instance_;
  instance_ = nullptr;
}
vgui_wx_app::vgui_wx_app() {}

vgui_wx_app::~vgui_wx_app() {}
