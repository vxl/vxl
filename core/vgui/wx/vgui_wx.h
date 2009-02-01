// This is core/vgui/wx/vgui_wx.h
#ifndef vgui__wx_h_
#define vgui__wx_h_
//:
// \file
// \brief  vgui_wx static methods
// \author Matt Leotta
// \date   August 7, 2008
//

//: include impl stuff so the user doesn't have to
#include <vgui/impl/wx/vgui_wx_adaptor.h>
#include <vgui/impl/wx/vgui_wx_statusbar.h>


//: vgui_wx static methods
class vgui_wx
{
 public:
  //: Initialize all vgui_wx XML Resource Handlers
  // Call this right after you call wxXmlResource::Get()->InitAllHandlers();
  static void InitVguiHandlers();
};

#endif  //vgui__wx_h_
