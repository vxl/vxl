// This is core/vgui/wx/vgui_wx.cxx
#include "vgui_wx.h"
//:
// \file
#include <wx/xrc/xmlres.h>

// Includes for XML Resource Handlers
#include "vgui_wx_adaptorXmlHandler.h"
#include "wxSliderPanelXmlHandler.h"
#include "wxVideoControlXmlHandler.h"


//: Initialize all vgui_wx XML Resource Handlers
// Call this right after you call wxXmlResource::Get()->InitAllHandlers();
void vgui_wx::InitVguiHandlers()
{
  wxXmlResource::Get()->AddHandler(new vgui_wx_adaptorXmlHandler);
  wxXmlResource::Get()->AddHandler(new wxSliderPanelXmlHandler);
  wxXmlResource::Get()->AddHandler(new wxVideoControlXmlHandler);
  // Add new resource handlers here
}
