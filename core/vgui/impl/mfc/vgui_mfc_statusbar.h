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
  
  //: Constructor, creates an MFC status bar object and displays it.
  vgui_mfc_statusbar();
  //: Destructor.
 ~vgui_mfc_statusbar();

  //: Append given text (of given length) to the status bar
  int write(const char* text, int n);
  //: Append given text (of given length) to the status bar
  int write(const char* text);
  //: Updates the status bar by calling OnUpdateStatusBar of vgui_mfc_mainfrm
  void update();

  //: MFC status bar object
  CStatusBar *statusbar;
  //: Text displayed on the statusbar
  vcl_string linebuffer;
  vgui_statusbuf* statusbuf;
  vcl_ostream out;  
};

#endif // vgui_mfc_statusbar_h_
