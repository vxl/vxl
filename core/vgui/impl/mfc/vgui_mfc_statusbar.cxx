//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// Class: vgui_mfc_statusbar
// Author: Marko Bacic, RRG, University of Oxford
// Created: 10 Aug 2000
//
//-----------------------------------------------------------------------------

#include "vgui_mfc_statusbar.h"
#include "vgui_mfc_mainfrm.h" 
vgui_mfc_statusbar::vgui_mfc_statusbar()
  : statusbuf(new vgui_statusbuf(this))
  , out(statusbuf)
{
	// -- Create status bar object
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
// -- Updates the status bar by calling OnUpdateStatusBar of vgui_mfc_mainfrm
void vgui_mfc_statusbar::update()
{
   
         CCmdUI cui;
         cui.m_nID = ID_SEPARATOR;
         cui.m_nIndex = 4;
         cui.m_pMenu = NULL;
         cui.m_pOther = statusbar;

         ((vgui_mfc_mainfrm *)AfxGetApp()->GetMainWnd())->UpdateStatusBar(&cui);
      
}
int vgui_mfc_statusbar::write(const char* text, int n) {

  if(n == 1) {
    if (text[0] == '\n') {

	  statusbar->SetWindowText(linebuffer.c_str());
          statusbar->SetPaneText(0,linebuffer.c_str());
      linebuffer = "";
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


int vgui_mfc_statusbar::write(const char* text) {
  
	statusbar->SetWindowText(text);
        linebuffer = text;
        statusbar->SetPaneText(0,linebuffer.c_str());
	statusbar->UpdateWindow();
	statusbar->ShowWindow(SW_SHOW);
          update();

	return 1;
}
