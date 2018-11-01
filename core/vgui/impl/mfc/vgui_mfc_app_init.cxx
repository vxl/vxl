// This is core/vgui/impl/mfc/vgui_mfc_app_init.cxx
// See vgui_mfc_app_init.h for a description of this file.
//
#include "vgui_mfc_app_init.h"

#include <vgui/impl/mfc/StdAfx.h>
#include <vgui/impl/mfc/vgui_mfc_app.h>


vgui_mfc_app_init::vgui_mfc_app_init()
{
  p = new vgui_mfc_app;
}

vgui_mfc_app_init::~vgui_mfc_app_init()
{
  delete p;
}
