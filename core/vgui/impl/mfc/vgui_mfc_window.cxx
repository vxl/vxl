// This is ./oxl/vgui/impl/mfc/vgui_mfc_window.cxx

//:
// \file
// \author  Marko Bacic, Oxford RRG
// \date    24 July 2000
// \brief   See vgui_mfc_window.h for a description of this file.
//
// \verbatim
//  Modifications:
//    20-JUL-2001  K.Y.McGaul  Added init_window function carry out common
//                             functionality from all constructors.
// \endverbatim

#ifdef __GNUC__
#pragma implementation
#endif

#include "vgui_mfc_window.h"
#include "vgui_mfc_adaptor.h"
#include "vgui_mfc_mainfrm.h"
#include "vgui_mfc.h"
#include "vgui_mfc_utils.h"
#include "vgui_mfc_statusbar.h"
#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>

static bool first_window = true;

vgui_mfc_window::init_window(char const *title,
                             vgui_menu const &menubar,
                             bool has_menu,
                             unsigned width, unsigned height,
                             int posx,
                             int posy)
{
 //if (first_window) // First time use the main window
  {
    // Get a pointer to the single CWinApp object for the application:
    CWinApp *app = AfxGetApp();
    app->GetMainWnd()->SetWindowText(title);
    pos1 = app->GetFirstDocTemplatePosition();
    CDocTemplate *tmpl = app->GetNextDocTemplate(pos1);
    pos2 = tmpl->GetFirstDocPosition();
    CDocument *pdoc = tmpl->GetNextDoc(pos2);
    pos3 = pdoc->GetFirstViewPosition();
    mfcwin = (vgui_mfc_adaptor *)pdoc->GetNextView(pos3);
    if (has_menu)
      vgui_mfc::instance()->utils->set_menu(menubar);
    statusbar = new vgui_mfc_statusbar();
    CWnd *main_wnd = app->GetMainWnd();
    ((vgui_mfc_mainfrm *)main_wnd)->SetStatusBar(statusbar);
    vgui::out.rdbuf(statusbar->statusbuf);
    // This part of code changes the window size to the one requested
    WINDOWPLACEMENT w_placement;
    // Obtain window geometry information
    main_wnd->GetWindowPlacement(&w_placement);
    // Modify bottom right corner coordinates to account for the width
    w_placement.rcNormalPosition.right = w_placement.rcNormalPosition.left+width;
    w_placement.rcNormalPosition.bottom = w_placement.rcNormalPosition.top+height;
    // Store the geometry information back into window
    main_wnd->SetWindowPlacement(&w_placement);
    first_window = false;
  }
}

//: Constructor for window without menubar.
vgui_mfc_window::vgui_mfc_window(char const *title,
                                 unsigned width, unsigned height,
                                 int posx,
                                 int posy)
  : vgui_window()
  , mfcwin(0)
{
  init_window(title, vgui_menu(), false, width, height, posx, posy);
}

//: Constructor for window with menubar.
vgui_mfc_window::vgui_mfc_window(unsigned width, unsigned height,
                                 vgui_menu const &menubar,
                                 char const *title)
  : vgui_window()
  , mfcwin(0)
{
  init_window(title, menubar, true, width, height, 0, 0);
}

//: Destructor
vgui_mfc_window::~vgui_mfc_window() {
  glFlush();
  delete mfcwin;
  mfcwin = 0;
}

//: Return the vgui_mfc_adaptor associated with this window.
vgui_adaptor* vgui_mfc_window::get_adaptor()
{
  return mfcwin;
}

void vgui_mfc_window::show()
{
}

void vgui_mfc_window::hide()
{
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

void vgui_mfc_window::iconify()
{
}

void vgui_mfc_window::reshape(unsigned w, unsigned h)
{
}

void vgui_mfc_window::reposition(int x,int y)
{
}

void vgui_mfc_window::set_title(const vcl_string &s)
{
 CWinApp *app = AfxGetApp();
 app->GetMainWnd()->SetWindowText(s.c_str());
}
