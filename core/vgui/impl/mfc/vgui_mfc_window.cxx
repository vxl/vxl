//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// .NAME vgui_mfc_window
// Author: Marko Bacic, Oxford RRG
// Created: 24 July 2000
//
//-----------------------------------------------------------------------------
#include "vgui_mfc_window.h"
#include "vgui_mfc_adaptor.h"
#include "vgui_mfc_mainfrm.h"
#include "vgui_mfc.h"
#include "vgui_mfc_utils.h"
#include "vgui_mfc_statusbar.h"
#include <vgui/vgui.h>

vgui_mfc_window::vgui_mfc_window(char const *title,
                                 unsigned w, unsigned h,
                                 int posx,
                                 int posy)
  : vgui_window()
  , mfcwin(0)
{
  CWinApp *app = AfxGetApp();
  app->GetMainWnd()->SetWindowText(title);
  POSITION pos = app->GetFirstDocTemplatePosition();
  CDocTemplate *tmpl = app->GetNextDocTemplate(pos);
  pos = tmpl->GetFirstDocPosition();
  CDocument *pdoc = tmpl->GetNextDoc(pos);
  pos = pdoc->GetFirstViewPosition();
  mfcwin = (vgui_mfc_adaptor *)pdoc->GetNextView(pos);
  statusbar = new vgui_mfc_statusbar();
  CWnd *main_wnd = app->GetMainWnd();
  ((vgui_mfc_mainfrm *)main_wnd)->SetStatusBar(statusbar);
  vgui::out.rdbuf(statusbar->statusbuf);
  // This part of code changes the window size to the one requested
  WINDOWPLACEMENT w_placement;
  // Obtain window geometry information
  main_wnd->GetWindowPlacement(&w_placement);
  // Modify bottom right corner coordinates to account for the width
  w_placement.rcNormalPosition.right = w_placement.rcNormalPosition.left+w;
  w_placement.rcNormalPosition.bottom = w_placement.rcNormalPosition.top+h;
  // Store the geometry information back into window
  main_wnd->SetWindowPlacement(&w_placement);
}

vgui_mfc_window::vgui_mfc_window(unsigned w, unsigned h,
                                 vgui_menu const &menubar,
                                 char const *title)
  : vgui_window()
  , mfcwin(0)
{
  CWinApp *app = AfxGetApp();
  app->GetMainWnd()->SetWindowText(title);
  POSITION pos = app->GetFirstDocTemplatePosition();
  CDocTemplate *tmpl = app->GetNextDocTemplate(pos);
  pos = tmpl->GetFirstDocPosition();
  CDocument *pdoc = tmpl->GetNextDoc(pos);
  pos = pdoc->GetFirstViewPosition();
  mfcwin = (vgui_mfc_adaptor *)pdoc->GetNextView(pos);
  vgui_mfc::instance()->utils->set_menu(menubar);
  statusbar = new vgui_mfc_statusbar();
  CWnd *main_wnd = app->GetMainWnd();
  ((vgui_mfc_mainfrm *)main_wnd)->SetStatusBar(statusbar);
  vgui::out.rdbuf(statusbar->statusbuf);
//vgui::out<<"Welcome to XCV revision 1.0a";
  // This part of code changes the window size to the one requested
  WINDOWPLACEMENT w_placement;
  // Obtain window geometry information
  main_wnd->GetWindowPlacement(&w_placement);
  // Modify bottom right corner coordinates to account for the width
  w_placement.rcNormalPosition.right = w_placement.rcNormalPosition.left+w;
  w_placement.rcNormalPosition.bottom = w_placement.rcNormalPosition.top+h;
  // Store the geometry information back into window
  main_wnd->SetWindowPlacement(&w_placement);
}


vgui_mfc_window::~vgui_mfc_window() {
  //delete mfcwin;
  mfcwin = 0;
}

vgui_adaptor* vgui_mfc_window::get_adaptor() {
  return mfcwin;
}

void vgui_mfc_window::show() {
}

void vgui_mfc_window::hide() {
}

void vgui_mfc_window::enable_hscrollbar(bool show)
{
        CWnd *wnd = AfxGetApp()->GetMainWnd();
        wnd->ShowScrollBar(SB_HORZ,show);
}

void vgui_mfc_window::enable_vscrollbar(bool show)
{
        CWnd *wnd = AfxGetApp()->GetMainWnd();
        wnd->ShowScrollBar(SB_VERT,show);
}

void vgui_mfc_window::iconify() {
}

void vgui_mfc_window::reshape(unsigned w, unsigned h) {
}

void vgui_mfc_window::reposition(int x,int y) {
}

void vgui_mfc_window::set_title(const vcl_string &s) {
}
