// This is core/vgui/impl/mfc/vgui_mfc_statusbar.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief See vgui_mfc_statusbar.h for a description of this file.
// \author  Marko Bacic, RRG, University of Oxford
// \date    10 Aug 2000
//
// \verbatim
//  Modifications
//   13-MAR-2001 K.Y.McGaul   Messages are now cleared when a new message
//                            appears, not immediately.
// \endverbatim
//

#include "vgui_mfc_statusbar.h"
#include "vgui_mfc_mainfrm.h"

//: Constructor, creates an MFC status bar object and displays it.
vgui_mfc_statusbar::vgui_mfc_statusbar()
  : statusbuf(new vgui_statusbuf(this))
  , out(statusbuf)
{
        statusbar = new CStatusBar();
        statusbar->Create(AfxGetApp()->GetMainWnd());
  unsigned x = ID_SEPARATOR;
  statusbar->SetIndicators(&x,1);
  CFrameWnd *fwnd = (CFrameWnd *)AfxGetApp()->GetMainWnd();
  fwnd->ShowControlBar(statusbar,TRUE,FALSE);
  statusbar->UpdateWindow();
        statusbar->ShowWindow(SW_SHOW);
}

vgui_mfc_statusbar::~vgui_mfc_statusbar()
{
}

static int context_id = 1;

//: Updates the status bar by calling OnUpdateStatusBar of vgui_mfc_mainfrm
void vgui_mfc_statusbar::update()
{
  CCmdUI cui;
  cui.m_nID = ID_SEPARATOR;
  cui.m_nIndex = 4;
  cui.m_pMenu = NULL;
  cui.m_pOther = statusbar;

  ((vgui_mfc_mainfrm *)AfxGetApp()->GetMainWnd())->UpdateStatusBar(&cui);
}


//: Append given text (of given length) to the status bar
int vgui_mfc_statusbar::write(const char* text, int n)
{
  static bool start_new = false;
  if (n == 1) {
    if (text[0] == '\n') {
      // We are at the end of the message. Set a bool so we know next time
      // to clear the buffer before we start a new message.
      start_new = true;
    }
    else if (start_new == true){
      statusbar->SetWindowText(linebuffer.c_str());
      statusbar->SetPaneText(0,linebuffer.c_str());
      linebuffer = "";
      linebuffer += text[0];
      start_new = false;
    }
    else
      linebuffer += text[0];
  }
  else {
    linebuffer.append(text, n);
    if (linebuffer.find('\n')) {
            statusbar->SetWindowText(linebuffer.c_str());
      statusbar->SetPaneText(0,linebuffer.c_str());
      linebuffer = "";
    }
  }
  statusbar->UpdateWindow();
        statusbar->ShowWindow(SW_SHOW);
        AfxGetApp()->GetMainWnd()->UpdateWindow();
        AfxGetApp()->GetMainWnd()->ShowWindow(SW_SHOW);
  update();
  return n;
}

//: Write given text to the status bar.
int vgui_mfc_statusbar::write(const char* text)
{
        statusbar->SetWindowText(text);
  linebuffer = text;
  statusbar->SetPaneText(0,linebuffer.c_str());
        statusbar->UpdateWindow();
        statusbar->ShowWindow(SW_SHOW);
  update();

        return 1;
}
