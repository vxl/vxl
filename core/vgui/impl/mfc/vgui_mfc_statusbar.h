#ifndef vgui_mfc_statusbar_h_
#define vgui_mfc_statusbar_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_mfc_statusbar
// .LIBRARY vgui-mfc
// .HEADER vxl Package
// .INCLUDE vgui/impl/mfc/vgui_mfc_statusbar.h
// .FILE vgui_mfc_statusbar.cxx
//
// .SECTION Description:
//   
//
// .SECTION Author:
//              Marko Bacic, 08 Aug 2000
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vcl_string.h>
#include <vcl_iosfwd.h>
#include <vgui/vgui_statusbuf.h>
#include <vgui/vgui_statusbar.h>
#include "stdafx.h"
class vgui_mfc_statusbar : public vgui_statusbar {
public:
  
  // Constructors/Destructors--------------------------------------------------
  
  vgui_mfc_statusbar();
 ~vgui_mfc_statusbar();

  int write(const char* text, int n);
  int write(const char* text);
  void update();
  CStatusBar *statusbar;
  vcl_string linebuffer;
  vgui_statusbuf* statusbuf;
  ostream out;  
};

#endif // vgui_mfc_statusbar_h_
